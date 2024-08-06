#ifndef PAGEDARRAY_H
#define PAGEDARRAY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstring>

const int ARRAY_SIZE = 8000;
const int MAX_FRAMES = 4;  // Actualizado a 4

class PagedArray {
public:
    class Frames {
    public:
        Frames(int numeroPagina);
        int getNumeroPagina() const;
        void setNumeroPagina(int numeroPagina);
        const int* getNumeros() const;
        int* getNumeros();
        void setNumeros(const int newNumeros[ARRAY_SIZE]);
        int& operator[](int index);

    private:
        int numeroPagina;
        int numeros[ARRAY_SIZE];
    };

    PagedArray(const std::string& inputPath, const std::string& outputPath, int totalIntegers);
    ~PagedArray();
    int& operator[](int index);
    void guardarFrameEnArchivo(Frames* frame, int frameIndex);
    void guardarTodosLosFrames();
    void cargarFrameDesdeArchivo(int frameIndex);
    int verificarFrames() const;

    int getPageHits() const;
    int getPageFaults() const;

private:
    void inicializarArchivoDeSalida();

    std::string inputFilePath;
    std::string outputFilePath;
    Frames* frames[MAX_FRAMES];
    int totalIntegers;
    int pageHits;
    int pageFaults;
};

#endif
