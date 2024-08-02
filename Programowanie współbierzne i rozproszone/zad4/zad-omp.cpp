
// .\zad-omp.exe 10 10 -5 15 macierz_wynikowa.csv 12

// .\zad-omp.exe mTest.csv macierz_wynikowa.csv 12

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <omp.h>
#include <chrono>

// Funkcja do zaokrąglania
double roundToSixDecimalPlaces(double value) {
    return std::round(value * 1000000.0) / 1000000.0;
}

// Funkcja do losowania macierzy
std::vector<std::vector<double>> generateMatrix(int m, int n, int min, int max) {
    std::vector<std::vector<double>> matrix(m, std::vector<double>(n));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = roundToSixDecimalPlaces(dis(gen));
        }
    }

    return matrix;
}

// Funkcja do wczytywania macierzy
std::vector<std::vector<double>> readMatrixFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::vector<std::vector<double>> matrix;

    std::string line;
    std::getline(file, line);
    int rows = std::stoi(line);

    std::getline(file, line);
    int cols = std::stoi(line);

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

// Funkcja do zapisywania macierzy do pliku .csv
void writeMatrixToCSV(const std::vector<std::vector<double>>& matrix, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    file << matrix.size() << "\n";
    if (!matrix.empty()) {
        file << matrix[0].size() << "\n";
    }

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

// Funkcja do sortowania macierzy
void sortMatrixRows(std::vector<std::vector<double>>& matrix) {
#pragma omp parallel for
    for (int i = 0; i < matrix.size(); ++i) {
        std::sort(matrix[i].begin(), matrix[i].end());
    }
}

int main(int argc, char* argv[]) {
    // Sprawdzenie poprawności liczby argumentów
    if (argc != 4 && argc != 7) {
        std::cerr << "Usage: " << argv[0] << " m n min max output_file P (for generated matrix)\n";
        std::cerr << "Or: " << argv[0] << " input_file output_file P (for reading matrix from CSV)\n";
        return 1;
    }

    std::vector<std::vector<double>> matrix;
    int m, n, min, max;
    std::string output_file;
    int P;

    // Jeśli 7 argumentów to generuj macierz
    if (argc == 7) {
        m = std::stoi(argv[1]);
        n = std::stoi(argv[2]);
        min = std::stoi(argv[3]);
        max = std::stoi(argv[4]);
        output_file = argv[5];
        P = std::stoi(argv[6]);

        matrix = generateMatrix(m, n, min, max);
    }
    // Jeśli 4 argumenty to wczytaj macierz z pliku
    else if (argc == 4) {
        std::string input_file = argv[1];
        output_file = argv[2];
        P = std::stoi(argv[3]);

        matrix = readMatrixFromCSV(input_file);
        m = matrix.size();
        n = matrix[0].size();
    }

    // Ustawienie liczby wątków
    omp_set_num_threads(P);

    // Start pomiaru czasu
    auto start_time = std::chrono::high_resolution_clock::now();

    // Sortowanie wierszy przez każdy wątek lokalnie
    sortMatrixRows(matrix);

    // Stop pomiaru czasu
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;

    std::cout << "Elapsed time: " << elapsed_time.count() << " milliseconds" << std::endl;

    // Zapisanie posortowanej macierzy do pliku
    writeMatrixToCSV(matrix, output_file);

    return 0;
}
