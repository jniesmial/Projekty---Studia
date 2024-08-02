// mpiexec -n 12 zad-mpi.exe matrixA1.csv matrixB1.csv matrixC.csv

// mpiexec -n 12 zad-mpi.exe matrixA2.csv matrixB2.csv matrixC.csv

// mpiexec -n 12 zad-mpi.exe mA.csv mB.csv mC.csv




#include <mpi.h>
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
    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Sprawdzenie czy liczba argumentów jest poprawna
    if (argc < 4) {
        if (rank == 0) {
            std::cerr << "Uzycie: " << argv[0] << " <matrix_A.csv> <matrix_B.csv> <output_matrix_C.csv>\n";
        }
        MPI_Finalize();
        return 1;
    }

    std::vector<std::vector<double>> A, B;
    if (rank == 0) {
        // Wczytanie macierzy A i B z plików CSV na procesie głównym (rank == 0)
        A = readMatrixFromCSV(argv[1]);
        B = readMatrixFromCSV(argv[2]);

        // Sprawdzenie poprawności wymiarów macierzy
        if (A[0].size() != B.size()) {
            std::cerr << "Zle wymiary.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Pomiar czasu rozpoczęcia obliczeń
    double startTime = MPI_Wtime();

    int n, m, p;
    if (rank == 0) {
        n = A.size();
        m = A[0].size();
        p = B[0].size();
    }
    // Rozgłoszenie wymiarów macierzy n, m, p
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Obliczenie liczby wierszy przypada na każdy proces
    int rowsPerProc = n / size + (rank < n% size);
    // Lokalne bufory dla fragmentów macierzy A i C
    std::vector<double> localA(rowsPerProc * m);
    std::vector<double> localC(rowsPerProc * p, 0.0);

    // Obliczenie parametrów dla funkcji MPI_Scatterv
    std::vector<int> sendCounts(size);
    std::vector<int> displacements(size);
    int displacement = 0;
    for (int i = 0; i < size; i++) {
        sendCounts[i] = (n / size + (i < n% size)) * m;
        displacements[i] = displacement;
        displacement += sendCounts[i];
    }

    // Rozgłoszenie fragmentów macierzy A do wszystkich procesów
    std::vector<double> sendBuffer;
    if (rank == 0) {
        sendBuffer.resize(n * m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                sendBuffer[i * m + j] = A[i][j];
            }
        }
    }
    MPI_Scatterv(rank == 0 ? sendBuffer.data() : nullptr, sendCounts.data(), displacements.data(), MPI_DOUBLE, localA.data(), rowsPerProc * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Rozgłoszenie macierzy B do wszystkich procesów
    std::vector<double> flatB(m * p);
    if (rank == 0) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < p; j++) {
                flatB[i * p + j] = B[i][j];
            }
        }
    }
    MPI_Bcast(flatB.data(), m * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Obliczenie fragmentów macierzy C przez każdy proces
    for (int i = 0; i < rowsPerProc; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < m; k++) {
                localC[i * p + j] += localA[i * m + k] * flatB[k * p + j];
            }
        }
    }

    // Zebranie fragmentów macierzy C na procesie głównym
    std::vector<double> receiveBuffer(n * p);
    std::vector<int> recvCounts(size);
    std::vector<int> recvDisplacements(size);
    displacement = 0;
    for (int i = 0; i < size; i++) {
        recvCounts[i] = (n / size + (i < n% size)) * p;
        recvDisplacements[i] = displacement;
        displacement += recvCounts[i];
    }

    MPI_Gatherv(localC.data(), rowsPerProc * p, MPI_DOUBLE, receiveBuffer.data(), recvCounts.data(), recvDisplacements.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Synchronizacja wszystkich procesów
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // Przetworzenie danych na macierz C i zapis do pliku CSV
        std::vector<std::vector<double>> C(n, std::vector<double>(p));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                C[i][j] = receiveBuffer[i * p + j];
            }
        }
        writeMatrixToCSV(C, argv[3]);
        // Pomiar czasu zakończenia obliczeń i wyświetlenie czasu wykonania
        double endTime = MPI_Wtime();
        std::cout << "Czas: " << endTime - startTime << " s." << std::endl;
    }

    // Zakończenie działania MPI
    MPI_Finalize();
    return 0;
}

