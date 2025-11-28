%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "codegen.h"

extern int yylex();
void yyerror(const char* s) { fprintf(stderr, "Error: %s\n", s); }
Nodo* raiz; /* Raíz del AST */
%}

%union {
    int ival;
    char* sval;
    struct Nodo* nodo;
}

%token <sval> ID LIT_CADENA
%token <ival> NUM
%token FUNCION TIPO_NULO TIPO_ENTERO TIPO_BOOL VARIABLE SI SINO MIENTRAS IMPRIMIR
%token PINTAR_PIXEL LEER_TECLA
%token IGUAL_QUE ASIGNAR MAS PAR_IZQ PAR_DER LLAVE_IZQ LLAVE_DER PYC

%type <nodo> programa lista_instr instruccion expresion bloque

%%

programa: 
    lista_instr { raiz = $1; }
    ;

lista_instr:
    instruccion { $$ = $1; }
    | instruccion lista_instr { $1->siguiente = $2; $$ = $1; }
    ;

instruccion:
    /* Declaración: variable ENTERO x = 0; */
    VARIABLE TIPO_ENTERO ID ASIGNAR expresion PYC {
        $$ = crear_nodo(NODO_VAR_DECL);
        $$->nombre = $3;
        $$->izq = $5; // Valor inicial
    }
    /* While: Mientras (cond) { ... } */
    | MIENTRAS PAR_IZQ expresion PAR_DER bloque {
        $$ = crear_nodo(NODO_MIENTRAS);
        $$->condicion = $3;
        $$->der = $5; // Cuerpo
    }
    /* Nativas para Opción C */
    | PINTAR_PIXEL PAR_IZQ expresion PAR_DER PYC {
        $$ = crear_nodo(NODO_PIXEL);
        $$->izq = $3;
    }
    ;

bloque:
    LLAVE_IZQ lista_instr LLAVE_DER { $$ = $2; }
    ;

expresion:
    ID { $$ = crear_nodo(NODO_ID); $$->nombre = $1; }
    | NUM { $$ = crear_nodo(NODO_NUMERO); $$->valor_int = $1; }
    | expresion MAS expresion {
        $$ = crear_nodo(NODO_BINARIO);
        $$->nombre = "+";
        $$->izq = $1; $$->der = $3;
    }
    ;

%%