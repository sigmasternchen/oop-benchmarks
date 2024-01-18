#include <iostream>
#include <cstdlib>
#include <chrono>

#include "common.h"

using namespace std;
using namespace std::chrono;

#define SEED (1337)

int inputs[CASES];
bool results[CASES];

extern bool test(int input);

bool isBinaryPalindrome(int input) {
    int numberOfDigits = input == 0 ? 1 : static_cast<int>(floor(log2(input)) + 1);

    for (int i = 0; i < numberOfDigits / 2; i++) {
        if (((input & (1 << i)) ? 1 : 0) != ((input & (1 << (numberOfDigits - i - 1))) ? 1 : 0)) {
            return false;
        }
    }

    return true;
}

void resetTestData() {
    srand(SEED);

    for (int i = 0; i < CASES; i++) {
        inputs[i] = rand() % (MAX_VALUE + 1);
        results[i] = isBinaryPalindrome(inputs[i]);
    }
}

int main() {
    resetTestData();

    double sum = 0;
    double sum_squared = 0;

    for (int r = 0; r < REPEATS; r++) {
        auto t1 = high_resolution_clock::now();
        for (int i = 0; i < CASES; i++) {
            bool result = test(inputs[i]);
            if (result != results[i]) {
                cout << i << ": " << inputs[i] << ", " << results[i] << " -> " << "FAIL" << endl;
            }
        }
        auto t2 = high_resolution_clock::now();

        duration<double, std::milli> ms = t2 - t1;
        double tmp = ms.count();

        sum += tmp;
        sum_squared += tmp * tmp;
    }

    double mu = sum / REPEATS;
    // from https://en.wikipedia.org/wiki/Standard_deviation#Rapid_calculation_methods
    double sigma = sqrt((REPEATS * sum_squared - sum * sum) / (REPEATS * (static_cast<double>(REPEATS) - 1)));

    cout << "mu = " << mu << " ms, sigma = " << sigma << " ms\n";

}