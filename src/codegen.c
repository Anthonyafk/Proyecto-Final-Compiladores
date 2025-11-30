#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"

// --- VARIABLES GLOBALES ---
int label_count = 0;
int temp_count = 0;

// --- PROTOTIPOS (Esto soluciona el error de conflicting types) ---
void generar_codigo(Nodo* n);
char* generar_expresion(Nodo* n);

// --- FUNCIONES AUXILIARES ---
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

// --- GENERACIÓN DE EXPRESIONES (Retorna T0, T1...) ---
char* generar_expresion(Nodo* n) {
    if (!n) return NULL;
    char* t = nuevo_temp();
    char *t1, *t2;

    switch (n->tipo) {
        case NODO_NUMERO: printf("ASSIGN %d %s\n", n->valor_int, t); return t;
        case NODO_DECIMAL: printf("ASSIGN %f %s\n", n->valor_dec, t); return t;
        case NODO_ID:     printf("ASSIGN %s %s\n", n->nombre, t); return t;
        case NODO_CADENA: printf("ASSIGN %s %s\n", n->texto, t); return t;
        case NODO_BOOLEANO: printf("ASSIGN %d %s\n", n->valor_bool, t); return t;

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
            if (strcmp(n->operador, "+") == 0) printf("ADD %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "-") == 0) printf("SUB %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "*") == 0) printf("MUL %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "/") == 0) printf("DIV %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "%") == 0) printf("MOD %s %s %s\n", t1, t2, t);
            return t;

        case NODO_COMPARACION:
            t1 = generar_expresion(n->izq);
            t2 = generar_expresion(n->der);
            if (strcmp(n->operador, "==") == 0) printf("EQ %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "!=") == 0) printf("NEQ %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "<") == 0) printf("LT %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, ">") == 0) printf("GT %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, "<=") == 0) printf("LTE %s %s %s\n", t1, t2, t);
            else if (strcmp(n->operador, ">=") == 0) printf("GTE %s %s %s\n", t1, t2, t);
            return t;
            
        case NODO_LOGICO:
            // Implementación simplificada para Y / O
            //t1 = generar_expresion(n->izq);
            //t2 = generar_expresion(n->der); // Puede ser NULL si es un NOT
            // Aquí iría la lógica específica, retornamos t1 por simplicidad en debug
            //return t1;
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
        case NODO_LLAMADA_FUNC:
            // Según FIS-25: Apilar argumentos con PARAM antes de GOSUB
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

            // El valor de retorno (si hay) queda implícito
            // Por simplicidad, asumimos que se puede usar un temporal
            return t;

        default:
            return t;
    }
}

// --- HELPER: Recorrer lista de instrucciones ---
void generar_bloque(Nodo* n) {
    Nodo* actual = n;
    while (actual) {
        generar_codigo(actual);
        actual = actual->siguiente;
    }
}

// --- GENERACIÓN DE INSTRUCCIONES ---
void generar_codigo(Nodo* n) {
    if (!n) return;

    switch (n->tipo) {
        // 1. Estructura del programa
        case NODO_PROGRAMA:
            // Corregido: Ya no usamos 'raiz', usamos 'n->siguiente' que viene del parser
            generar_bloque(n->siguiente); 
            break;

        case NODO_BLOQUE:
            generar_bloque(n); // Recorrer el bloque interno
            break;

        // 2. Variables y Asignaciones
        case NODO_VAR_DECL:
            printf("VAR %s\n", n->nombre);
            if (n->izq) {
                char* t = generar_expresion(n->izq);
                printf("ASSIGN %s %s\n", t, n->nombre);
            }
            break;

        case NODO_ASIGNACION: {
            char* t = generar_expresion(n->izq);
            printf("ASSIGN %s %s\n", t, n->nombre);
            break;
        }

        // 3. Control de Flujo
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

        // 4. Funciones Nativas
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

        // 5. Casos que no generan código por sí mismos (son parte de otros)
        case NODO_FUNCION: 
            // Si tuvieras funciones reales, aquí generarías LABEL func_name
            //generar_bloque(n->cuerpo);
            //break;
            // Generar etiqueta para la función
            printf("\n// Función: %s\n", n->nombre);
            printf("LABEL %s\n", n->nombre);

            // Recuperar parámetros con PARAM_GET
            Nodo* param = n->parametros;
            int param_idx = 0;
            while (param) {
                printf("VAR %s\n", param->nombre);
                printf("PARAM_GET %d %s\n", param_idx, param->nombre);
                param_idx++;
                param = param->siguiente;
            }

            // Generar cuerpo de la función
            generar_bloque(n->cuerpo);

            // Si no hay RETURN explícito, agregarlo
            printf("RETURN\n\n");
            break;
            
        case NODO_LLAMADA_FUNC:
            generar_expresion(n);
            break;

        // Silenciar warnings para nodos que son expresiones, no instrucciones
        default:
            break; 
    }
    fflush(stdout);
}

void generar_codigo_programa(Nodo* raiz) {
    generar_codigo(raiz); // Inicia la recursión
    printf("HALT\n");
}
