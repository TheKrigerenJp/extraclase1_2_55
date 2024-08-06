#include "PagedArray.h"

PagedArray::Frames::Frames(int numeroPagina) : numeroPagina(numeroPagina) {
    std::fill(std::begin(numeros), std::end(numeros), 0);
}

int PagedArray::Frames::getNumeroPagina() const {
    return numeroPagina;
}

void PagedArray::Frames::setNumeroPagina(int numeroPagina) {
    this->numeroPagina = numeroPagina;
}

const int* PagedArray::Frames::getNumeros() const {
    return numeros;
}

int* PagedArray::Frames::getNumeros() {
    return numeros;
}

void PagedArray::Frames::setNumeros(const int newNumeros[ARRAY_SIZE]) {
    std::copy(newNumeros, newNumeros + ARRAY_SIZE, numeros);
}

int& PagedArray::Frames::operator[](int index) {
    return numeros[index];
}

PagedArray::PagedArray(const std::string& inputPath, const std::string& outputPath, int totalIntegers)
    : inputFilePath(inputPath), outputFilePath(outputPath), totalIntegers(totalIntegers), pageHits(0), pageFaults(0) {
    std::fill(std::begin(frames), std::end(frames), nullptr);
    inicializarArchivoDeSalida();
}

PagedArray::~PagedArray() {
    guardarTodosLosFrames();  // Asegúrate de guardar cualquier frame restante
    for (int i = 0; i < MAX_FRAMES; ++i) {
        delete frames[i];
    }
}

void PagedArray::inicializarArchivoDeSalida() {
    std::ofstream outputFile(outputFilePath + "/salida", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!outputFile) {
        throw std::runtime_error("Error al abrir el archivo para inicializar");
    }
    int zero = 0;
    for (int i = 0; i < totalIntegers; ++i) {
        outputFile.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
    }

    outputFile.close();
}

int& PagedArray::operator[](int index) {
    int frameIndex = index / ARRAY_SIZE;
    int offset = index % ARRAY_SIZE;

    // Verifica si el frame está cargado
    for (int i = 0; i < MAX_FRAMES; ++i) {
        if (frames[i] && frames[i]->getNumeroPagina() == frameIndex) {
            ++pageHits;
            return frames[i]->getNumeros()[offset];
        }
    }

    ++pageFaults;
    cargarFrameDesdeArchivo(frameIndex);

    for (int i = 0; i < MAX_FRAMES; ++i) {
        if (frames[i] && frames[i]->getNumeroPagina() == frameIndex) {
            return frames[i]->getNumeros()[offset];
        }
    }

    throw std::runtime_error("Frame no cargado correctamente");
}

void PagedArray::guardarFrameEnArchivo(Frames* frame, int frameIndex) {
    std::fstream outputFile(outputFilePath + "/salida", std::ios::binary | std::ios::in | std::ios::out);

    if (!outputFile) {
        throw std::runtime_error("Error al abrir el archivo para guardar el frame");
    }

    std::streampos position = frameIndex * ARRAY_SIZE * sizeof(int);
    outputFile.seekp(position);

    if (!outputFile) {
        throw std::runtime_error("Error al mover el puntero de escritura en el archivo");
    }

    outputFile.write(reinterpret_cast<char*>(frame->getNumeros()), ARRAY_SIZE * sizeof(int));

    if (!outputFile) {
        throw std::runtime_error("Error al escribir en el archivo");
    }

    outputFile.close();
}

void PagedArray::guardarTodosLosFrames() {
    for (int i = 0; i < MAX_FRAMES; ++i) {
        if (frames[i]) {
            guardarFrameEnArchivo(frames[i], frames[i]->getNumeroPagina());
            delete frames[i];
            frames[i] = nullptr;
        }
    }
}

void PagedArray::cargarFrameDesdeArchivo(int frameIndex) {
    int freeFrameIndex = -1;
    for (int i = 0; i < MAX_FRAMES; ++i) {
        if (!frames[i]) {
            freeFrameIndex = i;
            break;
        }
    }

    if (freeFrameIndex == -1) {
        freeFrameIndex = rand() % MAX_FRAMES;
        guardarFrameEnArchivo(frames[freeFrameIndex], frames[freeFrameIndex]->getNumeroPagina());
        delete frames[freeFrameIndex];
    }

    std::ifstream outputFile(outputFilePath + "/salida", std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Error al abrir el archivo de salida para leer el frame");
    }

    int* data = new int[ARRAY_SIZE];
    std::streampos position = frameIndex * ARRAY_SIZE * sizeof(int);
    outputFile.seekg(position);
    outputFile.read(reinterpret_cast<char*>(data), ARRAY_SIZE * sizeof(int));

    bool loadFromOutput = false;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        if (data[i] != 0) {
            loadFromOutput = true;
            break;
        }
    }

    outputFile.close();

    if (!loadFromOutput) {
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("Error al abrir el archivo binario de entrada");
        }

        inputFile.seekg(position);
        if (!inputFile.read(reinterpret_cast<char*>(data), ARRAY_SIZE * sizeof(int))) {
            throw std::runtime_error("Error al leer el archivo binario de entrada");
        }

        inputFile.close();
    }

    frames[freeFrameIndex] = new Frames(frameIndex);
    frames[freeFrameIndex]->setNumeros(data);
    delete[] data;
}

int PagedArray::verificarFrames() const {
    for (int i = 0; i < MAX_FRAMES; ++i) {
        if (!frames[i]) return i + 1;
    }
    return 0;
}

int PagedArray::getPageHits() const {
    return pageHits;
}

int PagedArray::getPageFaults() const {
    return pageFaults;
}


