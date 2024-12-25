// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cassert>

using namespace std;

int const lock_size = 5;
using heights = array<int, lock_size>;

bool fits(heights const &key, heights const &lock) {
  for (size_t i = 0; i < lock_size; ++i)
    if (lock[i] + key[i] > lock_size)
      return false;
  return true;
}

struct north_pole {
  vector<heights> locks;
  vector<heights> keys;

  // Construct from cin
  north_pole();

  // Count the number of key/lock pairs that could fit
  int num_fits() const;
};

north_pole::north_pole() {
  heights zeros;
  zeros.fill(0);
  string row;
  while (cin >> row) {
    auto is_lock = row.front() == '#';
    assert(row == string(lock_size, row.front()));
    auto &which = is_lock ? locks : keys;
    which.push_back(zeros);
    auto &hts = which.back();
    for (int _ = 0; _ < lock_size; ++_) {
      cin >> row;
      for (int i = 0; i < lock_size; ++i)
        hts[i] += row[i] == '#' ? 1 : 0;
    }
    // Skip last filled row
    cin >> row;
    assert(row == string(lock_size, is_lock ? '.' : '#'));
  }
}

int north_pole::num_fits() const {
  int result = 0;
  for (auto const &lock : locks)
    for (auto const &key : keys)
      if (fits(key, lock))
        ++result;
  return result;
}

void part1() { cout << north_pole().num_fits() << '\n'; }
void part2() { cout << "Deliver The Chronicle\n"; }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
