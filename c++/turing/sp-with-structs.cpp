#include <cmath>

#include "common.h"

using namespace std;

typedef struct transition {
    int state;
    Direction direction;
    Symbol write;
} transition_t;


transition_t transitionTable(int state, Symbol symbolRead) {
    switch (state) {
        case 0: // init; look for first symbol
            switch (symbolRead) {
                case UNDEFINED: return {200, RIGHT, UNDEFINED};
                case TRUE:      return {10, RIGHT, UNDEFINED};
                case FALSE:     return {20, RIGHT, UNDEFINED};
            }
        case 10: // look for right hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return {11, LEFT, UNDEFINED};
                case TRUE:      return {10, RIGHT, TRUE};
                case FALSE:     return {10, RIGHT, FALSE};
            }
        case 11: // right hand side - this should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return {200, LEFT, UNDEFINED};
                case TRUE:      return {12, LEFT, UNDEFINED};
                case FALSE:     return {100, LEFT, UNDEFINED};
            }
        case 12: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return {200, RIGHT, UNDEFINED};
                case TRUE:      return {13, LEFT, UNDEFINED};
                case FALSE:     return {23, LEFT, UNDEFINED};
            }
        case 13: // look for left hand side - we need TRUE
            switch (symbolRead) {
                case UNDEFINED: return {14, RIGHT, UNDEFINED};
                case TRUE:      return {13, LEFT, TRUE};
                case FALSE:     return {13, LEFT, FALSE};
            }
        case 14: // left hand side - should be TRUE
            switch (symbolRead) {
                case UNDEFINED: return {200, RIGHT, UNDEFINED};
                case TRUE:      return {0, RIGHT, UNDEFINED};
                case FALSE:     return {100, RIGHT, UNDEFINED};
            }

        case 20: // look for right hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return {21, LEFT, UNDEFINED};
                case TRUE:      return {20, RIGHT, TRUE};
                case FALSE:     return {20, RIGHT, FALSE};
            }
        case 21: // right hand side - this should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return {200, LEFT, UNDEFINED};
                case TRUE:      return {100, LEFT, UNDEFINED};
                case FALSE:     return {22, LEFT, UNDEFINED};
            }
        case 22: // right hand side - new decision
            switch (symbolRead) {
                case UNDEFINED: return {200, RIGHT, UNDEFINED};
                case TRUE:      return {13, LEFT, UNDEFINED};
                case FALSE:     return {23, LEFT, UNDEFINED};
            }
        case 23: // look for left hand side - we need FALSE
            switch (symbolRead) {
                case UNDEFINED: return {24, RIGHT, UNDEFINED};
                case TRUE:      return {23, LEFT, TRUE};
                case FALSE:     return {23, LEFT, FALSE};
            }
        case 24: // left hand side - should be FALSE
            switch (symbolRead) {
                case UNDEFINED: return {200, RIGHT, UNDEFINED};
                case TRUE:      return {100, RIGHT, UNDEFINED};
                case FALSE:     return {0, RIGHT, UNDEFINED};
            }
        default:
            return {100, RIGHT, UNDEFINED};
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
        auto transition = transitionTable(state, read);
        state = transition.state;
        setOnBand(band, position, transition.write);
        position = getNextPosition(position, transition.direction);
    }

    return accept(state);
}


bool test(int input) {
    Symbol band[MAX_BAND_SIZE];
    initBand(band, input);

    return runMachine(band);
}
