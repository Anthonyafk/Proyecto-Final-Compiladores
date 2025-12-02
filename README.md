# Compilador de Lenguaje en Español para FIS-25

## Descripción

Compilador completo de un lenguaje de programación basado en el Español que genera código intermedio optimizado para el **Simluador FIS-25**. Incluye análisis léxico (Flex), sintáctico (Bison) y generación de código intermedio.

### Equipo

- Joshua Juárez Cruz
- Antonio Castillo Hernández
- Jesús Elías Vázquez Reyes

## Inicio Rápido

```bash
cd src
make
```

Se genera el código intermedio en `salida.txt` a partir de `reloj.esp`.

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

## Estructura

```
src/
├── lexer.l           # Analizador léxico
├── parser.y          # Gramática y parser
├── ast.h / ast.c     # AST
├── codegen.h / .c    # Generador de código
├── main.c            # Programa principal
├── reloj.esp         # Código fuente
└── salida.txt        # Código intermedio
```

Proceso:
1. Flex genera lexer
2. Bison genera parser
3. GCC compila el compilador
4. Ejecuta sobre `reloj.esp`
5. Genera `salida.txt`

## Ejemplo de sintaxis de nuestro lenguaje

```javascript
// Variables
variable ENTERO horas = 12;
variable ENTERO minutos = 30;

// Entrada
horas = entrada();

// Control de flujo
Si (horas > 12) { 
    horas = 12; 
}

Mientras (by < 64) {
    pintar_pixel(x, y, 1);
    by = by + 1;
}

// Operadores lógicos
Si (x0 == x1 Y y0 == y1) {
    loop = 0;
}

// Funciones nativas
pintar_pixel(x, y, color);
leer_tecla(numero, variable);
imprimir(expresion);
```

## Algoritmo Bresenham

Trazado eficiente de líneas para las manecillas:

```javascript
dx = x1 - x0;
dy = y1 - y0;
err = dx - dy;

Mientras (loop == 1) {
    pintar_pixel(x0, y0, 1);
    Si (x0 == x1 Y y0 == y1) { loop = 0; }
    
    e2 = 2 * err;
    Si (e2 > (0 - dy)) { 
        err = err - dy; 
        x0 = x0 + sx; 
    }
    Si (e2 < dx) { 
        err = err + dx; 
        y0 = y0 + sy; 
    }
}
```

