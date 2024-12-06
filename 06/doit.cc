// -*- C++ -*-
// Optimization is helpful for part 2
// g++ -std=c++20 -Wall -g -O2 -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <cassert>

using namespace std;

// This could use something better for part 2, which is about 0.1
// seconds even with optimization.  Perhaps some more explicit graph
// representation so that the loop checking can make larger steps?  Or
// maybe there's some unexploited structure in the possible loops?

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return coord{c1.first + c2.first, c1.second + c2.second};
}

// Directions for guard movement, clockwise from up = {0, -1}
coord dirs[4] = {{0, -1}, {+1, 0}, {0, +1}, {-1, 0}};

struct lab {
  vector<string> grid;
  coord guard_post;

  // Construct from cin
  lab();

  // Width, height
  int w() const { return grid.front().length(); }
  int h() const { return grid.size(); }
  // Access, returns ' ' for out-of-grid
  char at(coord const &c) const;

  // For faster state checking, I'll use a bit vector of size
  // 4*w()*h() instead of the usual set<coord>...
  // Index of a state into the visited vector
  int state_index(coord const &c, int facing) const {
    return (c.first * h() + c.second) * 4 + facing;
  }
  // Simulate the guard (possibly with an obstacle), return whether
  // the guard exited the grid and the visited vector.  Default is no
  // obstacle.
  pair<bool, vector<bool>> guard_path(coord const &obstacle = {-2, 0}) const;

  // Do something for every (unobstructed) visited position, count
  // the number of positions satisfying fn
  int for_visited(function<bool(coord const &)> const &fn) const;
};

lab::lab() {
  string line;
  while (getline(cin, line)) {
    if (auto pos = line.find('^'); pos != string::npos)
      guard_post = coord{pos, grid.size()};
    grid.push_back(line);
    assert(grid.back().length() == grid.front().length());
  }
}

char lab::at(coord const &c) const {
  if (c.first < 0 || c.second < 0 || c.first >= w() || c.second >= h())
    return ' ';
  return grid[c.second][c.first];
}

pair<bool, vector<bool>> lab::guard_path(coord const &obstacle) const {
  assert(obstacle != guard_post);
  vector<bool> visited(w() * h() * 4, false);
  int facing = 0;
  for (auto pos = guard_post; at(pos) != ' '; ) {
    if (visited[state_index(pos, facing)])
      // Loop detected
      return {false, visited};
    visited[state_index(pos, facing)] = true;
    auto dir = dirs[facing];
    if (at(pos + dir) == '#' || pos + dir == obstacle)
      // Clockwise turn
      facing = (facing + 1) % 4;
    else
      // Step
      pos = pos + dir;
  }
  // Exited
  return {true, visited};
}

int lab::for_visited(function<bool(coord const &)> const &fn) const {
  auto [exited, visited] = guard_path();
  assert(exited);
  int result = 0;
  for (int x = 0; x < w(); ++x)
    for (int y = 0; y < h(); ++y) {
      coord pos{x, y};
      for (int facing = 0; facing < 4; ++facing)
        if (visited[state_index(pos, facing)] && fn(pos)) {
          ++result;
          break;
        }
    }
  return result;
}

void part1() {
  cout << lab().for_visited([](coord const &) { return true; }) << '\n';
}

void part2() {
  lab l;
  cout << l.for_visited([&](coord const &pos) {
    if (pos == l.guard_post)
      // The starting point is not valid for an obstacle
      return false;
    // Any other visited position is a candidate for an obstacle that
    // could block the guard
    return !l.guard_path(pos).first;
  }) << '\n';
}

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
