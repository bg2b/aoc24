// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

vector<string> towels;

void onsen() {
  string line;
  getline(cin, line);
  line.push_back(',');
  stringstream ss(line);
  string towel;
  while (ss >> towel) {
    assert(towel.back() == ',');
    towel.pop_back();
    towels.push_back(towel);
  }
}

// cache[pos] is the number of ways to make the portion of the pattern
// that starts at pos, or -1 if that hasn't been computed yet.  For
// matching the whole pattern, initialize cache to a vector of -1 and
// call ways(pattern, 0, cache).
size_t ways(string const &pattern, size_t pos, vector<ssize_t> &cache) {
  if (pos == pattern.length())
    return 1;
  if (cache[pos] < 0) {
    cache[pos] = 0;
    for (auto const &towel : towels)
      if (pattern.compare(pos, towel.length(), towel) == 0)
        // This towel could be used starting a pos, see about the rest
        cache[pos] += ways(pattern, pos + towel.length(), cache);
  }
  return cache[pos];
}

void solve(bool all_ways) {
  onsen();
  size_t result = 0;
  string pattern;
  while (cin >> pattern) {
    vector<ssize_t> cache(pattern.length(), -1);
    auto w = ways(pattern, 0, cache);
    result += all_ways ? w : (w > 0);
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
