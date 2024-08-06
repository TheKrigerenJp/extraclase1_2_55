#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <stdexcept>
//;)
// Variables globales
const int PAGE_SIZE = 8000;
const int MAX_PAGE_FRAMES = 4;
int totalInts = 0;
int inputFileSize = 0;
long pageHits = 0;
long pageFaults = 0;

// Clase que representa un arreglo paginado
class PaginatedArray {
public:
    // Subclase para administrar cada página de datos
    class PageFrame {
    private:
        int pageIndex;
        int numbers[PAGE_SIZE];

    public:
        // Constructor
        PageFrame(int pageIndex) : pageIndex(pageIndex) {
            std::fill(std::begin(numbers), std::end(numbers), 0);
        }

        // Métodos getter y setter
        int getPageIndex() const {
            return pageIndex;
        }

        void setPageIndex(int pageIndex) {
            this->pageIndex = pageIndex;
        }

        const int* getNumbers() const {
            return numbers;
        }

        int* getNumbers() {
            return numbers;
        }

        void setNumbers(const int newNumbers[PAGE_SIZE]) {
            std::copy(newNumbers, newNumbers + PAGE_SIZE, numbers);
        }

        // Sobrecarga del operador []
        int& operator[](int index) {
            return numbers[index];
        }
    };

public:
    // Variables del arreglo paginado
    PageFrame* frames[MAX_PAGE_FRAMES];
    std::string inputFile;
    std::string outputFile;

    // Método para guardar un frame en el archivo de salida
    void saveFrameToFile(PageFrame* frame, int frameIndex) {
        std::fstream output(outputFile + "/output", std::ios::binary | std::ios::in | std::ios::out);

        if (!output) {
            throw std::runtime_error("Error al abrir el archivo para guardar el frame");
        }

        std::streampos position = frameIndex * PAGE_SIZE * sizeof(int);
        output.seekp(position);

        if (!output) {
            throw std::runtime_error("Error al mover el puntero de escritura en el archivo");
        }

        output.write(reinterpret_cast<char*>(frame->getNumbers()), PAGE_SIZE * sizeof(int));

        if (!output) {
            throw std::runtime_error("Error al escribir en el archivo");
        }

        output.close();
    }

    // Método para guardar todos los frames al finalizar el proceso
    void saveAllFrames() {
        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            if (frames[i]) {
                saveFrameToFile(frames[i], frames[i]->getPageIndex());
                delete frames[i];
                frames[i] = nullptr;
            }
        }
    }

public:
    // Constructor
    PaginatedArray(const std::string& inputPath, const std::string& outputPath)
        : inputFile(inputPath), outputFile(outputPath) {
        std::fill(std::begin(frames), std::end(frames), nullptr);
        initializeOutputFile();
    }

    // Destructor
    ~PaginatedArray() {
        saveAllFrames();
        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            delete frames[i];
        }
    }

    // Método para inicializar el archivo de salida
    void initializeOutputFile() {
        std::ofstream output(outputFile + "/output", std::ios::binary | std::ios::out | std::ios::trunc);
        if (!output) {
            std::cerr << "Error al abrir el archivo para inicializar: " << outputFile + "/output" << std::endl;
            throw std::runtime_error("Error al abrir el archivo para inicializar");
        }
        int zero = 0;
        for (int i = 0; i < totalInts; ++i) {
            output.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
        }

        output.close();
    }

    // Sobrecarga del operador []
    int& operator[](int index) {
        int frameIndex = index / PAGE_SIZE;
        int offset = index % PAGE_SIZE;

        // Verifica si el frame está cargado
        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            if (frames[i] && frames[i]->getPageIndex() == frameIndex) {
                pageHits++;
                return frames[i]->getNumbers()[offset];
            }
        }
        loadFrameFromFile(frameIndex);

        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            if (frames[i] && frames[i]->getPageIndex() == frameIndex) {
                return frames[i]->getNumbers()[offset];
            }
        }

        throw std::runtime_error("Frame no cargado correctamente");
    }

    // Método para cargar un frame desde el archivo
    void loadFrameFromFile(int frameIndex) {
        int freeFrameIndex = -1;
        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            if (!frames[i]) {
                freeFrameIndex = i;
                break;
            }
        }

        if (freeFrameIndex == -1) {
            freeFrameIndex = rand() % MAX_PAGE_FRAMES;
            saveFrameToFile(frames[freeFrameIndex], frames[freeFrameIndex]->getPageIndex());
            delete frames[freeFrameIndex];
        }

        std::ifstream output(outputFile + "/output", std::ios::binary);
        if (!output) {
            throw std::runtime_error("Error al abrir el archivo de salida para leer el frame");
        }

        int* data = new int[PAGE_SIZE];
        std::streampos position = frameIndex * PAGE_SIZE * sizeof(int);
        output.seekg(position);
        output.read(reinterpret_cast<char*>(data), PAGE_SIZE * sizeof(int));

        bool loadFromOutput = false;
        for (int i = 0; i < PAGE_SIZE; ++i) {
            if (data[i] != 0) {
                loadFromOutput = true;
                break;
            }
        }

        output.close();

        if (!loadFromOutput) {
            std::ifstream input(inputFile, std::ios::binary);
            if (!input) {
                throw std::runtime_error("Error al abrir el archivo binario de entrada");
            }

            input.seekg(position);
            if (!input.read(reinterpret_cast<char*>(data), PAGE_SIZE * sizeof(int))) {
                throw std::runtime_error("Error al leer el archivo binario de entrada");
            }

            input.close();
        }

        frames[freeFrameIndex] = new PageFrame(frameIndex);
        frames[freeFrameIndex]->setNumbers(data);
        delete[] data;
        pageFaults++;
    }

    // Método para verificar si hay frames disponibles
    int checkAvailableFrames() const {
        for (int i = 0; i < MAX_PAGE_FRAMES; ++i) {
            if (!frames[i]) return i + 1;
        }
        return 0;
    }
};

// Funciones de ordenamiento
int partition(PaginatedArray& arr, int start, int end) {
    int pivot = arr[start];
    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot) {
            count++;
        }
    }

    int pivotIndex = start + count;
    std::swap(arr[pivotIndex], arr[start]);

    int i = start, j = end;

    while (i < pivotIndex && j > pivotIndex) {
        while (arr[i] <= pivot) {
            i++;
        }

        while (arr[j] > pivot) {
            j--;
        }

        if (i < pivotIndex && j > pivotIndex) {
            std::swap(arr[i++], arr[j--]);
        }
    }

    return pivotIndex;
}

void quickSort(PaginatedArray& arr, int start, int end) {
    if (start >= end)
        return;

    int p = partition(arr, start, end);
    quickSort(arr, start, p - 1);
    quickSort(arr, p + 1, end);
}

void insertionSort(PaginatedArray& arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void bubbleSort(PaginatedArray& arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Función para convertir un archivo binario a texto
void convertBinaryToText(const std::string& binaryFilePath, const std::string& textFilePath) {
    std::ifstream inputFile(binaryFilePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo binario de entrada: " << binaryFilePath << "\n";
        return;
    }

    std::ofstream outputFile(textFilePath, std::ios::trunc);
    if (!outputFile) {
        std::cerr << "No se pudo crear el archivo de texto de salida: " << textFilePath << "\n";
        return;
    }

    int number;
    bool first = true;

    while (inputFile.read(reinterpret_cast<char*>(&number), sizeof(int))) {
        if (!first) {
            outputFile << ",";
        }
        outputFile << number;
        first = false;
    }

    inputFile.close();
    outputFile.close();
    std::cout << "Page Hits: " << pageHits << "\n";
    std::cout << "Page Faults: " << pageFaults << "\n";
}

// Función para calcular el tamaño del archivo
void calculateFileSize(const std::string& inputFile) {
    std::ifstream input(inputFile, std::ios::binary | std::ios::ate);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + inputFile);
    }

    inputFileSize = static_cast<int>(input.tellg());
    input.close();
}

// Función principal para administrar el proceso de ordenamiento
void sortFile(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& algorithm) {
    calculateFileSize(inputFilePath);
    totalInts = inputFileSize / sizeof(int);
    std::cout << "Número total de enteros en el archivo: " << totalInts << std::endl;

    PaginatedArray paginatedArray(inputFilePath, outputFilePath);

    auto start = std::chrono::high_resolution_clock::now();
    if (algorithm == "QS") {
        quickSort(paginatedArray, 0, totalInts - 1);
    } else if (algorithm == "IS") {
        insertionSort(paginatedArray, totalInts);
    } else if (algorithm == "BS") {
        bubbleSort(paginatedArray, totalInts);
    } else {
        std::cerr << "Algoritmo no reconocido: " << algorithm << std::endl;
        return;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "El proceso de ordenamiento y guardado duró: " << duration.count() << " segundos." << std::endl;
}

// Función principal para leer los argumentos y ejecutar el programa
int main(int argc, char *argv[]) {
    if (argc != 7) {
        std::cerr << "Uso: " << argv[0] << " -input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITHM>" << std::endl;
        return 1;
    }

    std::string inputFilePath;
    std::string outputFilePath;
    std::string algorithm;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-input" && i + 1 < argc) {
            inputFilePath = argv[++i];
        } else if (arg == "-output" && i + 1 < argc) {
            outputFilePath = argv[++i];
        } else if (arg == "-alg" && i + 1 < argc) {
            algorithm = argv[++i];
        }
    }

    if (inputFilePath.empty() || outputFilePath.empty()) {
        std::cerr << "Debe especificar las rutas de los archivos de entrada y salida." << std::endl;
        return 1;
    }

    sortFile(inputFilePath, outputFilePath, algorithm);
    convertBinaryToText(outputFilePath + "/output", outputFilePath + "/outputText.txt");
    return 0;
}
