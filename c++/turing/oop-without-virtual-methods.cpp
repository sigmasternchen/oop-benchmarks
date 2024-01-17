#include <stdexcept>
#include <cmath>

#include "common.h"

using namespace std;

class Transition {
public:
    int state;
    Direction direction;
    Symbol symbolToWrite;

    Transition(int state, Direction direction, Symbol symbolToWrite) {
        this->state = state;
        this->direction = direction;
        this->symbolToWrite = symbolToWrite;
    }
};

class PalindromeRecognizer {
private:
    int state;

    int getInitialState() {
        return 0;
    }
    
    Transition getNext(int state, Symbol symbolRead) {
        //cout << state << " " << (symbolRead == UNDEFINED) << " " << (symbolRead == TRUE) << endl;

        switch (state) {
            case 0: // init; look for first symbol
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(10, RIGHT, UNDEFINED);
                    case FALSE:     return Transition(20, RIGHT, UNDEFINED);
                }
            case 10: // look for right hand side - we need TRUE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(11, LEFT, UNDEFINED);
                    case TRUE:      return Transition(10, RIGHT, TRUE);
                    case FALSE:     return Transition(10, RIGHT, FALSE);
                }
            case 11: // right hand side - this should be TRUE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, LEFT, UNDEFINED);
                    case TRUE:      return Transition(12, LEFT, UNDEFINED);
                    case FALSE:     return Transition(100, LEFT, UNDEFINED);
                }
            case 12: // right hand side - new decision
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(13, LEFT, UNDEFINED);
                    case FALSE:     return Transition(23, LEFT, UNDEFINED);
                }
            case 13: // look for left hand side - we need TRUE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(14, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(13, LEFT, TRUE);
                    case FALSE:     return Transition(13, LEFT, FALSE);
                }
            case 14: // left hand side - should be TRUE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(0, RIGHT, UNDEFINED);
                    case FALSE:     return Transition(100, RIGHT, UNDEFINED);
                }

            case 20: // look for right hand side - we need FALSE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(21, LEFT, UNDEFINED);
                    case TRUE:      return Transition(20, RIGHT, TRUE);
                    case FALSE:     return Transition(20, RIGHT, FALSE);
                }
            case 21: // right hand side - this should be FALSE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, LEFT, UNDEFINED);
                    case TRUE:      return Transition(100, LEFT, UNDEFINED);
                    case FALSE:     return Transition(22, LEFT, UNDEFINED);
                }
            case 22: // right hand side - new decision
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(13, LEFT, UNDEFINED);
                    case FALSE:     return Transition(23, LEFT, UNDEFINED);
                }
            case 23: // look for left hand side - we need FALSE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(24, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(23, LEFT, TRUE);
                    case FALSE:     return Transition(23, LEFT, FALSE);
                }
            case 24: // left hand side - should be FALSE
                switch (symbolRead) {
                    case UNDEFINED: return Transition(200, RIGHT, UNDEFINED);
                    case TRUE:      return Transition(100, RIGHT, UNDEFINED);
                    case FALSE:     return Transition(0, RIGHT, UNDEFINED);
                }
            default:
                return Transition(100, RIGHT, UNDEFINED);
        }
    }
    
    bool reject(int state) {
        return state == 100;
    }
    
    bool accept(int state) {
        return state == 200;
    }

public:
    void reset() {
        state = getInitialState();
    }

    Transition getNext(Symbol symbolRead) {
        auto transition = getNext(state, symbolRead);
        state = transition.state;

        return transition;
    }

    bool reject() {
        return reject(state);
    }

    bool accept() {
        return accept(state);
    }
};

class Band {
private:
    Symbol fields[MAX_BAND_SIZE];
    int position;

    void set(Symbol symbol) {
        if (position < 0 || position >= MAX_BAND_SIZE) {
            return;
        } else {
            fields[position] = symbol;
        }
    }

    void move(Direction direction) {
        switch (direction) {
            case LEFT:
                position--;
                break;
            case RIGHT:
                position++;
                break;
        }
    }

public:
    Band(int input) {
        int numberOfDigits = input == 0 ? 1 : static_cast<int>(floor(log2(input)) + 1);

        for (int i = 0; i < numberOfDigits; i++) {
            fields[i] = (input & (1 << (numberOfDigits - i - 1))) ? TRUE : FALSE;
        }
        for (int i = numberOfDigits; i < MAX_BAND_SIZE; i++) {
            fields[i] = UNDEFINED;
        }

        this->position = 0;
    }

    Symbol get() {
        //cout << position << " ";
        if (position < 0 || position >= MAX_BAND_SIZE) {
            return UNDEFINED;
        } else {
            return fields[position];
        }
    }

    void apply(Transition transition) {
        set(transition.symbolToWrite);
        move(transition.direction);
    }
};

class TuringMachine {
private:
    Band* band;
    PalindromeRecognizer* stateMachine;

public:
    TuringMachine(PalindromeRecognizer* stateMachine, Band* band) {
        this->stateMachine = stateMachine;
        this->stateMachine->reset();
        this->band = band;
    }

    bool run() {
        while (true) {
            if (stateMachine->reject()) {
                return false;
            }
            if (stateMachine->accept()) {
                return true;
            }

            band->apply(
                    stateMachine->getNext(band->get())
            );
        }
    }
};

bool test(int input) {
    Band band(input);
    PalindromeRecognizer program;

    TuringMachine machine(&program, &band);
    return machine.run();
}