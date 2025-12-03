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

// Agrega variable a la tabla
void declarar_variable(char* nombre, TipoDato tipo) {
    Simbolo* s = (Simbolo*)malloc(sizeof(Simbolo));
    s->nombre = strdup(nombre);
    s->tipo = tipo;
    s->siguiente = tabla_simbolos;
    tabla_simbolos = s;
}

// Busca variable de la tabla
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

// Reporta un error semántico
void error_semantico(const char* msg, const char* detalle) {
    fprintf(stderr, "Error Semántico: %s [%s]\n", msg, detalle ? detalle : "");
    errores_semanticos++;
}

// Determina qué tipo de dato resulta de una expresión
TipoDato obtener_tipo(Nodo* n) {
    if (!n) return TIPO_DATO_NULO;

    switch (n->tipo) {
        case NODO_NUMERO: return TIPO_DATO_INT;
        case NODO_DECIMAL: return TIPO_DATO_FLOAT;
        case NODO_CADENA: return TIPO_DATO_STRING;
        case NODO_BOOLEANO: return TIPO_DATO_BOOL;
        
        case NODO_ID: {
            Simbolo* s = buscar_simbolo(n->nombre);
            if (s) return s->tipo;
            error_semantico("Variable no declarada", n->nombre);
            return TIPO_DATO_NULO;
        }

        case NODO_BINARIO: {
            TipoDato t1 = obtener_tipo(n->izq);
            TipoDato t2 = obtener_tipo(n->der);
            // Regla estricta: Ambos deben ser iguales
            if (t1 == t2) return t1;
            // Regla suave: Int y Float pueden operar (resultado Float)
            if ((t1 == TIPO_DATO_INT && t2 == TIPO_DATO_FLOAT) || 
                (t1 == TIPO_DATO_FLOAT && t2 == TIPO_DATO_INT)) {
                return TIPO_DATO_FLOAT;
            }
            error_semantico("Tipos incompatibles en operación", n->operador);
            return TIPO_DATO_NULO;
        }
        
        case NODO_ENTRADA: return TIPO_DATO_INT; // Asumimos que entrada devuelve int
        case NODO_OBTENER_TECLA: return TIPO_DATO_INT;

        default: return TIPO_DATO_NULO;
    }
}

// -Se recorre el árbol para verificar validez
void recorrer_y_validar(Nodo* n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_PROGRAMA:
        case NODO_BLOQUE:
            recorrer_y_validar(n->siguiente); // Recorremos lista de instrucciones
            break;

        case NODO_VAR_DECL: {
            // Verificamos doble declaración
            if (buscar_simbolo(n->nombre)) {
                error_semantico("Variable ya declarada", n->nombre);
            }
            // Verificamos tipo de la expresión asignada
            if (n->izq) { // Si tiene inicialización (como int x = 5)
                TipoDato tipo_expr = obtener_tipo(n->izq);
                // Permitimos asignar int a float, pero no al revés ni cosas raras
                if (n->tipo_dato != tipo_expr && tipo_expr != TIPO_DATO_NULO) {
                    if (!(n->tipo_dato == TIPO_DATO_FLOAT && tipo_expr == TIPO_DATO_INT)) {
                         error_semantico("Tipo incorrecto en asignación a", n->nombre);
                    }
                }
            }
            // Registramos la variable
            declarar_variable(n->nombre, n->tipo_dato);
            break;
        }

        case NODO_ASIGNACION: {
            // Verificamos la existencia
            Simbolo* s = buscar_simbolo(n->nombre);
            if (!s) {
                error_semantico("Variable no declarada", n->nombre);
            } else {
                // Verificamos compatibilidad
                TipoDato tipo_expr = obtener_tipo(n->izq);
                if (s->tipo != tipo_expr && tipo_expr != TIPO_DATO_NULO) {
                     error_semantico("Tipo incorrecto en asignación", n->nombre);
                }
            }
            break;
        }

        case NODO_SI:
        case NODO_MIENTRAS:
            // Validamos condición (debería ser BOOL o INT)
            obtener_tipo(n->condicion);
            recorrer_y_validar(n->der);  // Cuerpo
            recorrer_y_validar(n->sino); // Else
            break;

        // Validamos expresiones dentro de funciones gráficas
        case NODO_PIXEL:
            obtener_tipo(n->izq); // X
            obtener_tipo(n->der); // Y
            obtener_tipo(n->extra); // Color
            break;
            
        default:
            break;
    }

    // Continua con la siguiente instrucción
    recorrer_y_validar(n->siguiente);
}

// Función Principal
int analizar_semantica(Nodo* raiz) {
    errores_semanticos = 0;
    // Limpiamos tabla por si acaso
    tabla_simbolos = NULL; 
    
    if (raiz && raiz->tipo == NODO_PROGRAMA) {
        recorrer_y_validar(raiz->siguiente); // Saltamos el nodo raíz contenedor
    } else {
        recorrer_y_validar(raiz);
    }

    if (errores_semanticos > 0 && errores_semanticos == 1) {
        fprintf(stderr, "Se encontró un error semántico.\n");
        return 1;
    } else if (errores_semanticos > 1) {
        fprintf(stderr, "Se encontraron %d errores semánticos.\n", errores_semanticos);
        return 1;
    }
    
    return 0;
}