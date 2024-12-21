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
  return {c1.first + c2.first, c1.second + c2.second};
}

int manhattan(coord const &c1, coord const &c2) {
  return abs(c1.first - c2.first) + abs(c1.second - c2.second);
}

// Directions for movement
coord dirs[4] = {{+1, 0}, {0, -1}, {-1, 0}, {0, +1}};

struct racetrack {
  // Beginning and ending
  coord start;
  coord end;
  // Representation in grid
  int const wall = -2;
  int const unknown = -1;
  // Direct representation of the grid and no-cheating times,
  // converted during read.  Initialized with unknown (for spaces) or
  // wall, then the spaces are filled in with the no-cheating time
  // from start.
  vector<vector<int>> grid;
  // Number of steps along the non-cheating path
  int pico{0};

  // Construct from cin
  racetrack();

  // Access
  int w() const { return grid.front().size(); }
  int h() const { return grid.size(); }
  int at(coord const &c) const { return grid[c.second][c.first]; }
  int &at(coord const &c) { return grid[c.second][c.first]; }

  // Number of cheats of at most max_pico duration that save time at
  // least threshold.  Note that taking a cheat from c1 to c2 where
  // pico[c2] - pico[c1] = t only saves time t - manhattan(c1, c2)!
  // For example, consider something like this:
  // ######
  // >....#
  // #|##.#
  // #+--.#
  // ####v#
  // The normal path is passing > and continuing to v along the dots.
  // Cheating by going through walls along the |+- path and exiting
  // into the clear just above v does not help.
  int good_cheats(int max_pico, int threshold = 100) const;
};

racetrack::racetrack() {
  string line;
  while (getline(cin, line)) {
    if (auto pos = line.find('S'); pos != string::npos) {
      start = coord{pos, grid.size()};
      line[pos] = '.';
    }
    if (auto pos = line.find('E'); pos != string::npos) {
      end = coord{pos, grid.size()};
      line[pos] = '.';
    }
    // Convert for search
    grid.push_back({});
    for (auto ch : line)
      grid.back().push_back(ch == '#' ? wall : unknown);
    assert(grid.back().size() == grid.front().size());
  }
  at(start) = pico++;
  auto c = start;
  while (c != end) {
    for (auto const &dir : dirs)
      if (auto next = c + dir; at(next) == unknown) {
        // No need to restart dir loop after a step.  Either a later
        // dir will be the next step, or I'll catch it on the next
        // pass through c != end.
        c = next;
        at(c) = pico++;
      }
  }
}

int racetrack::good_cheats(int max_pico, int threshold) const {
  vector<int> savings(pico, 0);
  for (int x1 = 0; x1 < w(); ++x1)
    for (int y1 = 0; y1 < h(); ++y1) {
      coord c1{x1, y1};
      if (auto t1 = at(c1); t1 != wall) {
        // Look for other squares on the path within a max_pico
        // manhattan radius
        for (int dx = -max_pico; dx <= max_pico; ++dx) {
          int x2 = x1 + dx;
          if (x2 < 0 || x2 >= w())
            continue;
          int maxy = max_pico - abs(dx);
          for (int dy = -maxy; dy <= maxy; ++dy) {
            int y2 = y1 + dy;
            if (y2 < 0 || y2 >= h())
              continue;
            coord c2{x2, y2};
            if (auto t2 = at(c2); t2 != wall) {
              auto saved = t2 - t1 - manhattan(c1, c2);
              if (saved > 0)
                // Found a cheat
                ++savings[saved];
            }
          }
        }
      }
    }
  int result = 0;
  for (size_t i = threshold; i < savings.size(); ++i)
    result += savings[i];
  return result;
}

void part1() { cout << racetrack().good_cheats(2) << '\n'; }
void part2() { cout << racetrack().good_cheats(20) << '\n'; }

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
