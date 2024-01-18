#ifndef OOP_BENCHMARK_COMMON_H
#define OOP_BENCHMARK_COMMON_H

#ifndef REPEATS
#define REPEATS (1000)
#endif

#define CASES (1000)
#define MAX_VALUE (0xffff)

#define MAX_BAND_SIZE (MAX_VALUE + 1)

enum Direction {
    LEFT, RIGHT
};

enum Symbol {
    UNDEFINED, TRUE, FALSE
};


#endif //OOP_BENCHMARK_COMMON_H
