#ifndef AST_H
#define AST_H

typedef enum {
    NODO_PROGRAMA,
    NODO_FUNCION,
    NODO_VAR_DECL,
    NODO_ASIGNACION,
    NODO_MIENTRAS,
    NODO_PARA,
    NODO_PARACADA,
    NODO_SI,
    NODO_OSI,
    NODO_SINO,
    NODO_IMPRIMIR,
    NODO_PIXEL,
    NODO_TECLA,
    NODO_OBTENER_TECLA,
    NODO_ENTRADA,
    NODO_BINARIO,
    NODO_COMPARACION,
    NODO_LOGICO,
    NODO_NUMERO,
    NODO_DECIMAL,
    NODO_ID,
    NODO_CADENA,
    NODO_BOOLEANO,
    NODO_ARREGLO,
    NODO_LLAMADA_FUNC,
    NODO_RETURN,
    NODO_BLOQUE,
    NODO_ACCESO_ARRAY,
    NODO_ASIGNACION_ARRAY,
    NODO_LONGITUD,
    NODO_CARACTER_EN,
    NODO_AGREGAR,
    NODO_ELIMINAR,
    NODO_TAMANO_ARRAY
} TipoNodo;

typedef enum {
    TIPO_DATO_NULO,
    TIPO_DATO_INT,
    TIPO_DATO_FLOAT,
    TIPO_DATO_BOOL,
    TIPO_DATO_STRING,
    TIPO_DATO_POINTER
} TipoDato;

typedef struct Nodo {
    TipoNodo tipo;
    struct Nodo* izq;
    struct Nodo* der;
    struct Nodo* extra;
    struct Nodo* condicion;
    struct Nodo* entonces;
    struct Nodo* sino;
    struct Nodo* siguiente;
    struct Nodo* cuerpo;
    struct Nodo* inicializacion;
    struct Nodo* incremento;
    struct Nodo* iterador;
    struct Nodo* coleccion;
    struct Nodo* parametros;
    struct Nodo* argumentos;
    struct Nodo* indice;
    char* nombre;
    char* operador;
    int valor_int;
    float valor_dec;
    char* texto;
    int valor_bool;
    TipoDato tipo_dato;
    struct Nodo** elementos;
    int num_elementos;
    int es_arreglo;           // Nuevo: flag para saber si es arreglo
    TipoDato tipo_elemento;   // Nuevo: tipo de los elementos del arreglo
} Nodo;

Nodo* crear_nodo(TipoNodo tipo);
Nodo* crear_nodo_numero(int valor);
Nodo* crear_nodo_decimal(float valor);
Nodo* crear_nodo_id(char* nombre);
Nodo* crear_nodo_cadena(char* texto);
Nodo* crear_nodo_booleano(int valor);
Nodo* crear_nodo_binario(char* op, Nodo* izq, Nodo* der);
Nodo* crear_nodo_acceso_array(char* nombre, Nodo* indice);
Nodo* crear_nodo_asignacion_array(char* nombre, Nodo* indice, Nodo* valor);
Nodo* crear_nodo_longitud(Nodo* expresion);
Nodo* crear_nodo_caracter_en(Nodo* string, Nodo* indice);

#endif
