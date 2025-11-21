#include <chrono>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>

#define MEASURE_START auto __start = std::chrono::high_resolution_clock::now();
#define MEASURE_END auto __end = std::chrono::high_resolution_clock::now();
#define MEASURE_MICROS std::chrono::duration_cast<std::chrono::microseconds>(__end - __start).count()

void insertionSort(std::vector<int> &data) {
    for (std::size_t i = 1; i < data.size(); ++i) {
        int key = data[i];
        std::size_t j = i;
        while (j > 0 && data[j - 1] > key) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = key;
    }
}

void merge(std::vector<int> &data, std::size_t left, std::size_t mid, std::size_t right) {
    std::vector<int> temp;
    temp.reserve(right - left + 1);

    std::size_t i = left;
    std::size_t j = mid + 1;

    while (i <= mid && j <= right) {
        if (data[i] <= data[j]) {
            temp.push_back(data[i]);
            ++i;
        } else {
            temp.push_back(data[j]);
            ++j;
        }
    }

    while (i <= mid) {
        temp.push_back(data[i]);
        ++i;
    }

    while (j <= right) {
        temp.push_back(data[j]);
        ++j;
    }

    for (std::size_t k = 0; k < temp.size(); ++k) {
        data[left + k] = temp[k];
    }
}

void mergeSortImpl(std::vector<int> &data, std::size_t left, std::size_t right) {
    if (left >= right) {
        return;
    }
    std::size_t mid = left + (right - left) / 2;
    mergeSortImpl(data, left, mid);
    mergeSortImpl(data, mid + 1, right);
    merge(data, left, mid, right);
}

void mergeSort(std::vector<int> &data) {
    if (!data.empty()) {
        mergeSortImpl(data, 0, data.size() - 1);
    }
}

std::vector<std::string> listFiles(const std::string &path) {
    std::vector<std::string> files;
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        return files;
    }

    dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (entry->d_type == DT_REG && !name.empty() && name[0] != '.') {
            files.push_back(name);
        }
    }

    closedir(dir);
    return files;
}

std::vector<int> readNumbersFromFile(const std::string &filepath) {
    std::vector<int> numbers;
    std::ifstream in(filepath);
    if (!in) {
        std::cerr << "No se pudo abrir el archivo: " << filepath << "\n";
        return numbers;
    }

    int value = 0;
    while (in >> value) {
        numbers.push_back(value);
    }
    return numbers;
}

std::string buildOutputName(const std::string &inputName, const std::string &suffix) {
    std::size_t lastDot = inputName.rfind('.');
    if (lastDot == std::string::npos) {
        return inputName + suffix;
    }

    std::string base = inputName.substr(0, lastDot);
    std::string extension = inputName.substr(lastDot);
    return base + suffix + extension;
}

long long sortNumbers(std::vector<int> &numbers, int algorithm) {
    MEASURE_START
        if (algorithm == 1) {
        insertionSort(numbers);
    } else {
        mergeSort(numbers);
    }
    MEASURE_END
        return MEASURE_MICROS;
}

void ensureDirectory(const std::string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        mkdir(path.c_str(), 0755);
    }
}

void saveOrderedFile(const std::string &outputPath, const std::vector<int> &numbers, long long elapsedMicros) {
    std::ofstream out(outputPath);
    if (!out) {
        std::cerr << "No se pudo escribir el archivo: " << outputPath << "\n";
        return;
    }

    for (std::size_t i = 0; i < numbers.size(); ++i) {
        out << numbers[i];
        if (i + 1 < numbers.size()) {
            out << ' ';
        }
    }
    out << "\n" << elapsedMicros << "\n";
    std::cout << "Archivo generado: " << outputPath << "\n";
}

void showFileContent(const std::string &filepath) {
    std::ifstream in(filepath);
    if (!in) {
        std::cerr << "No se pudo abrir el archivo: " << filepath << "\n";
        return;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(in, line)) {
        if (lineNumber == 0) {
            std::cout << "Números ordenados: " << line << "\n";
        } else if (lineNumber == 1) {
            std::cout << "Tiempo de ejecución (microsegundos): " << line << "\n";
        }
        ++lineNumber;
    }
}

void listAvailableFiles(const std::vector<std::string> &files, const std::string &label) {
    std::cout << "Archivos disponibles en '" << label << "':\n";
    if (files.empty()) {
        std::cout << "  (No hay archivos disponibles)\n";
        return;
    }
    for (std::size_t i = 0; i < files.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << files[i] << '\n';
    }
}

int main() {
    const std::string inputDir = "arreglos";
    const std::string outputDir = "ordenados";

    ensureDirectory(inputDir);
    ensureDirectory(outputDir);

    int option = 0;
    while (true) {
        std::cout << "\nMenú principal\n";
        std::cout << "1. Listar archivos en 'arreglos'\n";
        std::cout << "2. Seleccionar archivo, ordenar y guardar en 'ordenados'\n";
        std::cout << "3. Ver contenido de un archivo ordenado\n";
        std::cout << "4. Salir\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> option;

        if (option == 4 || std::cin.fail()) {
            std::cout << "Saliendo...\n";
            break;
        }

        if (option == 1) {
            std::vector<std::string> files = listFiles(inputDir);
            listAvailableFiles(files, inputDir);
        } else if (option == 2) {
            std::vector<std::string> files = listFiles(inputDir);
            listAvailableFiles(files, inputDir);
            if (files.empty()) {
                continue;
            }

            std::cout << "Seleccione un archivo por número: ";
            std::size_t selection = 0;
            std::cin >> selection;
            if (selection == 0 || selection > files.size()) {
                std::cout << "Selección inválida.\n";
                continue;
            }

            std::cout << "Elija algoritmo:\n1. Insertion Sort\n2. Merge Sort\nOpción: ";
            int algOption = 0;
            std::cin >> algOption;
            if (algOption != 1 && algOption != 2) {
                std::cout << "Selección inválida.\n";
                continue;
            }

            std::string inputPath = inputDir + "/" + files[selection - 1];
            std::vector<int> numbers = readNumbersFromFile(inputPath);
            if (numbers.empty()) {
                std::cout << "El archivo no contiene números o no se pudo leer.\n";
                continue;
            }

            long long elapsed = sortNumbers(numbers, algOption);
            std::string suffix = (algOption == 1) ? "_insert" : "_merge";
            std::string outputName = buildOutputName(files[selection - 1], suffix);
            std::string outputPath = outputDir + "/" + outputName;
            saveOrderedFile(outputPath, numbers, elapsed);
        } else if (option == 3) {
            std::vector<std::string> files = listFiles(outputDir);
            listAvailableFiles(files, outputDir);
            if (files.empty()) {
                continue;
            }
            std::cout << "Seleccione un archivo por número: ";
            std::size_t selection = 0;
            std::cin >> selection;
            if (selection == 0 || selection > files.size()) {
                std::cout << "Selección inválida.\n";
                continue;
            }
            std::string filepath = outputDir + "/" + files[selection - 1];
            showFileContent(filepath);
        } else {
            std::cout << "Opción no reconocida.\n";
        }
    }

    return 0;
}
