// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

// I've split pushing into two phases (checking and committing) since
// that's necessary for vertical pushes in part 2.

struct warehouse {
  coord robot;
  vector<string> grid;

  // Construct from cin (stops after blank line)
  warehouse();

  // Apply holiday waistline effect
  void widen();

  // Accessing and returning references for modifying
  char at(coord const &c) const { return grid[c.second][c.first]; }
  char &at(coord const &c) { return grid[c.second][c.first]; }

  // Can the thing at c be pushed in the given direction?  Spaces can
  // always be pushed, walls never can, and for boxes it depends on
  // what's adjacent to them.
  bool can_push(coord const &c, coord const &dir) const;
  // Once a push has been verified to work, actually do it
  void push(coord const &c, coord const &dir);

  // Make the robot try to push in the given direction
  void push(char which_dir);

  // Return the sum of the GPS coordinates of all boxes
  int gps() const;
};

warehouse::warehouse() {
  string line;
  while (getline(cin, line) && !line.empty()) {
    if (auto pos = line.find('@'); pos != string::npos) {
      robot = {pos, grid.size()};
      line[pos] = '.';
    }
    grid.push_back(line);
    assert(grid.front().length() == grid.back().length());
  }
}

void warehouse::widen() {
  for (auto &line : grid) {
    string widened;
    for (auto c : line)
      widened += c == 'O' ? "[]" : string(2, c);
    line = widened;
  }
  robot.first *= 2;
}

bool warehouse::can_push(coord const &c, coord const &dir) const {
  if (at(c) == '.')
    return true;
  if (at(c) == '#')
    return false;
  if (at(c) == 'O' || dir.second == 0)
    // Small boxes or horizontal pushes; can push if the adjacent
    // stuff can be pushed
    return can_push(c + dir, dir);
  // Vertical push of a wide box
  auto other_part = c + (at(c) == '[' ? coord{+1, 0} : coord{-1, 0});
  // Can push if both parts can be pushed
  return can_push(c + dir, dir) && can_push(other_part + dir, dir);
}

void warehouse::push(coord const &c, coord const &dir) {
  char here = at(c);
  assert(here != '#');
  if (here == '.')
    return;
  // Push small boxes, wide boxes horizontally, or the first part of a
  // wide box vertically
  push(c + dir, dir);
  at(c + dir) = here;
  at(c) = '.';
  if ((here == '[' || here == ']') && dir.first == 0) {
    // Vertical push of the other part of a wide box
    auto other_part = c + coord{(here == '[' ? +1 : -1), 0};
    push(other_part + dir, dir);
    at(other_part + dir) = here == '[' ? ']' : '[';
    at(other_part) = '.';
  }
}

void warehouse::push(char which_dir) {
  coord dir;
  switch (which_dir) {
  case '^': dir = {0, -1}; break;
  case 'v': dir = {0, +1}; break;
  case '<': dir = {-1, 0}; break;
  default: assert(which_dir == '>'); dir = {+1, 0}; break;
  }
  if (can_push(robot + dir, dir)) {
    push(robot + dir, dir);
    robot = robot + dir;
  }
}

int warehouse::gps() const {
  int result = 0;
  for (size_t y = 0; y < grid.size(); ++y)
    for (size_t x = 0; x < grid[y].length(); ++x)
      if (at({x, y}) == 'O' || at({x, y}) == '[')
        result += 100 * y + x;
  return result;
}

void solve(bool wide) {
  warehouse w;
  if (wide)
    w.widen();
  char which_dir;
  while (cin >> which_dir)
    w.push(which_dir);
  cout << w.gps() << '\n';
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
