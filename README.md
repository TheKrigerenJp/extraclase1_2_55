# Paginación y Ordenamiento Externo

Este proyecto incluye dos programas principales: `generator.cpp` y `sorter.cpp`. El propósito es generar un archivo binario grande con números aleatorios y luego ordenar esos números utilizando un algoritmo de ordenamiento externo con paginación.

## Requisitos

- Sistema operativo: Linux (preferiblemente Ubuntu)
- Compilador: g++ (GCC)

## Compilación

Para compilar ambos programas, utiliza los siguientes comandos:

g++ -o generator generator.cpp
g++ -o sorter sorter.cpp

## Generación del Archivo

El programa generator se utiliza para crear un archivo binario de tamaño especificado con números aleatorios.


./generator --size <SIZE> --output <OUTPUT FILE PATH>

## Ordenamiento del Archivo

El programa sorter se utiliza para ordenar el archivo binario generado utilizando uno de los algoritmos de ordenamiento disponibles.

./sorter -input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITHM>

## Detalles Técnicos
generator.cpp

- El programa generator.cpp genera un archivo binario con números aleatorios. El tamaño del archivo puede ser pequeño (512 MB), mediano (1 GB) o grande (2 GB).
sorter.cpp

- El programa sorter.cpp implementa un algoritmo de ordenamiento externo con paginación para ordenar el archivo binario generado. Utiliza una estructura de PaginatedArray que simula un sistema de memoria virtual con marcos de página. Los algoritmos de ordenamiento disponibles son QuickSort, InsertionSort y BubbleSort.

## Notas
- La ruta de salida para generator debe ser absoluta y comenzar con /.
- Asegúrate de tener suficiente espacio en disco para los archivos generados.
- La ejecución de estos programas puede tomar tiempo dependiendo del tamaño del archivo y del algoritmo de ordenamiento seleccionado.
