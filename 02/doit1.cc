// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

struct report {
  vector<int> levels;

  // Construct from input line
  report(string const &line);

  // Is the report safe for a given direction, possibly using the
  // dampener?
  bool is_safe(bool with_dampener, int dir) const;
  bool is_safe(bool with_dampener) const {
    return is_safe(with_dampener, +1) || is_safe(with_dampener, -1);
  }
};

report::report(string const &line) {
  stringstream ss(line);
  int l;
  while (ss >> l)
    levels.push_back(l);
}

bool
report::is_safe(bool with_dampener, int dir) const {
  auto safe = [&](size_t i1, size_t i2) {
    auto l1 = levels[i1] * dir;
    auto l2 = levels[i2] * dir;
    return l1 < l2 && l2 <= l1 + 3;
  };
  // Keep track of locations i where (i, i+1) is unsafe
  vector<size_t> violations;
  auto n = levels.size();
  for (size_t i = 0; i + 1 < n; ++i)
    if (!safe(i, i + 1))
      violations.push_back(i);
  if (!violations.empty() && violations.size() < 3 && with_dampener) {
    // There are not too many problems to possibly fix...
    if (violations.size() == 1) {
      auto v = violations[0];
      // A violation at either end gets fixed automatically.
      // Otherwise consider deleting each level in the pair.
      return v == 0 || v == n - 2 || safe(v - 1, v + 1) || safe(v, v + 2);
    }
    if (violations[0] + 1 == violations[1])
      // For two consecutive violations (three consecutive levels), only
      // deleting the middle level might fix things
      return safe(violations[0], violations[0] + 2);
  }
  return violations.empty();
}

vector<report> read() {
  vector<report> result;
  string line;
  while (getline(cin, line))
    result.emplace_back(line);
  return result;
}

void solve(bool with_dampener) {
  int result = 0;
  for (auto const &r : read())
    if (r.is_safe(with_dampener))
      ++result;
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
