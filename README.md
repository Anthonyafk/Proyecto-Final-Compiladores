# Compilador de Lenguaje en Español para FIS-25

## Descripción

Compilador de lenguaje de programación en español que genera código intermedio para el **Simulador FIS-25**. Incluye análisis léxico (Flex), sintáctico (Bison), semántico y generación de código optimizado.

### Equipo

- Joshua Juárez Cruz
- Antonio Castillo Hernández
- Jesús Elías Vázquez Reyes

## Inicio Rápido

```bash
cd src
make
```

Genera código intermedio en `salida.txt` a partir de `reloj.esp`.

### Ejecutar en FIS-25

1. Cargar `salida.txt` en el simulador
2. Presionar "Run"
3. Configurar hora y minutos
4. Controlar con Tecla 1 (horas-Flecha izquierda) y Tecla 2 (minutos-Flecha hacia abajo)

## Proyecto: Reloj Analógico Interactivo

Reloj analógico de 12 horas en pantalla 64x64 con control manual mediante teclado.

### Características

- **Pantalla 64x64**: Reloj con marcas horarias y manecillas  
- **Control interactivo**: Ajuste manual con teclas 1 y 2  
- **Algoritmo Bresenham**: Trazado eficiente de líneas  
- **Entrada validada**: Límites automáticos (horas 1-12, minutos 0-59)  
- **Código optimizado**: Generación eficiente de código intermedio

## Sintaxis del Lenguaje

```javascript
// Variables
variable ENTERO horas = 12;
variable DECIMAL precio = 19.99;
variable BOOLEANO activo = verdadero;
variable ENTERO[] nums = [10, 20, 30];

// Entrada/Salida
variable ENTERO x = entrada();
imprimir(x);

// Control de flujo
Si (x > 10) {
    imprimir("Mayor");
} SiNo {
    imprimir("Menor");
}

Mientras (x < 100) {
    x = x + 1;
}

// Arreglos
agregar(nums, 40);
nums[0] = 99;
eliminar(nums, 1);

// Funciones
funcion ENTERO calcularMCD(ENTERO a, ENTERO b) {
    Mientras (b > 0) {
        variable ENTERO temp = b;
        b = a % b;
        a = temp;
    }
    retornar a;
}

// Operaciones
/ → División entera (ENTERO / ENTERO)
DIV → División decimal (produce DECIMAL)
% → Módulo

// Funciones FIS-25
pintar_pixel(x, y, color);
leer_tecla(numero, variable);
```

## Estructura

```
src/
├── lexer.l              # Análisis léxico
├── parser.y             # Análisis sintáctico
├── ast.h / ast.c        # AST
├── semantics.h / .c     # Análisis semántico
├── codegen.h / .c       # Generación de código
├── main.c               # Principal
├── reloj.esp            # Código fuente (reloj analógico)
├── torneo_v2.esp        # MCD con funciones
├── arreglos.esp         # Pruebas de arreglos
├── div.esp              # División entera
└── salida.txt           # Código intermedio
```

## Compilación

```bash
make                # Compila y genera código
make clean          # Limpia archivos generados
```

Proceso:
1. Flex → `lex.yy.c`
2. Bison → `parser.tab.c`
3. GCC → `compilador`
4. Ejecuta sobre `reloj.esp`
5. Genera `salida.txt`
