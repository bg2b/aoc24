// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

// I'm assuming this is big enough to avoid any overflows...
using num = unsigned long long;

// This is using brute force, not overly great but kind of acceptable

struct equation {
  num wanted;
  vector<num> values;

  // Construct from input line
  equation(string const &line);

  // Is there a way to make the equation true using +, *, and maybe
  // concatenation?
  bool possibly_true(bool with_concat) const;
};

equation::equation(string const &line) {
  stringstream ss(line);
  char colon;
  ss >> wanted >> colon;
  assert(ss && colon == ':');
  num n;
  while (ss >> n)
    values.push_back(n);
  assert(!values.empty());
}

inline num concat(num v1, num v2) {
  for (num v = v2; v > 0; v /= 10)
    v1 *= 10;
  return v1 + v2;
}

bool equation::possibly_true(bool with_concat) const {
  int const num_ops = with_concat ? 3 : 2;
  int num_ways = 1;
  for (size_t _ = 0; _ < values.size() - 1; ++_)
    num_ways *= num_ops;
  for (int way = 0; way < num_ways; ++way) {
    num value = values.front();
    int op_seq = way;
    for (size_t i = 1; i < values.size() && value <= wanted; ++i) {
      switch (op_seq % num_ops) {
      case 0: value += values[i]; break;
      case 1: value *= values[i]; break;
      case 2: value = concat(value, values[i]); break;
      }
      op_seq /= num_ops;
    }
    if (value == wanted)
      return true;
  }
  return false;
}

void solve(bool with_concat) {
  num result = 0;
  string line;
  while (getline(cin, line)) {
    equation eqn(line);
    // It's actually slightly faster to try without concat first, and
    // try with only if that fails, but whatevs
    if (eqn.possibly_true(with_concat))
      result += eqn.wanted;
  }
  cout << result << '\n';
}

void part1() { solve(false); }
void part2() { solve(true); }

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
