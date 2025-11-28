#include <stdio.h>
#include "ast.h"

int label_count = 0;

void generar_codigo(Nodo* n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_VAR_DECL:
            // variable ENTERO x = ...
            // Genera: VAR x
            // Genera: codigo de expr -> t0
            // Genera: ASSIGN t0 x
            printf("VAR %s\n", n->nombre);
            // (Aquí iría la lógica recursiva para calcular la expresión y asignar)
            break;

        case NODO_MIENTRAS: {
            // Lógica explicada en el video para WHILE
            int lbl_inicio = label_count++;
            int lbl_fin = label_count++;

            printf("LABEL L_INICIO_%d\n", lbl_inicio);
            
            // Evaluar condición (simplificado, asume resultado en t0)
            generar_codigo(n->condicion); 
            
            // Opción C: "El texto no se desplaza automáticamente" [cite: 44]
            // Aquí iría el chequeo
            printf("IFFALSE t0 GOTO L_FIN_%d\n", lbl_fin);

            // Cuerpo del ciclo
            generar_codigo(n->der);

            printf("GOTO L_INICIO_%d\n", lbl_inicio);
            printf("LABEL L_FIN_%d\n", lbl_fin);
            break;
        }

        case NODO_PIXEL:
            // Soporte para Opción C: Renderizar puntos [cite: 25]
            // Suponiendo que la expresión calcula coordenada
            printf("PIXEL t0 t1 1\n"); // X Y Color
            break;
            
        case NODO_TECLA:
             // Soporte para Opción C: Control manual [cite: 45]
             printf("KEY t0\n"); 
             break;
    }

    // Procesar siguiente instrucción
    generar_codigo(n->siguiente);
}