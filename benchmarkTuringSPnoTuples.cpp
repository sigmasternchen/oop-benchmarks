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

inline auto stateTransitionTable(int state, Symbol symbolRead) {
    switch (state) {
        case 0: // init; look for first symbol
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 10;
                case FALSE:     return 20;
            }
        case 10: // look for right hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return 11;
                case TRUE:      return 10;
                case FALSE:     return 10;
            }
        case 11: // right hand side - this should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 12;
                case FALSE:     return 100;
            }
        case 12: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 13;
                case FALSE:     return 23;
            }
        case 13: // look for left hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return 14;
                case TRUE:      return 13;
                case FALSE:     return 13;
            }
        case 14: // left hand side - should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 0;
                case FALSE:     return 100;
            }

        case 20: // look for right hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return 21;
                case TRUE:      return 20;
                case FALSE:     return 20;
            }
        case 21: // right hand side - this should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 100;
                case FALSE:     return 22;
            }
        case 22: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 13;
                case FALSE:     return 23;
            }
        case 23: // look for left hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return 24;
                case TRUE:      return 23;
                case FALSE:     return 23;
            }
        case 24: // left hand side - should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return 200;
                case TRUE:      return 100;
                case FALSE:     return 0;
            }
        default:
            return 100;
    }
}


inline auto directionTransitionTable(int state, Symbol symbolRead) {
    switch (state) {
        case 0: // init; look for first symbol
            return RIGHT;
        case 10: // look for right hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return LEFT;
                case TRUE:      return RIGHT;
                case FALSE:     return RIGHT;
            }
        case 11: // right hand side - this should be TRUE
            return LEFT;
        case 12: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return RIGHT;
                case TRUE:      return LEFT;
                case FALSE:     return LEFT;
            }
        case 13: // look for left hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return RIGHT;
                case TRUE:      return LEFT;
                case FALSE:     return LEFT;
            }
        case 14: // left hand side - should be TRUE
            return RIGHT;

        case 20: // look for right hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return LEFT;
                case TRUE:      return RIGHT;
                case FALSE:     return RIGHT;
            }
        case 21: // right hand side - this should be FALSE
            return LEFT;
        case 22: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return RIGHT;
                case TRUE:      return LEFT;
                case FALSE:     return LEFT;
            }
        case 23: // look for left hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return RIGHT;
                case TRUE:      return LEFT;
                case FALSE:     return LEFT;
            }
        case 24: // left hand side - should be FALSE
            return RIGHT;
        default:
            return RIGHT;
    }
}


inline auto writeTransitionTable(int state, Symbol symbolRead) {
    switch (state) {
        case 0: // init; look for first symbol
            return UNDEFINED;
        case 10: // look for right hand side - we need TRUE
            return symbolRead;
        case 11: // right hand side - this should be TRUE
            return UNDEFINED;
        case 12: // right hand side - new decision
            return UNDEFINED;
        case 13: // look for left hand side - we need TRUE
            return symbolRead;
        case 14: // left hand side - should be TRUE
            return UNDEFINED;

        case 20: // look for right hand side - we need FALSE
            return symbolRead;
        case 21: // right hand side - this should be FALSE
            return UNDEFINED;
        case 22: // right hand side - new decision
            return UNDEFINED;
        case 23: // look for left hand side - we need FALSE
            return symbolRead;
        case 24: // left hand side - should be FALSE
            return UNDEFINED;
        default:
            return UNDEFINED;
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

void initBand(Symbol band[MAX_BAND_SIZE], int input) {
    int numberOfDigits = input == 0 ? 1 : static_cast<int>(floor(log2(input)) + 1);

    int i = 0;
    for (; i < numberOfDigits; i++) {
        band[i] = (input & (1 << (numberOfDigits - i - 1))) ? TRUE : FALSE;
    }
    for (; i < MAX_BAND_SIZE; i++) {
        band[i] = UNDEFINED;
    }
}

Symbol getFromBand(Symbol band[MAX_BAND_SIZE], int position) {
    if (position < 0 || position >= MAX_BAND_SIZE) {
        return UNDEFINED;
    } else {
        return band[position];
    }
}

void setOnBand(Symbol band[MAX_BAND_SIZE], int position, Symbol symbol) {
    if (!(position < 0 || position >= MAX_BAND_SIZE)) {
        band[position] = symbol;
    }
}

bool abort(int state) {
    return reject(state) || accept(state);
}

bool runMachine(Symbol band[MAX_BAND_SIZE]) {
    int state = 0;
    int position = 0;

    while(!abort(state)) {
        Symbol read = getFromBand(band, position);
        setOnBand(band, position, writeTransitionTable(state, read));
        position = getNextPosition(position, directionTransitionTable(state, read));
        state = stateTransitionTable(state, read);
    }

    return accept(state);
}


bool test(int input) {
    Symbol band[MAX_BAND_SIZE];
    initBand(band, input);

    return runMachine(band);
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