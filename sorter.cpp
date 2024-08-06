#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include "PagedArray.h"

int totalIntegers = 0;

void calcularTamanoArchivo(const std::string& inputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary | std::ios::ate);
    if (!inputFile) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada");
    }

    totalIntegers = static_cast<int>(inputFile.tellg()) / sizeof(int);
    inputFile.close();
}

int partition(PagedArray& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; ++j) {
        if (arr[j] < pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void insertionSort(PagedArray& arr, int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void bubbleSort(PagedArray& arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Uso: sorter –input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITMO>" << std::endl;
        return 1;
    }

    std::string inputFilePath;
    std::string outputFilePath;
    std::string algorithm;

    for (int i = 1; i < argc; i += 2) {
        if (std::strcmp(argv[i], "-input") == 0) {
            inputFilePath = argv[i + 1];
        } else if (std::strcmp(argv[i], "-output") == 0) {
            outputFilePath = argv[i + 1];
        } else if (std::strcmp(argv[i], "-alg") == 0) {
            algorithm = argv[i + 1];
        } else {
            std::cerr << "Argumento no reconocido: " << argv[i] << std::endl;
            return 1;
        }
    }

    calcularTamanoArchivo(inputFilePath);

    PagedArray pagedArray(inputFilePath, outputFilePath, totalIntegers);

    auto start = std::chrono::high_resolution_clock::now();

    if (algorithm == "QS") {
        quickSort(pagedArray, 0, totalIntegers - 1);
    } else if (algorithm == "IS") {
        insertionSort(pagedArray, totalIntegers);
    } else if (algorithm == "BS") {
        bubbleSort(pagedArray, totalIntegers);
    } else {
        std::cerr << "Algoritmo no reconocido: " << algorithm << std::endl;
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    pagedArray.guardarTodosLosFrames();

    std::cout << "Tiempo de ejecución del algoritmo de ordenamiento: " << duration.count() << " segundos" << std::endl;
    std::cout << "Algoritmo utilizado: " << algorithm << std::endl;
    std::cout << "Page hits: " << pagedArray.getPageHits() << std::endl;
    std::cout << "Page faults: " << pagedArray.getPageFaults() << std::endl;

    return 0;
}

