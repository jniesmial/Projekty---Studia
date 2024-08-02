#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;

// .\zad1sekw.exe 2 5 8 1 99 1000

double function(double x, double A, double B, double C) {
    return A * x * x + B * x + C;
}

double integrate(double A, double B, double C, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        double x0 = a + i * h;
        double x1 = a + (i + 1) * h;
        double area = (function(x0, A, B, C) + function(x1, A, B, C)) * h / 2.0;
        sum += area;
    }

    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " A B C a b n" << endl;
        return 1;
    }

    double A = atof(argv[1]);
    double B = atof(argv[2]);
    double C = atof(argv[3]);
    double a = atof(argv[4]);
    double b = atof(argv[5]);
    int n = atoi(argv[6]);

    auto start_time = chrono::steady_clock::now();

    double result = integrate(A, B, C, a, b, n);

    auto end_time = chrono::steady_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "Result: " << result << endl;
    cout << fixed << setprecision(6);
    cout << "Time elapsed: " << elapsed_time << " milliseconds" << endl;

    return 0;
}
