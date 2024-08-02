// .\zad-thr.exe matrixA1.csv matrixB1.csv matrixC.csv 1


#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <thread>
#include <chrono>

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

    // pętla dla wartości właściwych macierzy
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

void multiplyMatrices(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B, std::vector<std::vector<double>>& C, int startRow, int endRow) {
    int n = A.size();
    int m = A.empty() ? 0 : A[0].size();
    int p = B.empty() ? 0 : B[0].size();

    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < p; ++j) {
            C[i][j] = 0.0;
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Sprawdzanie ilości argumentów z wiersza poleceń
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <matrix_A.csv> <matrix_B.csv> <output_matrix_C.csv> <num_threads>\n";
        return 1;
    }

    auto startTime = std::chrono::steady_clock::now(); // Punkt początkowy pomiaru czasu


    // Wczytanie macierzy wejściowych z plików
    std::vector<std::vector<double>> A = readMatrixFromCSV(argv[1]);
    std::vector<std::vector<double>> B = readMatrixFromCSV(argv[2]);

    if (A[0].size() != B.size()) {
        std::cerr << "Matrix dimensions must be compatible for multiplication.\n";
        return 1;
    }

    int numThreads = std::stoi(argv[4]);

    std::vector<std::vector<double>> C(A.size(), std::vector<double>(B[0].size()));

    int rowsPerThread = A.size() / numThreads;
    int extraRows = A.size() % numThreads;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = startRow + rowsPerThread;
        if (i == numThreads - 1) {
            endRow += extraRows;
        }
        threads.emplace_back(multiplyMatrices, std::ref(A), std::ref(B), std::ref(C), startRow, endRow);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::steady_clock::now(); // Punkt końcowy pomiaru czasu
    // Obliczenie różnicy czasów i przekształcenie jej na sekundy
    double executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

    // Zapisanie macierzy wynikowej do pliku CSV
    writeMatrixToCSV(C, argv[3]);

    std::cout << "Matrix multiplication completed successfully." << std::endl;

    std::cout << "Multiplication time: " << std::fixed << std::setprecision(4) << executionTime / 1000000 << " milliseconds." << std::endl;


    return 0;
}
