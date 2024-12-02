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

// Trying to check safety with the dampener without making a dampened
// copy of the report is subtle.  Given the report sizes, I think it's
// better to just do the explicit dampening.

struct report {
  vector<int> levels;

  // Construct from input line
  report(string const &line);
  // Apply dampener at the given position
  report(report const &r, size_t to_dampen);

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

report::report(report const &r, size_t to_dampen) : levels(r.levels) {
  assert(to_dampen < levels.size());
  levels.erase(levels.begin() + to_dampen);
}

bool
report::is_safe(bool with_dampener, int dir) const {
  auto safe = [](int l1, int l2) { return l1 < l2 && l2 <= l1 + 3; };
  bool ok = true;
  for (size_t i = 0; ok && i + 1 < levels.size(); ++i)
    if (!safe(levels[i] * dir, levels[i + 1] * dir))
      ok = false;
  if (!ok && with_dampener)
    for (size_t to_dampen = 0; to_dampen < levels.size(); ++to_dampen)
      if (report(*this, to_dampen).is_safe(false))
        return true;
  return ok;
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
