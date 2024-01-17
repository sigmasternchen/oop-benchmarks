#!/bin/sh

run() {
  test="$1"
  optimization="$2"

  clang++ -std=c++14 -O"$optimization" "benchmark.cpp" "$1.cpp"
  echo "$test $optimization: "
  ./a.out
}

runOpt() {
  run "$1" 0
  run "$1" 3
}

runOpt "oop-with-virtual-methods"
runOpt "oop-without-virtual-methods"
runOpt "sp-with-tuples"
runOpt "sp-with-function-per-value"
runOpt "sp-with-structs"