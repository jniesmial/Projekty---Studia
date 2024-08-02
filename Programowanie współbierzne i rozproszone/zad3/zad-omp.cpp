
// .\zad-omp.exe matrixA.csv wektorC.csv 12 G PP logFile.csv


#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <ctime>
#include <string>

using namespace std;

// Funkcja do wyświetlania macierzy w konsoli
void printMatrix(const vector<vector<double>>& matrix) {
    int rows = matrix.size();
    if (rows == 0) return;
    int cols = matrix[0].size();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
}


void readMatrixFromFile(const string& filename, vector<vector<double>>& matrix) {
    ifstream file(filename);
    if (!file) {
        cerr << "Unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    getline(file, line);
    int n = stoi(line);
    matrix.resize(n, vector<double>(n + 1));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (j < n) {
                getline(file, line, ';');
            }
            else {
                getline(file, line);
            }
            matrix[i][j] = stod(line);
        }
    }

    file.close();
}

void writeVectorToFile(const string& filename, const vector<double>& vec) {
    ofstream file(filename);
    if (!file) {
        cerr << "Unable to open file " << filename << endl;
        exit(1);
    }

    file << vec.size() << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        file << fixed << setprecision(6) << vec[i];
        if (i != vec.size() - 1) {
            file << ";";
        }
    }
    file << endl;

    file.close();
}

void logExecutionDetails(const string& logFilename, int numEquations, int numThreads, const string& method, double elapsedTime) {
    ofstream logFile(logFilename, ios_base::app);
    if (!logFile) {
        cerr << "Unable to open log file " << logFilename << endl;
        exit(1);
    }

    time_t now = time(0);
    struct tm timeInfo;
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);

    logFile << "Date and time: " << buffer;
    logFile << "Number of equations: " << numEquations << endl;
    logFile << "Number of threads: " << numThreads << endl;
    logFile << "Method: " << method << endl;
    logFile << "Elapsed time: " << fixed << setprecision(3) << elapsedTime << " milliseconds" << endl;
    logFile << "-----------------------------" << endl;

    logFile.close();
}

void gaussElimination(vector<vector<double>>& matrix, vector<double>& result, int numThreads, string pivotType) {
    int n = matrix.size();

    #pragma omp parallel num_threads(numThreads)
    {
        for (int k = 0; k < n; ++k) {
            #pragma omp single
            {
                if(pivotType == "PP") {
                    // Pivoting
                    int maxIndex = k;
                    for (int i = k + 1; i < n; ++i) {
                        if (abs(matrix[i][k]) > abs(matrix[maxIndex][k])) {
                            maxIndex = i;
                        }
                    }
                    swap(matrix[k], matrix[maxIndex]);
                }

                // normalize pivot row
                double pivot = matrix[k][k];
                for (int j = k; j <= n; ++j) {
                    matrix[k][j] /= pivot;
                }
            }

            // Elimination
            #pragma omp for schedule(static)
            for (int i = k + 1; i < n; ++i) {
                double factor = matrix[i][k];
                for (int j = k; j <= n; ++j) {
                    matrix[i][j] -= factor * matrix[k][j];
                }
            }
        }

        // Back substitution
        #pragma omp single
        {
            result.resize(n);
            for (int i = n - 1; i >= 0; --i) {
                result[i] = matrix[i][n];
                for (int j = i + 1; j < n; ++j) {
                    result[i] -= matrix[i][j] * result[j];
                }
            }
        }
    }
}

void gaussJordanElimination(vector<vector<double>>& matrix, vector<double>& result, int numThreads, string pivotType) {
    int n = matrix.size();

    #pragma omp parallel num_threads(numThreads)
    {
        for (int k = 0; k < n; ++k) {
        #pragma omp single
            {
                if (pivotType == "PP") {
                    // Pivoting
                    int maxIndex = k;
                    for (int i = k + 1; i < n; ++i) {
                        if (abs(matrix[i][k]) > abs(matrix[maxIndex][k])) {
                            maxIndex = i;
                        }
                    }
                    swap(matrix[k], matrix[maxIndex]);

                    // Normalize pivot row
                    double pivot = matrix[k][k];
                    for (int j = k; j <= n; ++j) {
                        matrix[k][j] /= pivot;
                    }
                }
            }

            // Elimination
            #pragma omp for schedule(static)
            for (int i = 0; i < n; ++i) {
                if (i != k) {
                    double factor = matrix[i][k];
                    for (int j = k; j <= n; ++j) {
                        matrix[i][j] -= factor * matrix[k][j];
                    }
                }
            }

            // Elimination
            #pragma omp for schedule(static)
            for (int i = k + 1; i < n; ++i) {
                double factor = matrix[i][k];
                for (int j = k; j <= n; ++j) {
                    matrix[i][j] -= factor * matrix[k][j];
                }
            }
        }

        #pragma omp single
        {
            printMatrix(matrix);
        }

        #pragma omp single
        {
            result.resize(n);
            for (int i = 0; i < n; ++i) {
                result[i] = matrix[i][n];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " <matrix_file> <vector_file> <num_threads> <method> <pivotType> <log_file>" << endl;
        return 1;
    }

    string matrixFile = argv[1];
    string vectorFile = argv[2];
    int numThreads = stoi(argv[3]);
    string method = argv[4];
    string pivotType = argv[5];
    string logFile = argv[6];

    if (pivotType != "PP" && pivotType != "NP") {
        cerr << "Invalid pivot type. Use 'NP' for no pivot or 'PP' for partial pivot." << endl;
        return 1;
    }

    vector<vector<double>> matrix;
    vector<double> result;

    readMatrixFromFile(matrixFile, matrix);
    int numEquations = matrix.size();

    double startTime = omp_get_wtime();
    if (method == "G") {
        gaussElimination(matrix, result, numThreads, pivotType);
    }
    else if (method == "GJ") {
        gaussJordanElimination(matrix, result, numThreads, pivotType);
    }
    else {
        cerr << "Invalid method. Use 'G' for Gauss or 'GJ' for Gauss-Jordan." << endl;
        return 1;
    }
    double endTime = omp_get_wtime();

    double elapsedTime = (endTime - startTime) * 1000.0; // Convert to milliseconds

    writeVectorToFile(vectorFile, result);
    logExecutionDetails(logFile, numEquations, numThreads, method, elapsedTime);

    cout << fixed << setprecision(3) << "Elapsed time: " << elapsedTime << " milliseconds" << endl;

    return 0;
}
