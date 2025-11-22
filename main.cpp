#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

#define TIME_START auto __inicio = std::chrono::high_resolution_clock::now();
#define TIME_END auto __fin = std::chrono::high_resolution_clock::now();
#define TIME_MICROS std::chrono::duration_cast<std::chrono::microseconds>(__fin - __inicio).count()


void insertionSort(std::vector<int> &arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        int actual = arr[i];
        int j = static_cast<int>(i - 1);

        while (j >= 0 && arr[j] > actual) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = actual;
    }
}


void merge(std::vector<int> &arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1);
    std::vector<int> R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(std::vector<int> &arr, int left, int right) {
    if (left >= right) return;

    int mid = (left + right) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}


bool cargarVector(std::vector<int> &datos, const std::string &ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return false;

    int numero;
    while (archivo >> numero) {
        datos.push_back(numero);
    }

    return true;
}


void guardarArchivoOrdenado(const std::vector<int> &datos, long long tiempo,
                            const std::string &nombre, const std::string &algoritmo) {
    std::string salida = "ordenados/" + nombre;

    if (algoritmo == "insertion") salida.insert(salida.size() - 4, "_insert");
    else salida.insert(salida.size() - 4, "_merge");

    std::ofstream archivo(salida);


    for (size_t i = 0; i < datos.size(); i++) {
        archivo << datos[i];
        if (i + 1 < datos.size()) archivo << " ";
    }
    archivo << "\n";


    archivo << tiempo;
}


std::vector<std::string> listarArchivos(const std::string &carpeta) {
    std::vector<std::string> lista;

    for (const auto &file : std::filesystem::directory_iterator(carpeta)) {
        if (file.is_regular_file()) {
            lista.push_back(file.path().filename().string());
        }
    }

    return lista;
}


int main() {
    while (true) {
        std::cout << "\n========== MENU PRINCIPAL ==========\n";
        std::cout << "1. Listar archivos en 'arreglos'\n";
        std::cout << "2. Seleccionar archivo, ordenar y guardar\n";
        std::cout << "3. Ver contenido de archivo ordenado\n";
        std::cout << "4. Salir\n";
        std::cout << "Ingrese una opcion: ";

        int opcion;
        std::cin >> opcion;


        if (opcion == 1) {
            auto archivos = listarArchivos("arreglos");

            std::cout << "\nArchivos disponibles en 'arreglos':\n";
            for (size_t i = 0; i < archivos.size(); i++) {
                std::cout << "[" << i + 1 << "] " << archivos[i] << "\n";
            }
        }


        else if (opcion == 2) {
            auto archivos = listarArchivos("arreglos");

            if (archivos.empty()) {
                std::cout << "No hay archivos en la carpeta.\n";
                continue;
            }

            std::cout << "\nArchivos disponibles:\n";
            for (size_t i = 0; i < archivos.size(); i++) {
                std::cout << "[" << i + 1 << "] " << archivos[i] << "\n";
            }

            std::cout << "\nSeleccione archivo por numero: ";
            int eleccion;
            std::cin >> eleccion;

            if (eleccion < 1 || eleccion > (int)archivos.size()) {
                std::cout << "Numero invalido.\n";
                continue;
            }

            std::string archivoElegido = archivos[eleccion - 1];

            std::cout << "\nSeleccione algoritmo:\n";
            std::cout << "1. Insertion Sort\n";
            std::cout << "2. Merge Sort\n";
            std::cout << "Elija: ";

            int alg;
            std::cin >> alg;

            std::vector<int> datos;
            std::string ruta = "arreglos/" + archivoElegido;

            if (!cargarVector(datos, ruta)) {
                std::cout << "Error al leer el archivo.\n";
                continue;
            }

            TIME_START

                if (alg == 1) {
                insertionSort(datos);
                TIME_END
                    guardarArchivoOrdenado(datos, TIME_MICROS, archivoElegido, "insertion");
                std::cout << "\nArchivo generado con Insertion Sort.\n";
            }
            else if (alg == 2) {
                mergeSort(datos, 0, datos.size() - 1);
                TIME_END
                    guardarArchivoOrdenado(datos, TIME_MICROS, archivoElegido, "merge");
                std::cout << "\nArchivo generado con Merge Sort.\n";
            }
            else {
                std::cout << "Algoritmo invalido.\n";
                continue;
            }
        }


        else if (opcion == 3) {
            auto archivos = listarArchivos("ordenados");

            if (archivos.empty()) {
                std::cout << "No hay archivos en la carpeta 'ordenados'.\n";
                continue;
            }

            std::cout << "\nArchivos disponibles en 'ordenados':\n";
            for (size_t i = 0; i < archivos.size(); i++) {
                std::cout << "[" << i + 1 << "] " << archivos[i] << "\n";
            }

            std::cout << "\nSeleccione archivo por numero: ";
            int eleccion;
            std::cin >> eleccion;

            if (eleccion < 1 || eleccion > (int)archivos.size()) {
                std::cout << "Numero invalido.\n";
                continue;
            }

            std::ifstream archivo("ordenados/" + archivos[eleccion - 1]);

            if (!archivo.is_open()) {
                std::cout << "No se pudo abrir el archivo.\n";
                continue;
            }

            std::string linea;

            std::cout << "\nContenido del archivo:\n";

            while (std::getline(archivo, linea)) {
                std::cout << linea << "\n";
            }
        }


        else if (opcion == 4) {
            std::cout << "Saliendo del programa...\n";
            break;
        }

        else {
            std::cout << "Opcion no valida.\n";
        }
    }

    return 0;
}
