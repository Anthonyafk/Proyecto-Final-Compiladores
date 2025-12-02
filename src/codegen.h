#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

// Genera código intermedio para un nodo del AST
void generar_codigo(Nodo* n);

// Genera código para una expresión y retorna el temporal donde quedó el resultado
char* generar_expresion(Nodo* n);

// Funciones auxiliares
char* nuevo_temp();
int nueva_etiqueta();

#endif
