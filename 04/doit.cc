// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return coord{c1.first + c2.first, c1.second + c2.second};
}

struct word_search {
  vector<string> grid;

  // Construct from cin
  word_search();

  // Width, height
  int w() const { return grid.front().length(); }
  int h() const { return grid.size(); }
  // Access, returns '.' for out-of-grid
  char at(coord const &c) const;

  // Does s occur starting at pos and continuing in dir?
  bool match(string const &s, coord pos, coord const &dir) const;

  // Number of XMAS's at pos
  int xmas(coord const &pos) const;
  // Number of crossing MAS's at pos (0 or 1 only)
  int x_mas(coord const &pos) const;
};

word_search::word_search() {
  string line;
  while (getline(cin, line)) {
    grid.push_back(line);
    assert(grid.back().length() == grid.front().length());
  }
}

char word_search::at(coord const &c) const {
  if (c.first < 0 || c.second < 0 || c.first >= w() || c.second >= h())
    return '.';
  return grid[c.second][c.first];
}

bool word_search::match(string const &s, coord pos, coord const &dir) const {
  for (auto wanted : s) {
    if (wanted != at(pos))
      return false;
    pos = pos + dir;
  }
  return true;
}

int word_search::xmas(coord const &pos) const {
  int xmases = 0;
  for (int dx : {-1, 0, +1})
    for (int dy : {-1, 0, +1})
      // Direction {0, 0} will fail since pos isn't moving...
      if (match("XMAS", pos, {dx, dy}))
        ++xmases;
  return xmases;
}

int word_search::x_mas(coord const &pos) const {
  bool diag1 = (match("MAS", pos + coord{-1, -1}, {+1, +1}) ||
                match("SAM", pos + coord{-1, -1}, {+1, +1}));
  bool diag2 = (match("MAS", pos + coord{-1, +1}, {+1, -1}) ||
                match("SAM", pos + coord{-1, +1}, {+1, -1}));
  return diag1 && diag2 ? 1 : 0;
}

void solve(int (word_search::*count)(coord const &pos) const) {
  word_search ws;
  int result = 0;
  for (int x = 0; x < ws.w(); ++x)
    for (int y = 0; y < ws.h(); ++y)
      result += (ws.*count)({x, y});
  cout << result << '\n';
}

void part1() { solve(&word_search::xmas); }
void part2() { solve(&word_search::x_mas); }

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
