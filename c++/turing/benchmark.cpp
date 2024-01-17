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

    auto t1 = high_resolution_clock::now();
    for (int i = 0; i < CASES; i++) {
        if (test(inputs[i]) != results[i]) {
            cout << i << ": " << inputs[i] << ", " << results[i] << " -> "<< "FAIL" << endl;
        }
    }
    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms = t2 - t1;
    cout << ms.count() << " ms, " << (ms.count() / CASES) << " ms/case" << endl;

}