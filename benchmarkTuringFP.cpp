#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define CASES (100000)
#define MAX_VALUE (0xffff)

#define MAX_BAND_SIZE (MAX_VALUE + 1)

#define SEED (1337)

int inputs[CASES];
bool results[CASES];

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

enum Direction {
    LEFT, RIGHT
};

enum Symbol {
    UNDEFINED, TRUE, FALSE
};

#define returnBand(x) { function<Symbol (int)> _f = x; return _f; }

auto emptyBand() {
    return [](int _) { return UNDEFINED; };
}

auto setOnBand(int pos, Symbol symbol, const function<Symbol (int)>& band) {
    return [=](int i) {
        if (i == pos) {
            return symbol;
        } else {
            return band(i);
        }
    };
}

auto inputBitToBand(int input, int bit, int length) {
    if (bit == length) {
        returnBand(emptyBand());
    }

    returnBand(setOnBand(
        bit,
        (input & (1 << (length - bit - 1))) ? TRUE : FALSE,
        inputBitToBand(input, bit + 1, length)
    ));
}

auto inputToBand(int input) {
    if (input == 0) {
        returnBand(emptyBand());
    }

    int numberOfDigits = static_cast<int>(floor(log2(input)) + 1);
    returnBand(inputBitToBand(input, 0, numberOfDigits));
}

auto transitionTable(int state, Symbol symbolRead) {
    switch (state) {
        case 0: // init; look for first symbol
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(10, RIGHT, UNDEFINED);
                case FALSE:     return make_tuple(20, RIGHT, UNDEFINED);
            }
        case 10: // look for right hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(11, LEFT, UNDEFINED);
                case TRUE:      return make_tuple(10, RIGHT, TRUE);
                case FALSE:     return make_tuple(10, RIGHT, FALSE);
            }
        case 11: // right hand side - this should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, LEFT, UNDEFINED);
                case TRUE:      return make_tuple(12, LEFT, UNDEFINED);
                case FALSE:     return make_tuple(100, LEFT, UNDEFINED);
            }
        case 12: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(13, LEFT, UNDEFINED);
                case FALSE:     return make_tuple(23, LEFT, UNDEFINED);
            }
        case 13: // look for left hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(14, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(13, LEFT, TRUE);
                case FALSE:     return make_tuple(13, LEFT, FALSE);
            }
        case 14: // left hand side - should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(0, RIGHT, UNDEFINED);
                case FALSE:     return make_tuple(100, RIGHT, UNDEFINED);
            }

        case 20: // look for right hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(21, LEFT, UNDEFINED);
                case TRUE:      return make_tuple(20, RIGHT, TRUE);
                case FALSE:     return make_tuple(20, RIGHT, FALSE);
            }
        case 21: // right hand side - this should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, LEFT, UNDEFINED);
                case TRUE:      return make_tuple(100, LEFT, UNDEFINED);
                case FALSE:     return make_tuple(22, LEFT, UNDEFINED);
            }
        case 22: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(13, LEFT, UNDEFINED);
                case FALSE:     return make_tuple(23, LEFT, UNDEFINED);
            }
        case 23: // look for left hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(24, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(23, LEFT, TRUE);
                case FALSE:     return make_tuple(23, LEFT, FALSE);
            }
        case 24: // left hand side - should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return make_tuple(200, RIGHT, UNDEFINED);
                case TRUE:      return make_tuple(100, RIGHT, UNDEFINED);
                case FALSE:     return make_tuple(0, RIGHT, UNDEFINED);
            }
        default:
            return make_tuple(100, RIGHT, UNDEFINED);
    }
}

bool reject(int state) {
    return state == 100;
}

bool accept(int state) {
    return state == 200;
}

int getNextPosition(int position, Direction direction) {
    if (direction == LEFT) {
        return position - 1;
    } else {
        return position + 1;
    }
}

bool runMachine(function<Symbol (int)> band, int position, int state) {
    auto transition = transitionTable(state, band(position));
    int nextState = get<0>(transition);
    Direction direction = get<1>(transition);
    Symbol write = get<2>(transition);
    int nextPosition = getNextPosition(position, direction);

    if (reject(nextState)) {
        return false;
    }
    if (accept(nextState)) {
        return true;
    }

    return runMachine(setOnBand(position, write, band), nextPosition, nextState);
}

bool test(int input) {
    auto band = inputToBand(input);

    return runMachine(band, 0, 0);
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