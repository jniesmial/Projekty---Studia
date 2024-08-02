#include <iostream>
#include <cmath>
#include <stdio.h>
#include <omp.h>
#include <iomanip>

using namespace std;
//               A B C a b n  m
// .\zad1omp.exe 2 5 8 1 99 1000 0

const int THREAD_COUNT = 12;

double function(double x, double A, double B, double C) {
    return A * x * x + B * x + C;
}

double trapezoidal(double A, double B, double C, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel for num_threads(THREAD_COUNT) reduction(+:sum)
    for (int i = 0; i < n; ++i) {
        double x0 = a + i * h;
        double x1 = a + (i + 1) * h;
        double area = (function(x0, A, B, C) + function(x1, A, B, C)) * h / 2.0;
        sum += area;
    }

    return sum;
}

double midpoint(double A, double B, double C, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel for num_threads(THREAD_COUNT) reduction(+:sum)
    for (int i = 0; i < n; ++i) {
        double x = a + (i + 0.5) * h;
        double area = function(x, A, B, C) * h;
        sum += area;
    }

    return sum;
}

double simpson(double A, double B, double C, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel for num_threads(THREAD_COUNT) reduction(+:sum)
    for (int i = 0; i < n; ++i) {
        double x0 = a + i * h;
        double x1 = a + (i + 1) * h;
        double x_mid = (x0 + x1) / 2.0;
        double area = (function(x0, A, B, C) + 4 * function(x_mid, A, B, C) + function(x1, A, B, C)) * h / 6.0;
        sum += area;
    }

    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 8) {
        cerr << "Usage: " << argv[0] << " A B C a b n m" << endl;
        return 1;
    }

    double A = atof(argv[1]);
    double B = atof(argv[2]);
    double C = atof(argv[3]);
    double a = atof(argv[4]);
    double b = atof(argv[5]);
    int n = atoi(argv[6]);
    int method = atoi(argv[7]);

    // deklaracja wskaźnika do funkcji, która przyjmuje sześć argumentów
    double (*integration_method)(double, double, double, double, double, int);

    switch (method) {
    case 0:
        integration_method = trapezoidal;
        break;
    case 1:
        integration_method = midpoint;
        break;
    case 2:
        integration_method = simpson;
        break;
    default:
        cerr << "Invalid method choice. Choose 0 for trapezoidal, 1 for midpoint, or 2 for Simpson." << endl;
        return 1;
    }

    double start = omp_get_wtime();

    double result = integration_method(A, B, C, a, b, n);

    double stop = omp_get_wtime();
    double elapsed_time = (stop - start) * 1000;

    cout << "Result: " << result << endl;
    cout << fixed << setprecision(4);
    cout << "Time elapsed: " << elapsed_time << " milliseconds" << endl;

    return 0;
}
