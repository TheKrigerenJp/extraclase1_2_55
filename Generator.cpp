#include <iostream>
#include <fstream>
#include <random>
#include <cstring>
//:)
void generateRandomFile(const std::string& filePath, size_t sizeInBytes);

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Uso: " << argv[0] << " --size <SIZE> --output <OUTPUT FILE PATH>" << std::endl;
        return 1;
    }

    std::string sizeStr, outputPath;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--size") == 0) {
            sizeStr = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0) {
            outputPath = argv[++i];
        }
    }

    // Verificar que la ruta de salida comience con '/'
    if (outputPath[0] != '/') {
        std::cerr << "Error: La ruta de salida debe ser absoluta y comenzar con '/'." << std::endl;
        return 1;
    }

    size_t sizeInBytes;
    if (sizeStr == "SMALL") {
        sizeInBytes = 512 * 1000 * 1000; // 512 MB
    } else if (sizeStr == "MEDIUM") {
        sizeInBytes = 1000 * 1000 * 1000; // 1 GB
    } else if (sizeStr == "LARGE") {
        sizeInBytes = 2 * 1000 * 1000 * 1000; // 2 GB
    } else {
        std::cerr << "Tamaño no válido. Debe ser SMALL, MEDIUM o LARGE." << std::endl;
        return 1;
    }

    try {
        std::cout << "Generando archivo de tamaño: " << sizeInBytes << " bytes" << std::endl;
        generateRandomFile(outputPath, sizeInBytes);
        std::cout << "Archivo generado correctamente en: " << outputPath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void generateRandomFile(const std::string& filePath, size_t sizeInBytes) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("No se pudo abrir el archivo para escritura: " + filePath);
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());

    size_t numIntegers = sizeInBytes / sizeof(int);
    for (size_t i = 0; i < numIntegers; ++i) {
        int randomNumber = dist(mt);
        outFile.write(reinterpret_cast<char*>(&randomNumber), sizeof(int));
    }

    outFile.close();
}
