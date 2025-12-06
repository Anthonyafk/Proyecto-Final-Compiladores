#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"

// Tabla de símbolos
typedef struct Simbolo {
    char* nombre;
    TipoDato tipo;
    struct Simbolo* siguiente;
} Simbolo;

Simbolo* tabla_simbolos = NULL;
int errores_semanticos = 0;

void declarar_variable(char* nombre, TipoDato tipo) {
    Simbolo* s = (Simbolo*)malloc(sizeof(Simbolo));
    s->nombre = strdup(nombre);
    s->tipo = tipo;
    s->siguiente = tabla_simbolos;
    tabla_simbolos = s;
}

Simbolo* buscar_simbolo(char* nombre) {
    Simbolo* actual = tabla_simbolos;
    while (actual) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

void error_semantico(const char* msg, const char* detalle) {
    fprintf(stderr, "Error Semántico: %s", msg);
    if (detalle && strlen(detalle) > 0) {
        fprintf(stderr, " [%s]", detalle);
    }
    fprintf(stderr, "\n");
    errores_semanticos++;
}

// Determina qué tipo de dato resulta de una expresión
TipoDato obtener_tipo(Nodo* n) {
    if (!n) return TIPO_DATO_NULO;

    switch (n->tipo) {
        case NODO_NUMERO: 
            return TIPO_DATO_INT;
            
        case NODO_DECIMAL: 
            return TIPO_DATO_FLOAT;
            
        case NODO_CADENA: 
            return TIPO_DATO_STRING;
            
        case NODO_BOOLEANO: 
            return TIPO_DATO_BOOL;
        
        case NODO_ID: {
            Simbolo* s = buscar_simbolo(n->nombre);
            if (s) return s->tipo;
            error_semantico("Variable no declarada", n->nombre);
            return TIPO_DATO_NULO;
        }

        case NODO_BINARIO: {
            TipoDato t1 = obtener_tipo(n->izq);
            TipoDato t2 = obtener_tipo(n->der);
            
            // Si alguno es nulo, ya hubo error previo
            if (t1 == TIPO_DATO_NULO || t2 == TIPO_DATO_NULO) {
                return TIPO_DATO_NULO;
            }
            
            // División entera ENTERO / ENTERO
            if (strcmp(n->operador, "/") == 0) {
                if (t1 == TIPO_DATO_INT && t2 == TIPO_DATO_INT) {
                    return TIPO_DATO_INT;
                } else {
                    error_semantico("El operador / (división entera) solo funciona con ENTERO / ENTERO", 
                                  "Use DIV para división con decimales");
                    return TIPO_DATO_NULO;
                }
            }
            
            // División decimal
            if (strcmp(n->operador, "DIV") == 0) {
                if ((t1 == TIPO_DATO_INT || t1 == TIPO_DATO_FLOAT) &&
                    (t2 == TIPO_DATO_INT || t2 == TIPO_DATO_FLOAT)) {
                    return TIPO_DATO_FLOAT; // División decimal siempre produce FLOAT
                } else {
                    error_semantico("DIV requiere operandos numéricos (ENTERO o DECIMAL)", "");
                    return TIPO_DATO_NULO;
                }
            }
            
            // MÓDULO: solo funciona con enteros
            if (strcmp(n->operador, "%") == 0) {
                if (t1 == TIPO_DATO_INT && t2 == TIPO_DATO_INT) {
                    return TIPO_DATO_INT;
                } else {
                    error_semantico("El operador % (módulo) solo funciona con ENTERO % ENTERO", "");
                    return TIPO_DATO_NULO;
                }
            }

            // Si ambos son INT → INT
            if (t1 == TIPO_DATO_INT && t2 == TIPO_DATO_INT) {
                return TIPO_DATO_INT;
            }
            
            // Si alguno es FLOAT → FLOAT
            if ((t1 == TIPO_DATO_INT || t1 == TIPO_DATO_FLOAT) &&
                (t2 == TIPO_DATO_INT || t2 == TIPO_DATO_FLOAT)) {
                return TIPO_DATO_FLOAT;
            }
            
            error_semantico("Tipos incompatibles en operación", n->operador);
            return TIPO_DATO_NULO;
        }
        
        case NODO_COMPARACION:
        case NODO_LOGICO:
            // Validar que los operandos sean válidos
            obtener_tipo(n->izq);
            if (n->der) obtener_tipo(n->der);
            return TIPO_DATO_BOOL;
        
        case NODO_ENTRADA: 
            return TIPO_DATO_INT;
            
        case NODO_OBTENER_TECLA: 
            return TIPO_DATO_INT;
            
        case NODO_LONGITUD: 
            return TIPO_DATO_INT;
            
        case NODO_ACCESO_ARRAY:
            // Por simplicidad, asumimos que retorna el tipo base
            return TIPO_DATO_INT;

        default: 
            return TIPO_DATO_NULO;
    }
}

// Valida el árbol de sintaxis
void recorrer_y_validar(Nodo* n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_PROGRAMA:
        case NODO_BLOQUE:
            recorrer_y_validar(n->siguiente);
            break;

        case NODO_VAR_DECL: {
            // Verificar doble declaración
            if (buscar_simbolo(n->nombre)) {
                error_semantico("Variable ya declarada", n->nombre);
            }
            
            // Verificar compatibilidad de tipos en la inicialización
            if (n->izq) {
                TipoDato tipo_expr = obtener_tipo(n->izq);
                
                if (tipo_expr != TIPO_DATO_NULO && n->tipo_dato != tipo_expr) {
                    if (n->tipo_dato == TIPO_DATO_FLOAT && tipo_expr == TIPO_DATO_INT) {
                    }
                    else if (n->tipo_dato == TIPO_DATO_INT && tipo_expr == TIPO_DATO_FLOAT) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), 
                               "No se puede asignar decimal a entero en la variable '%s'", 
                               n->nombre);
                        error_semantico(msg, "Los decimales no pueden guardarse en variables enteras");
                    }
                    // Otros tipos incompatibles
                    else {
                        char msg[256];
                        snprintf(msg, sizeof(msg), 
                               "Tipo incompatible en la declaración de '%s'", 
                               n->nombre);
                        error_semantico(msg, "");
                    }
                }
            }
            
            // Registrar la variable
            declarar_variable(n->nombre, n->tipo_dato);
            break;
        }

        case NODO_ASIGNACION: {
            // Verificar que la variable existe
            Simbolo* s = buscar_simbolo(n->nombre);
            if (!s) {
                error_semantico("Variable no declarada", n->nombre);
            } else {
                // Verificar compatibilidad de tipos
                TipoDato tipo_expr = obtener_tipo(n->izq);
                
                if (tipo_expr != TIPO_DATO_NULO && s->tipo != tipo_expr) {
                    // INT → FLOAT OK (promoción)
                    if (s->tipo == TIPO_DATO_FLOAT && tipo_expr == TIPO_DATO_INT) {
                        // Permitido
                    }
                    // FLOAT → INT ERROR
                    else if (s->tipo == TIPO_DATO_INT && tipo_expr == TIPO_DATO_FLOAT) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), 
                               "No se puede asignar decimal a entero en '%s'", 
                               n->nombre);
                        error_semantico(msg, "Use división entera (/) en lugar de DIV");
                    }
                    // Otros incompatibles
                    else {
                        char msg[256];
                        snprintf(msg, sizeof(msg), 
                               "Tipo incompatible en asignación a '%s'", 
                               n->nombre);
                        error_semantico(msg, "");
                    }
                }
            }
            break;
        }

        case NODO_SI:
        case NODO_MIENTRAS:
            obtener_tipo(n->condicion);
            recorrer_y_validar(n->der);
            recorrer_y_validar(n->sino);
            break;

        case NODO_IMPRIMIR:
            obtener_tipo(n->izq);
            break;

        case NODO_PIXEL:
            obtener_tipo(n->izq);
            obtener_tipo(n->der);
            obtener_tipo(n->extra);
            break;
        
        case NODO_FUNCION:
            recorrer_y_validar(n->parametros);
            recorrer_y_validar(n->cuerpo);
            break;
            
        case NODO_RETURN:
            if (n->izq) {
                obtener_tipo(n->izq);
            }
            break;
            
        default:
            break;
    }

    recorrer_y_validar(n->siguiente);
}

// Función principal del análisis semántico
int analizar_semantica(Nodo* raiz) {
    errores_semanticos = 0;
    tabla_simbolos = NULL;
    
    
    if (raiz && raiz->tipo == NODO_PROGRAMA) {
        recorrer_y_validar(raiz->siguiente);
    } else {
        recorrer_y_validar(raiz);
    }

    if (errores_semanticos > 0 && errores_semanticos == 1) {
        fprintf(stderr, "\nSe encontró un error semántico.\n");
        return 1;
    } else if (errores_semanticos > 1) {
        fprintf(stderr, "\nSe encontraron %d errores semánticos.\n", errores_semanticos);
        return 1;
    }
    
    return 0;
}