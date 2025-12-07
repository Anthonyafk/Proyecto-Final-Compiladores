#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"

// Variables globales
int label_count = 0;
int temp_count = 0;

void generar_codigo(Nodo* n);
char* generar_expresion(Nodo* n);

// Variables globales en codegen.c
typedef struct ArrayInfo {
    char* nombre;
    int size_actual;
    struct ArrayInfo* siguiente;
} ArrayInfo;

ArrayInfo* arrays_info = NULL;

// Función para obtener/crear info de arreglo
ArrayInfo* obtener_array_info(char* nombre) {
    ArrayInfo* actual = arrays_info;
    while (actual) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    // Crear nuevo
    ArrayInfo* nuevo = (ArrayInfo*)malloc(sizeof(ArrayInfo));
    nuevo->nombre = strdup(nombre);
    nuevo->size_actual = 0;
    nuevo->siguiente = arrays_info;
    arrays_info = nuevo;
    return nuevo;
}

char* safe_strdup(const char* s) {
    if (!s) return NULL;
    char* copy = (char*)malloc(strlen(s) + 1);
    if (copy) strcpy(copy, s);
    return copy;
}

char* nuevo_temp() {
    static char temp[10];
    sprintf(temp, "T%d", temp_count++);
    return safe_strdup(temp);
}

int nueva_etiqueta() { 
    return label_count++; 
}

// Generación de las expresiones
char* generar_expresion(Nodo* n) {
    if (!n) return NULL;
    char* t = nuevo_temp();
    char *t1, *t2;

    switch (n->tipo) {
        case NODO_NUMERO: 
            printf("ASSIGN %d %s\n", n->valor_int, t); 
            return t;

        case NODO_DECIMAL: 
            printf("ASSIGN %f %s\n", n->valor_dec, t); 
            return t;

        case NODO_ID:     
            printf("ASSIGN %s %s\n", n->nombre, t); 
            return t;

        case NODO_CADENA: 
            printf("ASSIGN %s %s\n", n->texto, t); 
            return t;

        case NODO_BOOLEANO: 
            printf("ASSIGN %d %s\n", n->valor_bool, t); 
            return t;

        case NODO_ENTRADA: 
            printf("INPUT %s\n", t); 
            return t;

        case NODO_OBTENER_TECLA:
            t1 = generar_expresion(n->izq);
            printf("KEY %s %s\n", t1, t);
            return t;

        case NODO_BINARIO:
            t1 = generar_expresion(n->izq);
            t2 = generar_expresion(n->der);
            
            if (strcmp(n->operador, "+") == 0) {
                printf("ADD %s %s %s\n", t1, t2, t);
            }
            else if (strcmp(n->operador, "-") == 0) {
                printf("SUB %s %s %s\n", t1, t2, t);
            }
            else if (strcmp(n->operador, "*") == 0) {
                printf("MUL %s %s %s\n", t1, t2, t);
            }
            else if (strcmp(n->operador, "/") == 0) {
                // División entera trunca el resultado forzando a entero, restando la parte decimal
                char* div_result = nuevo_temp();
                char* one = nuevo_temp();
                char* frac = nuevo_temp();
                
                printf("DIV %s %s %s\n", t1, t2, div_result);
                printf("ASSIGN 1 %s\n", one);
                printf("MOD %s %s %s\n", div_result, one, frac);  // Obtener parte decimal
                printf("SUB %s %s %s\n", div_result, frac, t);    // Restar para obtener entero
            }
            else if (strcmp(n->operador, "DIV") == 0) {
                // División DECIMAL: usa DIV directamente
                printf("DIV %s %s %s\n", t1, t2, t);
            }
            else if (strcmp(n->operador, "%") == 0) {
                printf("MOD %s %s %s\n", t1, t2, t);
            }
            return t;

        case NODO_COMPARACION:
            t1 = generar_expresion(n->izq);
            t2 = generar_expresion(n->der);
            if (strcmp(n->operador, "==") == 0) 
                printf("EQ %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "!=") == 0)
                printf("NEQ %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "<") == 0) 
                printf("LT %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, ">") == 0) 
                printf("GT %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "<=") == 0) 
                printf("LTE %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, ">=") == 0) 
                printf("GTE %s %s %s\n", t1, t2, t);
            return t;
            
        case NODO_LOGICO:
            if (strcmp(n->operador, "No") == 0) {
                t1 = generar_expresion(n->izq);
                printf("EQ %s 0 %s\n", t1, t);
            } else {
                t1 = generar_expresion(n->izq);
                t2 = generar_expresion(n->der);
                if (strcmp(n->operador, "Y") == 0) {
                    char* temp_mul = nuevo_temp();
                    printf("MUL %s %s %s\n", t1, t2, temp_mul);
                    printf("NEQ %s 0 %s\n", temp_mul, t);
                } else if (strcmp(n->operador, "O") == 0) {
                    char* temp_add = nuevo_temp();
                    printf("ADD %s %s %s\n", t1, t2, temp_add);
                    printf("NEQ %s 0 %s\n", temp_add, t);
                }
            }
            return t;
/*        case NODO_LLAMADA_FUNC:
            // Apilar argumentos con PARAM antes de GOSUB
            Nodo* arg = n->argumentos;
            int num_args = 0;
            // Contar argumentos primero
            Nodo* temp_arg = arg;
            while (temp_arg) {
                num_args++;
                temp_arg = temp_arg->siguiente;
            }
            // Apilar argumentos en orden
            arg = n->argumentos;
            while (arg) {
                char* temp_arg_val = generar_expresion(arg);
                printf("PARAM %s\n", temp_arg_val);
                arg = arg->siguiente;
            }
            // Llamar a la subrutina
            printf("GOSUB %s\n", n->nombre);
            return t;
*/
        case NODO_LLAMADA_FUNC: {
            // Apilar argumentos
            Nodo* arg = n->argumentos;
            while (arg) {
                char* temp_arg_val = generar_expresion(arg);
                printf("PARAM %s\n", temp_arg_val);
                arg = arg->siguiente;
            }
            // Llamar función
            printf("GOSUB %s\n", n->nombre);
            printf("POP_RESULT %s\n", t);
            return t;
        }

        case NODO_ACCESO_ARRAY: {
            // arr[indice]
            char* idx = generar_expresion(n->indice);
            char* t = nuevo_temp();

            // Verificar rango
            char* t_valid = nuevo_temp();
            printf("LT %s %s_size %s\n", idx, n->nombre, t_valid);

            int L_safe = nueva_etiqueta();
            int L_error = nueva_etiqueta();
            int L_end = nueva_etiqueta();

            printf("IF %s GOTO L%d\n", t_valid, L_safe);

            printf("LABEL L%d\n", L_error);
            printf("PRINT \"Error: Indice fuera de rango\"\n");
            printf("ASSIGN 0 %s\n", t);
            printf("GOTO L%d\n", L_end);

            printf("LABEL L%d\n", L_safe);
            // Acceso directo a la variable
            printf("ASSIGN %s_%d %s\n", n->nombre, n->indice->valor_int, t);

            printf("LABEL L%d\n", L_end);
            return t;
        }
        case NODO_LONGITUD: {
            char* t = nuevo_temp();
            if (n->izq && n->izq->tipo == NODO_ID) {
                printf("ASSIGN %s_size %s\n", n->izq->nombre, t);
            }
            return t;
        }
        case NODO_TAMANO_ARRAY: {
            char* t = nuevo_temp();
            printf("ASSIGN %s_size %s\n", n->nombre, t);
            return t;
        }
        default:
            return t;
    }
}

// Recorre lista de instrucciones
void generar_bloque(Nodo* n) {
    Nodo* actual = n;
    while (actual) {
        generar_codigo(actual);
        actual = actual->siguiente;
    }
}

// Generación de instrucciones
void generar_codigo(Nodo* n) {
    if (!n) return;

    switch (n->tipo) {
        // Estructura del programa
        case NODO_PROGRAMA:
            generar_bloque(n->siguiente); 
            break;

        case NODO_BLOQUE:
            generar_bloque(n); // Recorremos el bloque interno
            break;

        // Variables y Asignaciones
        case NODO_VAR_DECL:
            if (n->es_arreglo) {
                // Declarar puntero base del arreglo
                printf("VAR %s\n", n->nombre);

                // Declarar variable para el tamaño
                printf("VAR %s_size\n", n->nombre);
                printf("ASSIGN 0 %s_size\n", n->nombre);

                // Obtener info del arreglo
                ArrayInfo* arr_info = obtener_array_info(n->nombre);

                // Si tiene inicialización con literal [1, 2, 3]
                if (n->izq && n->izq->tipo == NODO_ARREGLO) {
                    Nodo* elemento = n->izq->izq;
                    int index = 0;

                    // Reservar memoria para cada elemento
                    while (elemento) {
                        // Crear variable temporal para cada posición
                        printf("VAR %s_%d\n", n->nombre, index);

                        char* t = generar_expresion(elemento);
                        printf("ASSIGN %s %s_%d\n", t, n->nombre, index);

                        index++;
                        elemento = elemento->siguiente;
                    }

                    // Actualizar tamaño
                    printf("ASSIGN %d %s_size\n", index, n->nombre);
                    arr_info->size_actual = index;
                }
            } else {
                // Declaración normal de variable
                printf("VAR %s\n", n->nombre);
                if (n->izq) {
                    char* t = generar_expresion(n->izq);
                    printf("ASSIGN %s %s\n", t, n->nombre);
                }
            }
            break;

        case NODO_ASIGNACION_ARRAY: {
            // arr[indice] = valor
            char* idx = generar_expresion(n->indice);
            char* val = generar_expresion(n->izq);

            // Comentario para claridad
            //printf("// %s[%s] = %s\n", n->nombre, idx, val);

            // Verificar que el índice esté en rango
            char* t_valid = nuevo_temp();
            printf("LT %s %s_size %s\n", idx, n->nombre, t_valid);

            int L_safe = nueva_etiqueta();
            int L_end = nueva_etiqueta();
            printf("IF %s GOTO L%d\n", t_valid, L_safe);
            printf("PRINT \"Error: Indice fuera de rango\"\n");
            printf("GOTO L%d\n", L_end);

            printf("LABEL L%d\n", L_safe);
            // Asignar directamente a la variable correspondiente
            printf("ASSIGN %s %s_%d\n", val, n->nombre, n->indice->valor_int);
            printf("LABEL L%d\n", L_end);

            break;
        }
        
        case NODO_AGREGAR: {
            // agregar(arr, valor)
            char* val = generar_expresion(n->izq);
            //printf("// agregar(%s, %s)\n", n->nombre, val);

            // Obtener info del arreglo
            ArrayInfo* arr_info = obtener_array_info(n->nombre);
            int nuevo_indice = arr_info->size_actual;

            // Crear nueva variable con el índice correcto
            printf("VAR %s_%d\n", n->nombre, nuevo_indice);
            printf("ASSIGN %s %s_%d\n", val, n->nombre, nuevo_indice);

            // Incrementar tamaño
            arr_info->size_actual++;
            printf("ASSIGN %d %s_size\n", arr_info->size_actual, n->nombre);
            break;
        }
        case NODO_ELIMINAR: {
            // eliminar(arr, indice)
            //char* idx = generar_expresion(n->indice);
            //printf("// eliminar(%s, índice en %s)\n", n->nombre, idx);
            
            // Obtener info del arreglo
            ArrayInfo* arr_info = obtener_array_info(n->nombre);
            
            // Verificar rango
            char* t_valid = nuevo_temp();
            printf("LT %d %s_size %s\n", n->indice->valor_int, n->nombre, t_valid);

            int L_valid = nueva_etiqueta();
            int L_end = nueva_etiqueta();

            printf("IF %s GOTO L%d\n", t_valid, L_valid);
            printf("PRINT \"Error: Indice fuera de rango\"\n");
            printf("GOTO L%d\n", L_end);

            printf("LABEL L%d\n", L_valid);

            for (int i = 0; i < arr_info->size_actual; i++) {
                //int L_check = nueva_etiqueta();
                int L_next = nueva_etiqueta();
        
                char* cond = nuevo_temp();
                printf("EQ %d %d %s\n", n->indice->valor_int, i, cond);
                printf("IFFALSE %s GOTO L%d\n", cond, L_next);
        
                // Desplazar todos los elementos siguientes
                for (int j = i; j < arr_info->size_actual - 1; j++) {
                    printf("ASSIGN %s_%d %s_%d\n", n->nombre, j+1, n->nombre, j);
                }
                //printf("GOTO L%d\n", L_next);
                printf("LABEL L%d\n", L_next);
            }
    
            // Decrementar tamaño
            arr_info->size_actual--;
            printf("ASSIGN %d %s_size\n", arr_info->size_actual, n->nombre);

            printf("LABEL L%d\n", L_end);
            break;
        }

        case NODO_ASIGNACION: {
            char* t = generar_expresion(n->izq);
            printf("ASSIGN %s %s\n", t, n->nombre);
            break;
        }

        // Control de flujo
        case NODO_MIENTRAS: {
            int L1 = nueva_etiqueta();
            int L2 = nueva_etiqueta();
            printf("LABEL L%d\n", L1);
            char* c = generar_expresion(n->condicion);
            printf("IFFALSE %s GOTO L%d\n", c, L2);
            generar_bloque(n->der); // Cuerpo
            printf("GOTO L%d\n", L1);
            printf("LABEL L%d\n", L2);
            break;
        }

        case NODO_SI: {
            int L_FIN = nueva_etiqueta();
            int L_SINO = nueva_etiqueta();
            char* c = generar_expresion(n->condicion);
            
            if (n->sino) {
                printf("IFFALSE %s GOTO L%d\n", c, L_SINO);
                generar_bloque(n->der);
                printf("GOTO L%d\n", L_FIN);
                printf("LABEL L%d\n", L_SINO);
                generar_bloque(n->sino);
            } else {
                printf("IFFALSE %s GOTO L%d\n", c, L_FIN);
                generar_bloque(n->der);
            }
            printf("LABEL L%d\n", L_FIN);
            break;
        }

        // Funciones nativas
        case NODO_IMPRIMIR: {
             char* t = generar_expresion(n->izq);
             printf("PRINT %s\n", t);
             break;
        }

        case NODO_PIXEL: {
            char* x = generar_expresion(n->izq);
            char* y = generar_expresion(n->der);
            char* c = generar_expresion(n->extra);
            printf("PIXEL %s %s %s\n", x, y, c);
            break;
        }

        case NODO_TECLA: {
            char* k = generar_expresion(n->izq);
            printf("KEY %s %s\n", k, n->nombre);
            break;
        }
        
        case NODO_RETURN:
            if(n->izq) {
                char* t = generar_expresion(n->izq);
                printf("// RETURN %s\n", t);
            }
            printf("RETURN\n");
            break;

        // Casos que no generan código por sí mismos
        case NODO_FUNCION: 
            printf("\n// Función: %s\n", n->nombre);
            printf("LABEL %s\n", n->nombre);

            // Recuperamos parámetros
            Nodo* param = n->parametros;
            int param_idx = 0;
            while (param) {
                printf("VAR %s\n", param->nombre);
                printf("PARAM_GET %d %s\n", param_idx, param->nombre);
                param_idx++;
                param = param->siguiente;
            }

            // Generamos cuerpo de la función
            generar_bloque(n->cuerpo);

            // Si no hay return explícito, lo agregamos
            printf("RETURN\n\n");
            break;
            
        case NODO_LLAMADA_FUNC:
            generar_expresion(n);
            break;
            
        default:
            break; 
    }
    fflush(stdout);
}

void generar_codigo_programa(Nodo* raiz) {
    printf("// Código Intermedio FIS-25\n");
    generar_codigo(raiz); // Inicia la recursión

}
