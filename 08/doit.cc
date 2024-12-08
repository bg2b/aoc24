// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

coord operator-(coord const &c1, coord const &c2) {
  return {c1.first - c2.first, c1.second - c2.second};
}

coord operator*(int a, coord const &c) { return {a * c.first, a * c.second}; }

struct antennas {
  // Grid size
  int w{0};
  int h{0};
  // Locations grouped by type
  map<char, vector<coord>> locations;

  // Construct from cin
  antennas();

  // Is a coordinate in the grid?
  bool in_bounds(coord const &c) const {
    return 0 <= c.first && c.first < w && 0 <= c.second && c.second < h;
  }

  // How many unique antinodes, possibly with harmonics?
  int num_antinodes(bool harmonics) const;
};

antennas::antennas() {
  string line;
  while (getline(cin, line)) {
    int len = line.length();
    w = max(w, len);
    for (int i = 0; i < len; ++i)
      if (line[i] != '.')
        locations[line[i]].emplace_back(i, h);
    ++h;
  }
}

int antennas::num_antinodes(bool harmonics) const {
  set<coord> nodes;
  for (auto const &[type, locs] : locations)
    for (auto const &l1 : locs)
      for (auto const &l2 : locs)
        if (l1 != l2) {
          auto delta = l1 - l2;
          for (int i = (harmonics ? 0 : 1); ; ++i) {
            auto c = l1 + i * delta;
            if (in_bounds(c))
              nodes.insert(c);
            if (!harmonics || !in_bounds(c))
              break;
          }
        }
  return nodes.size();
}

void part1() { cout << antennas().num_antinodes(false) << '\n'; }
void part2() { cout << antennas().num_antinodes(true) << '\n'; }

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
