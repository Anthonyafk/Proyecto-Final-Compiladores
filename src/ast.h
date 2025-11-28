#ifndef AST_H
#define AST_H

typedef enum {
    NODO_PROGRAMA,
    NODO_FUNCION,
    NODO_VAR_DECL, // variable ENTERO x = ...
    NODO_ASIGNACION,
    NODO_MIENTRAS,
    NODO_SI,
    NODO_IMPRIMIR,
    NODO_PIXEL,    // Nativa para el letrero
    NODO_TECLA,    // Nativa para leer teclado
    NODO_BINARIO,  // Operaciones +, -, *, /
    NODO_NUMERO,
    NODO_ID,
    NODO_CADENA
} TipoNodo;

typedef struct Nodo {
    TipoNodo tipo;
    // Punteros genéricos para construir el árbol
    struct Nodo* izq;
    struct Nodo* der;
    struct Nodo* condicion;
    struct Nodo* siguiente; // Para lista de instrucciones
    
    // Datos
    char* nombre;   // Para IDs y nombres de func
    int valor_int;  // Para literales ENTERO
    float valor_dec; // Para DECIMAL
    char* texto;    // Para literales TEXTO
} Nodo;

// Constructor básico
Nodo* crear_nodo(TipoNodo tipo);
#endif