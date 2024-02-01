#!/bin/sh

run() {
  test="$1"
  optimization="$2"

  clang++ -std=c++14 -O"$optimization" "benchmarkTuring$test.cpp"
  echo "$test $optimization: "
  ./a.out
}

runOpt() {
  run "$1" 0
  run "$1" 3
}

runOpt "OOP"
#runOpt "FP"
runOpt "SP"
runOpt "SPnoTuples"