// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <utility>
#include <cassert>

using namespace std;

// Templates FTW

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return coord{c1.first + c2.first, c1.second + c2.second};
}

struct topo_map {
  vector<string> heights;

  // Construct from cin
  topo_map();

  // Width, height
  int w() const { return heights.front().length(); }
  int h() const { return heights.size(); }
  // Access, returns ' ' for out-of-range
  char at(coord const &c) const;

  // Return total score or rating for all trails, depending on T
  // set<coord> for score, vector<coord> for rating
  template <class T> int trails() const;
};

topo_map::topo_map() {
  string line;
  while (getline(cin, line)) {
    heights.push_back(line);
    assert(heights.back().length() == heights.front().length());
  }
}

char topo_map::at(coord const &c) const {
  if (c.first < 0 || c.second < 0 || c.first >= w() || c.second >= h())
    return ' ';
  return heights[c.second][c.first];
}

template <class T>
int topo_map::trails() const {
  int result = 0;
  for (int x = 0; x < w(); ++x)
    for (int y = 0; y < h(); ++y) {
      if (at({x, y}) != '0')
        continue;
      T reached{{x, y}};
      for (char target = '1'; target <= '9'; ++target) {
        T next_reached;
        auto i = inserter(next_reached, next_reached.end());
        for (auto const &c : reached)
          for (auto const &dir : {coord{0, -1}, {0, +1}, {-1, 0}, {+1, 0}})
            if (at(c + dir) == target)
              i = c + dir;
        reached = next_reached;
      }
      result += reached.size();
    }
  return result;
}

void part1() { cout << topo_map().trails<set<coord>>() << '\n'; }
void part2() { cout << topo_map().trails<vector<coord>>() << '\n'; }

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
