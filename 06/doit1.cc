// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <cassert>

using namespace std;

// This uses skip tables to speed up part 2.  The idea is to do an
// initial scan in each direction and make tables that record "when at
// coordinate {x, y} and facing direction 0, the next interesting
// thing is at {x, y'}" (and similarly for the other directions).
// Then the search for a loop can jump directly between interesting
// positions.

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

coord operator-(coord const &c1, coord const &c2) {
  return {c1.first - c2.first, c1.second - c2.second};
}

bool between(int a, int b1, int b2) {
  return (b1 < a && a < b2) || (b2 < a && a < b1);
}

// Directions for guard movement, clockwise from up = {0, -1}
coord dirs[4] = {{0, -1}, {+1, 0}, {0, +1}, {-1, 0}};

struct lab {
  vector<string> grid;
  coord guard_post;
  // For part 2, skips[coord_index][facing] tells the next interesting
  // value of the changing coordinate
  vector<array<int, 4>> skips;

  // Construct from cin
  lab();

  // Width, height
  int w() const { return grid.front().length(); }
  int h() const { return grid.size(); }
  // Access, returns ' ' for out-of-grid
  char at(coord const &c) const;

  // For faster state checking, I'll use a bit vectors...
  // Index for coordinates only
  int coord_index(coord const &c) const { return c.first * h() + c.second; }
  // Index when including direction too
  int state_index(coord const &c, int facing) const {
    return 4 * coord_index(c) + facing;
  }

  // Simulate the guard until she exits, return visited positions
  vector<bool> guard_path() const;
  // Scan to compute skip tables
  void compute_skips();
  // Does placing a new obstacle cause the guard to loop?
  bool loops(coord const &obstacle) const;
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

vector<bool> lab::guard_path() const {
  vector<bool> visited(w() * h(), false);
  int facing = 0;
  for (auto pos = guard_post; at(pos) != ' '; ) {
    visited[coord_index(pos)] = true;
    auto dir = dirs[facing];
    if (at(pos + dir) == '#')
      // Clockwise turn
      facing = (facing + 1) % 4;
    else
      // Step
      pos = pos + dir;
  }
  // Exited
  return visited;
}

void lab::compute_skips() {
  skips.resize(w() * h());
  // Ugly; I was tempted to write it out as four similar copies...
  for (int facing = 0; facing < 4; ++facing) {
    int d1 = dirs[facing].first == 0 ? w() : h();
    int d2 = d1 == w() ? h() : w();
    auto x = dirs[facing].first == 0 ? &coord::first : &coord::second;
    auto y = x == &coord::first ? &coord::second : &coord::first;
    auto xy = [&](int i1, int i2) { coord c; c.*x = i1; c.*y = i2; return c; };
    for (int i1 = 0; i1 < d1; ++i1) {
      // interesting is either just off the edge of the grid or the
      // most recent obstruction
      auto interesting = xy(i1, dirs[facing].*y == -1 ? -1 : d2);
      auto c = interesting - dirs[facing];
      for (int i2 = 0; i2 < d2; ++i2) {
        if (at(c) == '#')
          interesting = c;
        skips[coord_index(c)][facing] = interesting.*y;
        c = c - dirs[facing];
      }
    }
  }
}

bool lab::loops(coord const &obstacle) const {
  if (obstacle == guard_post)
    return false;
  assert(!skips.empty());
  vector<bool> bumps(w() * h() * 4, false);
  int facing = 0;
  for (auto pos = guard_post; at(pos) != ' '; ) {
    auto interesting = pos;
    // What's the next interesting position?
    int skip_to = skips[coord_index(pos)][facing];
    if (dirs[facing].first == 0) {
      // Vertical movement
      if (obstacle.first == pos.first &&
          between(obstacle.second, pos.second, skip_to))
        // The new obstacle is in the way
        skip_to = obstacle.second;
      interesting.second = skip_to;
    } else {
      // Ditto for horizontal movement
      if (obstacle.second == pos.second &&
          between(obstacle.first, pos.first, skip_to))
        skip_to = obstacle.first;
      interesting.first = skip_to;
    }
    if (at(interesting) == '#' || interesting == obstacle) {
      // Hit an obstruction
      if (bumps[state_index(interesting, facing)])
        // Loop found
        return true;
      bumps[state_index(interesting, facing)] = true;
      // Back up and turn right
      interesting = interesting - dirs[facing];
      facing = (facing + 1) % 4;
    }
    pos = interesting;
  }
  // Exited the grid, no loop
  return false;
}

void part1() {
  auto visited = lab().guard_path();
  cout << count(visited.begin(), visited.end(), true) << '\n';
}

void part2() {
  lab l;
  l.compute_skips();
  auto visited = l.guard_path();
  int result = 0;
  for (size_t i = 0; i < visited.size(); ++i)
    if (visited[i] && l.loops({i / l.h(), i % l.h()}))
      ++result;
  cout << result << '\n';
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
