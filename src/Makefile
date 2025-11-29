# --- Variables de Configuración ---
CC = gcc
CFLAGS = -Wall -g -Wno-implicit-function-declaration -Wno-unused-function -D_GNU_SOURCE 
LEX = flex
YACC = bison

EXEC = compilador
SRC = letrero.esp
OUT = salida.txt

GEN_SRC = parser.tab.c parser.tab.h lex.yy.c
OBJS = main.o ast.o codegen.o parser.tab.o lex.yy.o

# REGLA 
all:
	@echo " "
	@echo "-> Borrando archivos anteriores"
	@rm -f $(GEN_SRC) $(OBJS) $(EXEC) $(EXEC).exe $(OUT)
	
	@echo " "
	@echo "-> Generando sintaxis"
	@$(YACC) -d parser.y
	
	@echo " "
	@echo "-> Generando léxico"
	@$(LEX) lexer.l
	
	@echo " "
	@echo "-> Compilando ejecutable"
	@$(CC) $(CFLAGS) -o $(EXEC) parser.tab.c lex.yy.c ast.c codegen.c main.c
	
	@echo " "
	@echo "Generando código intermedio en $(OUT)"
	@./$(EXEC) $(SRC) > $(OUT)
	
	@echo " "
	@echo "Eliminando ejecutables y temporales"
	@rm -f $(GEN_SRC)
	@rm -f $(OBJS)
	@rm -f $(EXEC)
	@rm -f $(EXEC).exe
	
	@echo " "
	@echo "Carpeta limpia y resultado guardado en: $(OUT)"

# --- Utilidades ---
# Borra TODO, incluyendo el txt generado
clean:
	@rm -f $(GEN_SRC) $(OBJS) $(EXEC) $(EXEC).exe $(OUT)
	@echo "Directorio totalmente limpio."

.PHONY: all clean