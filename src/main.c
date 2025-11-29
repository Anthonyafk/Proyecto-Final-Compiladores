#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yyparse();
extern FILE* yyin;
extern Nodo* raiz;

void generar_codigo_programa(Nodo* n);

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: No se puede abrir el archivo %s\n", argv[1]);
            return 1;
        }
    }
    
    // Parsear el programa
    if (yyparse() != 0) {
        fprintf(stderr, "Error de análisis sintáctico\n");
        return 1;
    }
    
    // Generar código intermedio (Directo al grano, sin comentarios extra)
    if (raiz) {
        generar_codigo_programa(raiz);
    }
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return 0;
}