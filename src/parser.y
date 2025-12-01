%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
void yyerror(const char* s) { fprintf(stderr, "Error de sintaxis: %s\n", s); }
Nodo* raiz;

/* Helpers para crear nodos específicos */
Nodo* crear_nodo_return(Nodo* expr) {
    Nodo* n = crear_nodo(NODO_RETURN);
    n->izq = expr;
    return n;
}
%}

%union {
    int ival;
    float fval;
    char* sval;
    struct Nodo* nodo;
}

/* --- TOKENS (Sincronizados con lexer.l) --- */
%token <sval> ID LIT_STRING
%token <ival> NUM_INT VERDADERO FALSO
%token <fval> NUM_FLOAT

%token FUNCION TIPO_NULO TIPO_INT TIPO_FLOAT TIPO_BOOL TIPO_STRING VARIABLE
%token SI OSI SINO MIENTRAS PARA PARACADA EN IMPRIMIR RETORNAR
%token PINTAR_PIXEL LEER_TECLA OBTENER_TECLA ENTRADA
%token LONGITUD CARACTER_EN

%token IGUAL_QUE DIFERENTE MENOR_QUE MAYOR_QUE MENOR_IGUAL MAYOR_IGUAL
%token ASIGNAR MAS MENOS MULTIPLICAR DIVIDIR MODULO POTENCIA
%token PAR_IZQ PAR_DER LLAVE_IZQ LLAVE_DER CORCHETE_IZQ CORCHETE_DER
%token PYC COMA Y O NO

/* --- NO-TERMINALES --- */
%type <nodo> programa lista_instr instruccion expresion bloque factor 
%type <nodo> lista_args declaracion_func args_func arg_func

/* --- PRECEDENCIA (De menor a mayor) --- */
%left O
%left Y
%left IGUAL_QUE DIFERENTE MENOR_QUE MAYOR_QUE MENOR_IGUAL MAYOR_IGUAL
%left MAS MENOS
%left MULTIPLICAR DIVIDIR MODULO
%right POTENCIA
%right NO
%right UMENOS /* Para el menos unario (-5) */

%%

programa: 
    lista_instr { 
        // Envolvemos la lista en un nodo raíz para que el main lo maneje fácil
        raiz = crear_nodo(NODO_PROGRAMA);
        raiz->siguiente = $1; 
    }
    ;

lista_instr:
    instruccion { $$ = $1; }
    | lista_instr instruccion { 
        Nodo* t = $1;
        while(t->siguiente != NULL) t = t->siguiente;
        t->siguiente = $2;
        $$ = $1;
    }
    ;

instruccion:
    /* 1. Declaración de Variables */
    VARIABLE TIPO_INT ID ASIGNAR expresion PYC {
        $$ = crear_nodo(NODO_VAR_DECL); $$->nombre = $3; $$->izq = $5; 
    }
    | VARIABLE TIPO_BOOL ID ASIGNAR expresion PYC {
        $$ = crear_nodo(NODO_VAR_DECL); $$->nombre = $3; $$->izq = $5; 
    }
    | VARIABLE TIPO_STRING ID ASIGNAR expresion PYC { /* Agregado para strings */
        $$ = crear_nodo(NODO_VAR_DECL); $$->nombre = $3; $$->izq = $5;
    }

    /* 2. Asignación */
    | ID ASIGNAR expresion PYC {
        $$ = crear_nodo(NODO_ASIGNACION); $$->nombre = $1; $$->izq = $3;
    }
    
    /* 3. Control de Flujo */
    | MIENTRAS PAR_IZQ expresion PAR_DER bloque {
        $$ = crear_nodo(NODO_MIENTRAS); $$->condicion = $3; $$->der = $5;
    }
    | SI PAR_IZQ expresion PAR_DER bloque {
        $$ = crear_nodo(NODO_SI); $$->condicion = $3; $$->der = $5;
    }
    | SI PAR_IZQ expresion PAR_DER bloque SINO bloque {
        $$ = crear_nodo(NODO_SI); $$->condicion = $3; $$->der = $5; $$->sino = $7;
    }
    
    /* 4. Funciones Nativas */
    | IMPRIMIR PAR_IZQ expresion PAR_DER PYC {
        $$ = crear_nodo(NODO_IMPRIMIR); $$->izq = $3;
    }
    | PINTAR_PIXEL PAR_IZQ expresion COMA expresion COMA expresion PAR_DER PYC {
        $$ = crear_nodo(NODO_PIXEL); $$->izq = $3; $$->der = $5; $$->extra = $7;
    }
    | LEER_TECLA PAR_IZQ expresion COMA ID PAR_DER PYC {
        $$ = crear_nodo(NODO_TECLA); $$->izq = $3; $$->nombre = $5;
    }

    /* 5. Llamada a funciones VOID */
    | ID PAR_IZQ lista_args PAR_DER PYC {
        $$ = crear_nodo(NODO_LLAMADA_FUNC); $$->nombre = $1; $$->argumentos = $3;
    }
    | ID PAR_IZQ PAR_DER PYC {
        $$ = crear_nodo(NODO_LLAMADA_FUNC); $$->nombre = $1;
    }

    /* 6. Definición de Funciones */
    | declaracion_func { $$ = $1; }

    /* 7. RETORNO (¡Faltaba esto!) */
    | RETORNAR expresion PYC {
        $$ = crear_nodo_return($2);
    }
    | RETORNAR PYC {
        $$ = crear_nodo_return(NULL);
    }

    | PARA PAR_IZQ instruccion expresion PYC ID ASIGNAR expresion PAR_DER bloque {
        $$ = $3; 
        Nodo* mientras = crear_nodo(NODO_MIENTRAS);
        mientras->condicion = $4; // Condición del medio

        Nodo* incremento = crear_nodo(NODO_ASIGNACION);
        incremento->nombre = $6; // ID del incremento
        incremento->izq = $8;    // Expresión del incremento
        
        if ($10) {
            mientras->der = $10; // Cuerpo del usuario
            Nodo* t = $10;
            while (t->siguiente) t = t->siguiente; // Ir al final de la lista del cuerpo
            t->siguiente = incremento;
        } else {
            mientras->der = incremento;
        }
        Nodo* t = $3;
        while (t->siguiente) t = t->siguiente;
        t->siguiente = mientras;
    }
    ;

declaracion_func:
    /* Función con tipo y argumentos */
    FUNCION TIPO_INT ID PAR_IZQ args_func PAR_DER bloque {
        $$ = crear_nodo(NODO_FUNCION); $$->nombre = $3; $$->parametros = $5; $$->cuerpo = $7;
    }
    /* Función con tipo SIN argumentos (¡Faltaba esto!) */
    | FUNCION TIPO_INT ID PAR_IZQ PAR_DER bloque {
        $$ = crear_nodo(NODO_FUNCION); $$->nombre = $3; $$->cuerpo = $6;
    }
    | FUNCION TIPO_BOOL ID PAR_IZQ args_func PAR_DER bloque {
        $$ = crear_nodo(NODO_FUNCION); $$->nombre = $3; $$->parametros = $5; $$->cuerpo = $7;
    }
    /* Función void */
    | FUNCION TIPO_NULO ID PAR_IZQ args_func PAR_DER bloque {
        $$ = crear_nodo(NODO_FUNCION); $$->nombre = $3; $$->parametros = $5; $$->cuerpo = $7;
    }
    | FUNCION TIPO_NULO ID PAR_IZQ PAR_DER bloque {
        $$ = crear_nodo(NODO_FUNCION); $$->nombre = $3; $$->cuerpo = $6;
    }
    ;

args_func:
    arg_func { $$ = $1; }
    | args_func COMA arg_func { 
        Nodo* t = $1; while(t->siguiente) t = t->siguiente; t->siguiente = $3; $$ = $1; 
    }
    ;

arg_func:
    TIPO_INT ID { $$ = crear_nodo(NODO_VAR_DECL); $$->nombre = $2; }
    | TIPO_BOOL ID { $$ = crear_nodo(NODO_VAR_DECL); $$->nombre = $2; }
    ;

bloque:
    LLAVE_IZQ lista_instr LLAVE_DER { $$ = $2; }
    | LLAVE_IZQ LLAVE_DER { $$ = NULL; }
    ;

/* --- EXPRESIONES --- */
expresion:
    factor
    | expresion MAS expresion { $$ = crear_nodo(NODO_BINARIO); $$->operador = strdup("+"); $$->izq = $1; $$->der = $3; }
    | expresion MENOS expresion { $$ = crear_nodo(NODO_BINARIO); $$->operador = strdup("-"); $$->izq = $1; $$->der = $3; }
    | expresion MULTIPLICAR expresion { $$ = crear_nodo(NODO_BINARIO); $$->operador = strdup("*"); $$->izq = $1; $$->der = $3; }
    | expresion DIVIDIR expresion { $$ = crear_nodo(NODO_BINARIO); $$->operador = strdup("/"); $$->izq = $1; $$->der = $3; }
    | expresion MODULO expresion { $$ = crear_nodo(NODO_BINARIO); $$->operador = strdup("%"); $$->izq = $1; $$->der = $3; }
    
    | expresion IGUAL_QUE expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup("=="); $$->izq = $1; $$->der = $3; }
    | expresion DIFERENTE expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup("!="); $$->izq = $1; $$->der = $3; }
    | expresion MENOR_QUE expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup("<"); $$->izq = $1; $$->der = $3; }
    | expresion MAYOR_QUE expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup(">"); $$->izq = $1; $$->der = $3; }
    | expresion MENOR_IGUAL expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup("<="); $$->izq = $1; $$->der = $3; }
    | expresion MAYOR_IGUAL expresion { $$ = crear_nodo(NODO_COMPARACION); $$->operador = strdup(">="); $$->izq = $1; $$->der = $3; }
    
    | expresion Y expresion { $$ = crear_nodo(NODO_LOGICO); $$->operador = strdup("Y"); $$->izq = $1; $$->der = $3; }
    | expresion O expresion { $$ = crear_nodo(NODO_LOGICO); $$->operador = strdup("O"); $$->izq = $1; $$->der = $3; }
    ;

factor:
    ID { $$ = crear_nodo(NODO_ID); $$->nombre = $1; }
    | NUM_INT { $$ = crear_nodo(NODO_NUMERO); $$->valor_int = $1; }
    | NUM_FLOAT { $$ = crear_nodo(NODO_DECIMAL); $$->valor_dec = $1; }
    | LIT_STRING { $$ = crear_nodo(NODO_CADENA); $$->texto = $1; }
    | VERDADERO { $$ = crear_nodo(NODO_BOOLEANO); $$->valor_bool = 1; }
    | FALSO { $$ = crear_nodo(NODO_BOOLEANO); $$->valor_bool = 0; }
    | PAR_IZQ expresion PAR_DER { $$ = $2; }
    
    /* MENOS UNARIO (-5) ¡Faltaba esto! */
    | MENOS factor %prec UMENOS {
        $$ = crear_nodo(NODO_BINARIO);
        $$->operador = strdup("-");
        $$->izq = crear_nodo_numero(0); // 0 - factor
        $$->der = $2;
    }

    /* Funciones nativas en expresión */
    | ENTRADA PAR_IZQ PAR_DER { $$ = crear_nodo(NODO_ENTRADA); }
    | OBTENER_TECLA PAR_IZQ expresion PAR_DER { $$ = crear_nodo(NODO_OBTENER_TECLA); $$->izq = $3; }
    
    /* Funciones de Array */
    | LONGITUD PAR_IZQ expresion PAR_DER { 
        $$ = crear_nodo(NODO_LONGITUD); $$->izq = $3; 
    }
    | CARACTER_EN PAR_IZQ expresion COMA expresion PAR_DER {
        /* Simplificado: Acceso a caracter en string/array */
        $$ = crear_nodo(NODO_ACCESO_ARRAY); $$->nombre = "str"; $$->izq = $3; $$->der = $5;
    }

    /* Llamada a función con retorno */
    | ID PAR_IZQ lista_args PAR_DER {
        $$ = crear_nodo(NODO_LLAMADA_FUNC); $$->nombre = $1; $$->argumentos = $3;
    }
    | ID PAR_IZQ PAR_DER {
        $$ = crear_nodo(NODO_LLAMADA_FUNC); $$->nombre = $1;
    }
    ;

lista_args:
    expresion { $$ = $1; }
    | lista_args COMA expresion { 
        Nodo* t = $1; while(t->siguiente) t = t->siguiente; t->siguiente = $3; $$ = $1; 
    }
    ;

%%