#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>

using namespace std;

// //                        A B C a b n  m
// mpiexec -n 12 zad1mpi.exe 2 5 8 1 99 1000 0

// Struktura reprezentująca parametry zadania
struct TaskParameters {
    double A;
    double B;
    double C;
    double a;
    double b;
    int n;
    int m;
};

double function(double x, double A, double B, double C) {
    return A * x * x + B * x + C;
}

double trapezoidal_integration(TaskParameters params, int rank, int size) {
    double A = params.A;
    double B = params.B;
    double C = params.C;
    double a = params.a;
    double b = params.b;
    int n = params.n;

    double h = (b - a) / n;
    double sum = 0.0;

    // Obliczanie liczby przedziałów dla każdego procesu
    int local_n = n / size;

    // Obliczanie granic przedziału dla danego procesu
    double local_a = a + rank * local_n * h;
    double local_b = local_a + local_n * h;

    // Poprawa granic przedziału dla ostatniego procesu
    if (rank == size - 1) {
        local_n += n % size; // Dodatkowe przedziały dla ostatniego procesu
        local_b = b; // Ostatni proces całkuje do końca przedziału
    }

    // Obliczanie lokalnej sumy
    for (int i = 0; i < local_n; ++i) {
        double x0 = local_a + i * h;
        double x1 = local_a + (i + 1) * h;
        double area = (function(x0, A, B, C) + function(x1, A, B, C)) * h / 2.0;
        sum += area;
    }

    // Synchronizacja wszystkich procesów
    MPI_Barrier(MPI_COMM_WORLD);

    // Zbieranie wyników z wszystkich procesów
    double total_sum = 0.0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return total_sum;
}

// Implementacja metody kwadratów
double midpoint_integration(TaskParameters params, int rank, int size) {

    double A = params.A;
    double B = params.B;
    double C = params.C;
    double a = params.a;
    double b = params.b;
    int n = params.n;

    double h = (b - a) / n;
    double sum = 0.0;

    // Obliczanie liczby przedziałów dla każdego procesu
    int local_n = n / size;

    // Obliczanie granic przedziału dla danego procesu
    double local_a = a + rank * local_n * h + h / 2.0;
    double local_b = local_a + local_n * h;

    // Poprawa granic przedziału dla ostatniego procesu
    if (rank == size - 1) {
        local_n += n % size; // Dodatkowe przedziały dla ostatniego procesu
        local_b = b; // Ostatni proces całkuje do końca przedziału
    }

    // Obliczanie lokalnej sumy
    for (int i = 0; i < local_n; ++i) {
        double x = local_a + i * h;
        double area = function(x, A, B, C) * h;
        sum += area;
    }

    // Synchronizacja wszystkich procesów
    MPI_Barrier(MPI_COMM_WORLD);

    // Zbieranie wyników z wszystkich procesów
    double total_sum = 0.0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return total_sum;
}

double simpson_integration(TaskParameters params, int rank, int size) {
    // Implementacja metody Simpsona

    double A = params.A;
    double B = params.B;
    double C = params.C;
    double a = params.a;
    double b = params.b;
    int n = params.n;

    double h = (b - a) / n;
    double sum = 0.0;

    // Obliczanie liczby przedziałów dla każdego procesu
    int local_n = n / size;

    // Obliczanie granic przedziału dla danego procesu
    double local_a = a + rank * local_n * h;
    double local_b = local_a + local_n * h;

    // Poprawa granic przedziału dla ostatniego procesu
    if (rank == size - 1) {
        local_n += n % size; // Dodatkowe przedziały dla ostatniego procesu
        local_b = b; // Ostatni proces całkuje do końca przedziału
    }

    // Obliczanie lokalnej sumy
    for (int i = 0; i < local_n; ++i) {
        double x0 = local_a + i * h;
        double x1 = local_a + (i + 1) * h;
        double x_mid = (x0 + x1) / 2.0;
        double area = (function(x0, A, B, C) + 4 * function(x_mid, A, B, C) + function(x1, A, B, C)) * h / 6.0;
        sum += area;
    }

    // Synchronizacja wszystkich procesów
    MPI_Barrier(MPI_COMM_WORLD);

    // Zbieranie wyników z wszystkich procesów
    double total_sum = 0.0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return total_sum;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // double A, B, C, a, b;
    // int n, m;
    double start_time, end_time;

    TaskParameters params;

    if (rank == 0) {
        // Pobieranie argumentów linii poleceń
        if (argc != 8) {
            cerr << "Usage: " << argv[0] << " A B C a b n m" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        params.A = atof(argv[1]);
        params.B = atof(argv[2]);
        params.C = atof(argv[3]);
        params.a = atof(argv[4]);
        params.b = atof(argv[5]);
        params.n = atoi(argv[6]);
        params.m = atoi(argv[7]);

        start_time = MPI_Wtime(); // Początek pomiaru czasu
    }

    // Rozgłaszanie parametrów do wszystkich procesów
    MPI_Bcast(&params, sizeof(TaskParameters), MPI_BYTE, 0, MPI_COMM_WORLD);

    double result;

    // Wybór metody obliczania całki
    switch (params.m) {
    case 0:
        result = trapezoidal_integration(params, rank, size);
        break;
    case 1:
        result = midpoint_integration(params, rank, size);
        break;
    case 2:
        result = simpson_integration(params, rank, size);
        break;
    default:
        if (rank == 0) {
            cerr << "Invalid method choice. Choose 0 for trapezoidal, 1 for midpoint, or 2 for Simpson's method." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        end_time = MPI_Wtime(); // Koniec pomiaru czasu
        double time_elapsed_ms = (end_time - start_time) * 1000; // Konwersja na milisekundy
        cout << "Result: " << result << endl;
        cout << "Time elapsed: " << time_elapsed_ms << " milliseconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
