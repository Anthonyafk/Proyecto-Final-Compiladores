#include <stdlib.h>
#include <string.h>
#include "ast.h"

Nodo* crear_nodo(TipoNodo tipo) {
    Nodo* n = (Nodo*)malloc(sizeof(Nodo));
    n->tipo = tipo;
    n->izq = NULL;
    n->der = NULL;
    n->extra = NULL;
    n->condicion = NULL;
    n->entonces = NULL;
    n->sino = NULL;
    n->siguiente = NULL;
    n->cuerpo = NULL;
    n->inicializacion = NULL;
    n->incremento = NULL;
    n->iterador = NULL;
    n->coleccion = NULL;
    n->parametros = NULL;
    n->argumentos = NULL;
    n->indice = NULL;
    n->nombre = NULL;
    n->operador = NULL;
    n->texto = NULL;
    n->valor_int = 0;
    n->valor_dec = 0.0;
    n->valor_bool = 0;
    n->elementos = NULL;
    n->num_elementos = 0;
    return n;
}

Nodo* crear_nodo_numero(int valor) {
    Nodo* n = crear_nodo(NODO_NUMERO);
    n->valor_int = valor;
    return n;
}

Nodo* crear_nodo_decimal(float valor) {
    Nodo* n = crear_nodo(NODO_DECIMAL);
    n->valor_dec = valor;
    return n;
}

Nodo* crear_nodo_id(char* nombre) {
    Nodo* n = crear_nodo(NODO_ID);
    n->nombre = strdup(nombre);
    return n;
}

Nodo* crear_nodo_cadena(char* texto) {
    Nodo* n = crear_nodo(NODO_CADENA);
    n->texto = strdup(texto);
    return n;
}

Nodo* crear_nodo_booleano(int valor) {
    Nodo* n = crear_nodo(NODO_BOOLEANO);
    n->valor_bool = valor;
    return n;
}

Nodo* crear_nodo_binario(char* op, Nodo* izq, Nodo* der) {
    Nodo* n = crear_nodo(NODO_BINARIO);
    n->operador = strdup(op);
    n->izq = izq;
    n->der = der;
    return n;
}

Nodo* crear_nodo_acceso_array(char* nombre, Nodo* indice) {
    Nodo* n = crear_nodo(NODO_ACCESO_ARRAY);
    n->nombre = strdup(nombre);
    n->indice = indice;
    return n;
}

Nodo* crear_nodo_asignacion_array(char* nombre, Nodo* indice, Nodo* valor) {
    Nodo* n = crear_nodo(NODO_ASIGNACION_ARRAY);
    n->nombre = strdup(nombre);
    n->indice = indice;
    n->izq = valor;
    return n;
}

Nodo* crear_nodo_longitud(Nodo* expresion) {
    Nodo* n = crear_nodo(NODO_LONGITUD);
    n->izq = expresion;
    return n;
}

Nodo* crear_nodo_caracter_en(Nodo* string, Nodo* indice) {
    Nodo* n = crear_nodo(NODO_CARACTER_EN);
    n->izq = string;
    n->der = indice;
    return n;
}