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

int manhattan(coord const &c1, coord const &c2) {
  return abs(c1.first - c2.first) + abs(c1.second - c2.second);
}

// Directions for movement
coord dirs[4] = {{+1, 0}, {0, -1}, {-1, 0}, {0, +1}};

struct racetrack {
  // Beginning and ending
  coord start;
  coord end;
  // Direct representation of the grid
  vector<string> grid;
  // No-cheating time from start
  map<coord, int> pico;

  // Construct from cin
  racetrack();

  // Access
  char at(coord const &c) const { return grid[c.second][c.first]; }

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
    grid.push_back(line);
    assert(grid.back().length() == grid.front().length());
  }
  pico.emplace(start, 0);
  auto c = start;
  while (c != end) {
    for (auto const &dir : dirs)
      if (auto next = c + dir; !pico.contains(next) && at(next) == '.') {
        // No need to restart dir loop after a step.  Either a later
        // dir will be the next step, or I'll catch it on the next
        // pass through c != end.
        c = next;
        pico.emplace(c, pico.size());
      }
  }
}

int racetrack::good_cheats(int max_pico, int threshold) const {
  vector<int> savings(pico.size(), 0);
  for (auto const &[c1, t1] : pico)
    for (int dx = -max_pico; dx <= max_pico; ++dx) {
      // Search the vertical slice at x == c1.first + dx
      int maxy = max_pico - abs(dx);
      auto imin = pico.lower_bound({c1.first + dx, c1.second - maxy});
      auto imax = pico.upper_bound({c1.first + dx, c1.second + maxy});
      for (auto i = imin; i != imax; ++i) {
        auto const &[c2, t2] = *i;
        auto saved = t2 - t1 - manhattan(c1, c2);
        if (saved > 0)
          ++savings[saved];
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
