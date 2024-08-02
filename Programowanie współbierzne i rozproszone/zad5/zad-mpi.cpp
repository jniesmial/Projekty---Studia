
// mpiexec -n 12 zad-mpi.exe 10 10 -5 15 macierz_wynikowa.csv

// mpiexec -n 12 zad-mpi.exe mTest.csv macierz_wynikowa.csv

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <mpi.h>
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
    for (auto& row : matrix) {
        std::sort(row.begin(), row.end());
    }
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv); // Inicjalizacja MPI
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Pobranie rangi (ID) bieżącego procesu
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Pobranie liczby procesów

    // Sprawdzenie poprawności liczby argumentów
    if (argc != 3 && argc != 6) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " m n min max output_file (for generated matrix)\n";
            std::cerr << "Or: " << argv[0] << " input_file output_file (for reading matrix from CSV)\n";
        }
        MPI_Finalize();
        return 1;
    }

    std::vector<std::vector<double>> matrix;
    int m, n, min, max;
    std::string output_file;

    // Jeśli 6 argumentów to generuj macierz
    if (argc == 6) {
        m = std::stoi(argv[1]);
        n = std::stoi(argv[2]);
        min = std::stoi(argv[3]);
        max = std::stoi(argv[4]);
        output_file = argv[5];

        if (rank == 0) {
            matrix = generateMatrix(m, n, min, max);
        }
    }
    // Jeśli 3 argumenty to wczytaj macierz z pliku
    else if (argc == 3) {
        std::string input_file = argv[1];
        output_file = argv[2];

        // Wczytywanie macierzy tylko w procesie głównym
        if (rank == 0) {
            matrix = readMatrixFromCSV(input_file);
            m = matrix.size();
            n = matrix[0].size();
        }

        // Rozsyłanie rozmiarów macierzy do wszystkich procesów
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Start pomiaru czasu
    auto start_time = std::chrono::high_resolution_clock::now();

    // Obliczanie liczby wierszy na proces oraz reszty wierszy
    int rows_per_proc = m / size;
    int remainder = m % size;

    // Alokowanie miejsca na lokalną macierz dla każdego procesu
    std::vector<std::vector<double>> local_matrix(rows_per_proc + (rank < remainder ? 1 : 0), std::vector<double>(n));

    // Proces główny rozsyła wiersze do innych procesów
    if (rank == 0) {
        int offset = rows_per_proc + (rank < remainder ? 1 : 0);
        for (int i = 1; i < size; ++i) {
            int rows = rows_per_proc + (i < remainder ? 1 : 0);
            for (int j = 0; j < rows; ++j) {
                MPI_Send(matrix[offset + j].data(), n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            }
            offset += rows;
        }
        // Proces główny zachowuje swoje wiersze
        local_matrix.assign(matrix.begin(), matrix.begin() + (rows_per_proc + (rank < remainder ? 1 : 0)));
    }
    // Inne procesy odbierają swoje wiersze
    else {
        for (int i = 0; i < local_matrix.size(); ++i) {
            MPI_Recv(local_matrix[i].data(), n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Sortowanie wierszy przez każdy proces lokalnie
    sortMatrixRows(local_matrix);

    // Inne procesy wysyłają swoje posortowane wiersze do procesu głównego
    if (rank != 0) {
        for (const auto& row : local_matrix) {
            MPI_Send(row.data(), n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    // Proces główny odbiera posortowane wiersze od innych procesów
    else {
        int offset = rows_per_proc + (rank < remainder ? 1 : 0);
        for (int i = 1; i < size; ++i) {
            int rows = rows_per_proc + (i < remainder ? 1 : 0);
            for (int j = 0; j < rows; ++j) {
                MPI_Recv(matrix[offset + j].data(), n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            offset += rows;
        }
        // Proces główny aktualizuje macierz swoimi lokalnymi posortowanymi wierszami
        for (int i = 0; i < local_matrix.size(); ++i) {
            matrix[i] = local_matrix[i];
        }
    }

    // Proces główny zapisuje posortowaną macierz do pliku
    if (rank == 0) {
        writeMatrixToCSV(matrix, output_file);
    }

    // stop pomiaru czasu
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;

    if (rank == 0) {
        std::cout << "Elapsed time: " << elapsed_time.count() << " milliseconds" << std::endl;
    }

    // Finalizacja MPI
    MPI_Finalize();
    return 0;
}

