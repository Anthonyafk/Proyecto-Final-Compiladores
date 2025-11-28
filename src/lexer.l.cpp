%{
#include "parser.tab.h"
#include <string.h>
%}

%option noyywrap

%%

"funcion"       { return FUNCION; }
"NULO"          { return TIPO_NULO; }
"ENTERO"        { return TIPO_ENTERO; }
"BOOLEANO"      { return TIPO_BOOL; }
"variable"      { return VARIABLE; }
"Si"            { return SI; }
"SiNo"          { return SINO; }
"Mientras"      { return MIENTRAS; }
"imprimir"      { return IMPRIMIR; }
"pintar_pixel"  { return PINTAR_PIXEL; } /* Necesario para Opción C */
"leer_tecla"    { return LEER_TECLA; }   /* Necesario para Opción C */

[0-9]+          { yylval.ival = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return ID; }
\"[^"]*\"       { yylval.sval = strdup(yytext); return LIT_CADENA; }

"=="            { return IGUAL_QUE; }
"="             { return ASIGNAR; }
"+"             { return MAS; }
"("             { return PAR_IZQ; }
")"             { return PAR_DER; }
"{"             { return LLAVE_IZQ; }
"}"             { return LLAVE_DER; }
";"             { return PYC; }

[ \t\n]         ; /* Ignorar espacios */
.               { printf("Error lexico: %s\n", yytext); }

%%