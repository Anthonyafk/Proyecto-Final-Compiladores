%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

char* mi_strdup(const char* s) {
    if (!s) return NULL;
    char* nuevo = (char*)malloc(strlen(s) + 1);
    if (nuevo) strcpy(nuevo, s);
    return nuevo;
}
%}

%option noyywrap

%%

[ \t\r\n]+      ; /* Ignorar espacios y saltos de línea */
"//".* ; /* Ignorar comentarios */

"funcion"           { return FUNCION; }
"NULO"              { return TIPO_NULO; }
"ENTERO"            { return TIPO_INT; }
"DECIMAL"           { return TIPO_FLOAT; }
"BOOLEANO"          { return TIPO_BOOL; }
"TEXTO"             { return TIPO_STRING; }
"variable"          { return VARIABLE; }
"Si"                { return SI; }
"OSi"               { return OSI; }
"SiNo"              { return SINO; }
"Mientras"          { return MIENTRAS; }
"Para"              { return PARA; }
"ParaCada"          { return PARACADA; }
"en"                { return EN; }
"imprimir"          { return IMPRIMIR; }
"retornar"          { return RETORNAR; }
"verdadero"         { yylval.ival = 1; return VERDADERO; }
"falso"             { yylval.ival = 0; return FALSO; }
"Y"                 { return Y; }
"O"                 { return O; }
"No"                { return NO; }

"pintar_pixel"      { return PINTAR_PIXEL; }
"leer_tecla"        { return LEER_TECLA; }
"obtener_tecla"     { return OBTENER_TECLA; }
"entrada"           { return ENTRADA; }

"longitud"          { return LONGITUD; }
"caracter_en"       { return CARACTER_EN; }

"=="                { return IGUAL_QUE; }
"!="                { return DIFERENTE; }
"<="                { return MENOR_IGUAL; }
">="                { return MAYOR_IGUAL; }
"<"                 { return MENOR_QUE; }
">"                 { return MAYOR_QUE; }
"="                 { return ASIGNAR; }
"+"                 { return MAS; }
"-"                 { return MENOS; }
"*"                 { return MULTIPLICAR; }
"/"                 { return DIVIDIR; }
"%"                 { return MODULO; }
"^"                 { return POTENCIA; }
"("                 { return PAR_IZQ; }
")"                 { return PAR_DER; }
"{"                 { return LLAVE_IZQ; }
"}"                 { return LLAVE_DER; }
"["                 { return CORCHETE_IZQ; }
"]"                 { return CORCHETE_DER; }
";"                 { return PYC; }
","                 { return COMA; }

[0-9]+\.[0-9]+      { yylval.fval = atof(yytext); return NUM_FLOAT; }
[0-9]+              { yylval.ival = atoi(yytext); return NUM_INT; }
\"([^\"\\]|\\.)*\"  { yylval.sval = mi_strdup(yytext); return LIT_STRING; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = mi_strdup(yytext); return ID; }

.                   { printf("Error lexico: Caracter no reconocido %s\n", yytext); }

%%