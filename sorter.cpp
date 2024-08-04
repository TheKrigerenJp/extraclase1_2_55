#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include "PagedArray.h"
//holaaaaa
void quickSort(PagedArray& arr, int low, int high);
void insertionSort(PagedArray& arr, int n);
void bubbleSort(PagedArray& arr, int n);

void swap(PagedArray& arr, int i, int j) {
    int temp = arr[i];
    arr.set(i, arr[j]);
    arr.set(j, temp);
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Uso: " << argv[0] << " --input <INPUT FILE PATH> --output <OUTPUT FILE PATH> --alg <ALGORITMO>" << std::endl;
        return 1;
    }

    std::string inputPath, outputPath, algorithm;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0) {
            outputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--alg") == 0) {
            algorithm = argv[++i];
        }
    }

    // Copiar el archivo de entrada al archivo de salida
    std::ifstream src(inputPath, std::ios::binary);
    std::ofstream dst(outputPath, std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();

    try {
        PagedArray arr(outputPath);
        auto start = std::chrono::high_resolution_clock::now();

        if (algorithm == "QS") {
            quickSort(arr, 0, arr.size() - 1);
        } else if (algorithm == "IS") {
            insertionSort(arr, arr.size());
        } else if (algorithm == "BS") {
            bubbleSort(arr, arr.size());
        } else {
            throw std::invalid_argument("Algoritmo no válido. Debe ser QS, IS o BS.");
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "Tiempo durado: " << duration.count() << " segundos" << std::endl;
        std::cout << "Algoritmo utilizado: " << algorithm << std::endl;
        // Aquí puedes imprimir la cantidad de page faults y page hits si implementas esa funcionalidad en PagedArray
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; ++j) {
            if (arr[j] < pivot) {
                ++i;
                swap(arr, i, j);
            }
        }
        swap(arr, i + 1, high);
        int pi = i + 1;

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void insertionSort(PagedArray& arr, int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr.set(j + 1, arr[j]);
            --j;
        }
        arr.set(j + 1, key);
    }
}

void bubbleSort(PagedArray& arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr, j, j + 1);
            }
        }
    }
}
