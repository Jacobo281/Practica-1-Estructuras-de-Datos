# Resultados de Terminal 

## Compilación

```bash
clang++ -std=c++17 avl.cpp -o Avl 
clang++ -std=c++17 heapsort.cpp -o Heapsort 
clang++ -std=c++17 quicksort.cpp -o Quicksort 
```

## Ejecución y Salida

### 1. AVL Tree

```bash
$ ./Avl.exe
Se cargaron 100000 palabras limpias.
Iniciando Arbol AVL...

[AVL Tree]
  Tiempo    : 313.77 ms
  Ordenado  : SI
  Peso input: 2.18 MB
  Peso out  : 1.00 MB

--- Complexity Analysis ---
Algorithm   Best            Average         Worst           Space
----------------------------------------------------------------------------
AVL Tree    O(n log n)      O(n log n)      O(n log n)      O(n)
```

### 2. HeapSort

```bash
$ ./Heapsort.exe
Se cargaron 100000 palabras limpias.
Iniciando HeapSort...

[HeapSort]
  Tiempo    : 314.96 ms
  Ordenado  : SI
  Peso input: 2.18 MB
  Peso out  : 1.00 MB

--- Complexity Analysis ---
Algorithm   Best            Average         Worst           Space
----------------------------------------------------------------------------
HeapSort    O(n log n)      O(n log n)      O(n log n)      O(1)
```

### 3. QuickSort

```bash
$ ./Quicksort.exe
Se cargaron 100000 palabras limpias.
Iniciando QuickSort...

[QuickSort]
  Tiempo    : 208.81 ms
  Ordenado  : SI
  Peso input: 2.18 MB
  Peso out  : 1.00 MB

--- Complexity Analysis ---
Algorithm   Best            Average         Worst           Space
----------------------------------------------------------------------------
QuickSort   O(n log n)      O(n log n)      O(n^2)          O(log n)
```
