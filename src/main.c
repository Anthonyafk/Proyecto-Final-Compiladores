#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantics.h"

extern int yyparse();
extern FILE* yyin;
extern Nodo* raiz;

void generar_codigo_programa(Nodo* n);

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: No se puede abrir %s\n", argv[1]);
            return 1;
        }
    }
    
    // Análisis sintáctico
    if (yyparse() != 0) {
        fprintf(stderr, "Error de sintaxis.\n");
        return 1;
    }
    
    // Análisis semántico
    if (raiz) {
        // Si hay errores semánticos, detenemos todo
        if (analizar_semantica(raiz) != 0) {
            return 1; 
        }

        // Generación de código intermedio
        generar_codigo_programa(raiz);
    }
    
    
    if (argc > 1) {
        fclose(yyin);
    }

    return 0;
}