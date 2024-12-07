// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

using num = uint64_t;

// Work backwards from the end.  Multiplication can only be used at
// stage i if the target number v is an exact multiple of the ith
// value vi.  Addition can only be used if v >= vi.  And
// concatentation (if allowed) can only be used if there exists v'
// such that v == concat(v', vi).  In each case, I can derive a new
// target number for stage i-1.  At stage 0, I have to have v == v0.

struct equation {
  num wanted;
  vector<num> values;

  // Construct from input line
  equation(string const &line);

  // Is it possible to work backwards to make v using the numbers in
  // [0, end)?
  bool search(bool with_concat, num v, size_t end) const;

  // Is there a way to make the equation true using +, *, and maybe
  // concatenation?  If so, return wanted, else 0.
  num calibration(bool with_concat) const {
    return search(with_concat, wanted, values.size()) ? wanted : 0;
  }
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

bool equation::search(bool with_concat, num v, size_t end) const {
  assert(end != 0);
  auto v1 = values[--end];
  if (end == 0)
    // Ran out of operations to do
    return v == v1;
  if (v >= v1 && search(with_concat, v - v1, end))
    // Could add
    return true;
  if (v % v1 == 0 && search(with_concat, v / v1, end))
    // Could multiply
    return true;
  if (with_concat && v >= v1) {
    v -= v1;
    auto was_concat = true;
    while (v1 > 0) {
      was_concat = was_concat && (v % 10 == 0);
      v1 /= 10;
      v /= 10;
    }
    if (was_concat)
      // Could concatenate
      return search(with_concat, v, end);
  }
  // No way to manage
  return false;
}

void solve(bool with_concat) {
  num result = 0;
  string line;
  while (getline(cin, line))
    result += equation(line).calibration(with_concat);
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
