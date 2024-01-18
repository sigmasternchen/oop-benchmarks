#!/bin/sh

run() {
  test="$1"
  optimization="$2"
  repeats="$3"

  clang++ -std=c++14 -DREPEATS="$repeats" -O"$optimization" "histogram.cpp" "$1.cpp"
  echo "$test $optimization (n = $repeats): "
  ./a.out
}

run "oop-without-virtual-methods" 3 200000