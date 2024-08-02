

//                                                     \/ collapse()
//                                                          \/ schedule() - static, dynamic, collapse
// .\zad-omp.exe matrixA1.csv matrixB1.csv matrixC1.csv 1 2 static

// .\zad-omp.exe matrixA2.csv matrixB2.csv matrixC2.csv 12 2 static

// .\zad-omp.exe matrixA3.csv matrixB3.csv matrixC3.csv 12 2 dynamic



#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iomanip>

std::vector<std::vector<double>> readMatrixFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::vector<std::vector<double>> matrix;

    // pierwsza linia - wiersze
    std::string line;
    std::getline(file, line);
    int rows = std::stoi(line);

    // druga linia - kolumny
    std::getline(file, line);
    int cols = std::stoi(line);

    // pętla dla wartośći właściwych macierzy
    while (std::getline(file, line)) {
        std::vector<double> row;
        std::stringstream ss(line);
        double value;
        for (int i = 0; i < cols; ++i) {
            ss >> value;
            row.push_back(value);
            if (ss.peek() == ';') ss.ignore();
        }
        matrix.push_back(row);
    }

    file.close();
    return matrix;
}

void writeMatrixToCSV(const std::vector<std::vector<double>>& matrix, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Dwie pierwsze linie wierszy i kolumn
    file << matrix.size() << "\n";
    if (!matrix.empty()) {
        file << matrix[0].size() << "\n";
    }

    // Zapisywanie wartości z tablicy do pliku CSV z średnikami
    for (const auto& row : matrix) {
        for (size_t i = 0; i < row.size(); i++) {
            file << std::fixed << std::setprecision(6) << row[i];
            if (i < row.size() - 1) {
                file << ";";
            }
        }
        file << "\n";
    }

    file.close();
}


int main(int argc, char* argv[]) {

    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " <matrix_A.csv> <matrix_B.csv> <output_matrix_C.csv> <num_threads> <schedule> <collapse>\n";
        return 1;
    }
  

    std::vector<std::vector<double>> A = readMatrixFromCSV(argv[1]);
    std::vector<std::vector<double>> B = readMatrixFromCSV(argv[2]);

    if (A[0].size() != B.size()) {
        std::cerr << "Matrix dimensions must be compatible for multiplication.\n";
        return 1;
    }

    double startTime = omp_get_wtime();

    int n = A.size();
    int m = A.empty() ? 0 : A[0].size();
    int p = B.empty() ? 0 : B[0].size();
    std::vector<std::vector<double>> C(n, std::vector<double>(p, 0.0));
    
    int numThreads = std::stoi(argv[4]);
    int collapseLevel = std::stoi(argv[5]);

    std::string scheduleType = argv[6];

    // Ustawiamy domyślnie "static" jeśli nie podano innego harmonogramu
    // const std::string scheduleType = (argc == 7) ? argv[6] : "static";
    
    /*
    #pragma omp parallel for collapse(collapseLevel) schedule(scheduleType) num_threads(numThreads)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < m; k++) {
                #pragma omp atomic
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    */

    // Wybierz odpowiedni harmonogram w zależności od wartości zmiennej scheduleType
    if (scheduleType == "static") {
        #pragma omp parallel for collapse(collapseLevel) schedule(static) num_threads(numThreads)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                for (int k = 0; k < m; k++) {
                    #pragma omp atomic
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    else if (scheduleType == "dynamic") {
        #pragma omp parallel for collapse(collapseLevel) schedule(dynamic) num_threads(numThreads)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                for (int k = 0; k < m; k++) {
                    #pragma omp atomic
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    else if (scheduleType == "guided") {
        #pragma omp parallel for collapse(collapseLevel) schedule(guided) num_threads(numThreads)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                for (int k = 0; k < m; k++) {
                    #pragma omp atomic
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    else {
        std::cerr << "Podano niewlasciwy harmonogram klauzuli schedule().\n";
        return 1;
    }

    // -------------------------------------------------

    double endTime = omp_get_wtime();

    writeMatrixToCSV(C, argv[3]);

    std::cout << "Matrix multiplication completed successfully." << std::endl;
    std::cout << "Multiplication time: " << (endTime - startTime)*1000 << " miliseconds." << std::endl;

    return 0;
}