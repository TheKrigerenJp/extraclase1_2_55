#ifndef PAGEDARRAY_H
#define PAGEDARRAY_H

#include <vector>
#include <fstream>
#include <unordered_map>
//holaaaa
class PagedArray {
public:
    PagedArray(const std::string& filePath);
    ~PagedArray();
    int operator[](size_t index);
    void set(size_t index, int value);
    size_t size() const;

private:
    static const size_t PAGE_SIZE = 1024; // Número de enteros por página
    static const size_t NUM_PAGES = 4; // Máximo número de páginas en memoria
    std::vector<int*> pages; // Vector de punteros a páginas en memoria
    std::vector<size_t> pageIndices; // Índices de las páginas en memoria
    std::unordered_map<size_t, int*> pageTable; // Tabla de páginas
    std::string filePath; // Ruta del archivo
    std::ifstream inputFile; // Archivo de entrada
    std::ofstream outputFile; // Archivo de salida
    size_t fileSize; // Tamaño del archivo en enteros

    void loadPage(size_t pageIndex);
    void unloadPage(size_t pageIndex);
    size_t getPageIndex(size_t index) const;
    size_t getOffset(size_t index) const;
};

#endif
