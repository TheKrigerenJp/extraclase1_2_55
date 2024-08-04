#include "PagedArray.h"
#include <iostream>
#include <algorithm>
#include <cstring>

PagedArray::PagedArray(const std::string& filePath) : filePath(filePath), fileSize(0) {
    inputFile.open(filePath, std::ios::binary | std::ios::ate);
    if (!inputFile) {
        throw std::runtime_error("Error al abrir el archivo de entrada.");
    }
    fileSize = inputFile.tellg() / sizeof(int);
    inputFile.seekg(0, std::ios::beg);
    pages.resize(NUM_PAGES, nullptr);
    pageIndices.resize(NUM_PAGES, SIZE_MAX);
}

PagedArray::~PagedArray() {
    for (auto page : pages) {
        delete[] page;
    }
    inputFile.close();
}

int PagedArray::operator[](size_t index) {
    size_t pageIndex = getPageIndex(index);
    size_t offset = getOffset(index);

    if (pageTable.find(pageIndex) == pageTable.end()) {
        loadPage(pageIndex);
    }

    return pageTable[pageIndex][offset];
}

void PagedArray::set(size_t index, int value) {
    size_t pageIndex = getPageIndex(index);
    size_t offset = getOffset(index);

    if (pageTable.find(pageIndex) == pageTable.end()) {
        loadPage(pageIndex);
    }

    pageTable[pageIndex][offset] = value;
}

size_t PagedArray::size() const {
    return fileSize;
}

void PagedArray::loadPage(size_t pageIndex) {
    if (pageTable.size() >= NUM_PAGES) {
        unloadPage(pageIndices[0]);
    }

    int* page = new int[PAGE_SIZE];
    inputFile.seekg(pageIndex * PAGE_SIZE * sizeof(int), std::ios::beg);
    inputFile.read(reinterpret_cast<char*>(page), PAGE_SIZE * sizeof(int));

    pageTable[pageIndex] = page;
    pageIndices.push_back(pageIndex);
}

void PagedArray::unloadPage(size_t pageIndex) {
    if (pageTable.find(pageIndex) != pageTable.end()) {
        delete[] pageTable[pageIndex];
        pageTable.erase(pageIndex);
        pageIndices.erase(std::remove(pageIndices.begin(), pageIndices.end(), pageIndex), pageIndices.end());
    }
}
//holaaaa
size_t PagedArray::getPageIndex(size_t index) const {
    return index / PAGE_SIZE;
}

size_t PagedArray::getOffset(size_t index) const {
    return index % PAGE_SIZE;
}
