#!/bin/sh

run() {
  test="$1"
  optimization="$2"
  repeats="$3"

  clang++ -std=c++14 -DREPEATS="$repeats" -O"$optimization" "benchmark.cpp" "$1.cpp"
  echo "$test $optimization (n = $repeats): "
  ./a.out
}

runOpt() {
  run "$1" 0 1000
  run "$1" 3 20000
}

runOpt "oop-with-virtual-methods"
runOpt "oop-without-virtual-methods"
runOpt "sp-with-tuples"
runOpt "sp-with-function-per-value"
runOpt "sp-with-structs"