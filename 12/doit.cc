// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <functional>
#include <algorithm>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return coord{c1.first + c2.first, c1.second + c2.second};
}

coord dirs[4] = {{0, -1}, {+1, 0}, {0, +1}, {-1, 0}};

#define AT(c, stuff) (stuff)[(c).second][(c).first]

struct farm {
  vector<string> plots;

  // Construct from cin
  farm();

  // Width, height
  int w() const { return plots.front().length(); }
  int h() const { return plots.size(); }
  // Access, returning a reference.  For coordinates outside, gives a
  // reference to a dummy location holding ' '.
  char &at(coord const &c);

  // Harvest one patch.  Return is the area and the perimeter
  // sections.  Perimeter sections are indicated by the coordinates of
  // the plot that it borders and the direction from that plot.  This
  // changes plots to lowercase as it visits them, so I don't have to
  // keep separate markings.
  pair<int, vector<pair<int, coord>>> harvest(coord const &c);

  // Harvest everything, counting the perimeter according to the given
  // cost function
  int harvest(function<int(vector<pair<int, coord>> &)> const &cost_fn);
};

farm::farm() {
  string line;
  while (getline(cin, line)) {
    plots.push_back(line);
    assert(plots.back().length() == plots.front().length());
  }
}

char &farm::at(coord const &c) {
  if (c.first < 0 || c.second < 0 || c.first >= w() || c.second >= h()) {
    static char outside = ' ';
    return outside;
  }
  return plots[c.second][c.first];
}

pair<int, vector<pair<int, coord>>> farm::harvest(coord const &c) {
  auto growing = at(c);
  assert(isupper(growing));
  int area = 0;
  vector<pair<int, coord>> perimeter;
  vector<coord> searching{c};
  while (!searching.empty()) {
    auto c1 = searching.back();
    searching.pop_back();
    if (at(c1) == tolower(growing))
      // Already harvested
      continue;
    assert(at(c1) == growing);
    // Harvest this spot
    at(c1) = tolower(growing);
    ++area;
    // Check neighbors
    for (int dir = 0; dir < 4; ++dir) {
      auto c2 = c1 + dirs[dir];
      if (at(c2) == growing)
        // Part of the same region
        searching.push_back(c2);
      else if (at(c2) != tolower(growing))
        // The region is bordering something else in that direction
        perimeter.emplace_back(dir, c1);
    }
  }
  return {area, perimeter};
}

int farm::harvest(function<int(vector<pair<int, coord>> &)> const &cost_fn) {
  int result = 0;
  for (int x = 0; x < w(); ++x)
    for (int y = 0; y < h(); ++y)
      if (isupper(at({x, y}))) {
        // Harvest this patch
        auto [area, perimeter] = harvest({x, y});
        result += area * cost_fn(perimeter);
      }
  return result;
}

void part1() {
  cout << farm().harvest([](vector<pair<int, coord>> &perimeter) {
    return perimeter.size();
  }) << '\n';
}

void part2() {
  // For counting the number of sides, the idea is to sort the
  // perimeter sections by direction and then by coordinate.  Same
  // direction and same first coordinate and second coordinates
  // differing by 1 indicate sections on the same side.
  auto num_sides = [](vector<pair<int, coord>> &perimeter) {
    int sides = perimeter.size();
    for (int _ = 0; _ < 2; ++_) {
      // Organize by direction, then by sorted coordinates
      sort(perimeter.begin(), perimeter.end());
      for (size_t i = 0; i + 1 < perimeter.size(); ++i) {
        auto const &[d1, c1] = perimeter[i];
        auto const &[d2, c2] = perimeter[i + 1];
        if (d1 == d2 && c1.first == c2.first && c1.second + 1 == c2.second)
          // Adjoining sections on the same side
          --sides;
      }
      // Transpose coordinates for sides in other direction
      for (auto &[_, c] : perimeter)
        c = {c.second, c.first};
    }
    return sides;
  };
  cout << farm().harvest(num_sides) << '\n';
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
