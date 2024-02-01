#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define CASES (100000)
#define MAX_VALUE (0xffff)

// depends on max value
#define MAX_BAND_SIZE (16 + 1)

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

class StateMachine {
protected:
    int state;

    virtual int getInitialState() = 0;
    virtual Transition getNext(int state, Symbol symbolRead) = 0;
    virtual bool reject(int state) = 0;
    virtual bool accept(int state) = 0;

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

class PalindromeRecognizer : public StateMachine {
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
    StateMachine* stateMachine;

public:
    TuringMachine(StateMachine* stateMachine, Band* band) {
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