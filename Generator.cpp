#include <iostream>
#include <fstream>
#include <random>
#include <cstring>
#include <filesystem>

void generateFile(const std::string& outputDir, size_t sizeInBytes) {
    std::filesystem::path dir(outputDir);
    std::filesystem::path filePath = dir / "generated_file.bin";

    std::ofstream outputFile(filePath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura: " << filePath << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist;

    size_t numIntegers = sizeInBytes / sizeof(int);

    for (size_t i = 0; i < numIntegers; ++i) {
        int randomInt = dist(gen);
        outputFile.write(reinterpret_cast<const char*>(&randomInt), sizeof(randomInt));
    }

    outputFile.close();
    if (outputFile.good()) {
        std::cout << "Archivo generado correctamente: " << filePath << std::endl;
    } else {
        std::cerr << "Error al escribir en el archivo." << std::endl;
    }
}

size_t parseSizeArgument(const std::string& sizeArg) {
    if (sizeArg == "SMALL") {
        return 512 * 1024 * 1024; // 512 MB
    } else if (sizeArg == "MEDIUM") {
        return 1024 * 1024 * 1024; // 1 GB
    } else if (sizeArg == "LARGE") {
        return 2 * 1024 * 1024 * 1024; // 2 GB
    } else {
        throw std::invalid_argument("Tamaño no válido. Debe ser SMALL, MEDIUM o LARGE.");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Uso: " << argv[0] << " --size <SIZE> --output <OUTPUT DIRECTORY PATH>" << std::endl;
        return 1;
    }

    std::string sizeArg;
    std::string outputDir;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--size") == 0) {
            sizeArg = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0) {
            outputDir = argv[++i];
        }
    }

    try {
        size_t sizeInBytes = parseSizeArgument(sizeArg);
        generateFile(outputDir, sizeInBytes);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}