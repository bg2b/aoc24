// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <optional>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

// Directions for movement
coord dirs[4] = {{+1, 0}, {0, -1}, {-1, 0}, {0, +1}};

struct memory {
  // Final coordinate
  coord exit;
  // Records what's damaged (only filled in after calling damage())
  vector<string> grid;
  // Falling byte coordinates
  vector<coord> falling;

  // Construct from exit location (bottom right corner) and cin
  memory(coord const &exit_);

  // Access
  bool in_bounds(coord const &c) const;
  char at(coord const &c) const {
    return in_bounds(c) ? grid[c.second][c.first] : '#';
  }
  char &at(coord const &c) {
    assert(in_bounds(c));
    return grid[c.second][c.first];
  }

  // Record damaged locations, return *this
  memory &damage(size_t n);
  // Return length of shorted path to exit, or nullopt if none
  optional<int> search() const;
  // Return the falling byte that blocks access to exit.  Non-const
  // since it calls damage()
  coord blocking();
};

memory::memory(coord const &exit_) : exit(exit_) {
  coord c;
  char comma;
  while (cin >> c.first >> comma >> c.second) {
    assert(comma == ',');
    falling.push_back(c);
  }
}

bool memory::in_bounds(coord const &c) const {
  return (c.first >= 0 && c.first <= exit.first &&
          c.second >= 0 && c.second <= exit.second);
}

memory &memory::damage(size_t n) {
  // Clear damaged markers
  grid = vector<string>(exit.second + 1, string(exit.first + 1, '.'));
  assert(n <= falling.size());
  for (size_t i = 0; i < n; ++i)
    at(falling[i]) = '#';
  return *this;
}

optional<int> memory::search() const {
  // Location and number of steps
  using state = pair<int, coord>;
  // Queue for breadth-first search
  list<state> q;
  // Reached locations
  set<coord> visited;
  // Check a location to see if it's new, if so add to the queue
  auto visit = [&](int steps, coord const &c) {
    if (at(c) != '#' && !visited.contains(c)) {
      q.emplace_back(steps, c);
      visited.insert(c);
    }
  };
  visit(0, {0, 0});
  while (!q.empty()) {
    auto [steps, c] = q.front();
    q.pop_front();
    if (c == exit)
      return steps;
    for (auto const &dir : dirs)
      visit(steps + 1, c + dir);
  }
  return nullopt;
}

coord memory::blocking() {
  size_t start = 0;
  size_t end = falling.size();
  // Exit is reachable at start, unreachable at end
  assert(damage(start).search().has_value());
  assert(!damage(end).search().has_value());
  while (start + 1 < end) {
    auto mid = (start + end) / 2;
    if (damage(mid).search().has_value())
      start = mid;
    else
      end = mid;
  }
  // Once this byte falls, the exit is cut off
  return falling[start];
}

void part1() {
  // cout << memory({6, 6}).damage(12).search().value() << '\n';
  cout << memory({70, 70}).damage(1024).search().value() << '\n';
}

void part2() {
  // auto c = memory({6, 6}).blocking();
  auto c = memory({70, 70}).blocking();
  cout << c.first << ',' << c.second << '\n';
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
