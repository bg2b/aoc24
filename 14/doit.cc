// -*- C++ -*-
// Use -O2 if you're a bit impatient about part 2 (~1.5 seconds with just -g)
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

coord operator*(int a, coord const &c) {
  return {a * c.first, a * c.second};
}

struct lobby {
  coord const size;
  struct robot {
    coord start;
    coord v;
  };
  vector<robot> robots;

  // Construct from cin and dimensions
  lobby(coord const &size_);

  // Location of the given robot at the given time?
  coord at(robot const &r, int t) const;

  // Part 1
  int safety_factor(int t) const;
  // For part 2, recognize a picture by assuming there will be a
  // reasonable-length group of robots lined up.  Single stepping
  // might be a bit faster, but I'm just recomputing at time t from
  // scratch.
  bool has_long_run(int t, size_t threshold) const;
};

lobby::lobby(coord const &size_) : size(size_) {
  string line;
  while (getline(cin, line)) {
    stringstream ss(line);
    auto get = [&](char what) {
      char c, eq, comma;
      int x, y;
      ss >> c >> eq >> x >> comma >> y;
      assert(c == what);
      return coord{x, y};
    };
    auto p = get('p');
    auto v = get('v');
    robots.push_back({p, v});
  }
}

coord lobby::at(robot const &r, int t) const {
  auto c = r.start + t * r.v;
  auto pos_mod = [](int &a, int sz) {
    a %= sz;
    if (a < 0)
      a += sz;
  };
  pos_mod(c.first, size.first);
  pos_mod(c.second, size.second);
  return c;
}

int lobby::safety_factor(int t) const {
  int quads[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (auto const &r : robots) {
    auto c = at(r, t);
    auto which = [](int loc, int sz) {
      sz /= 2;
      return loc < sz ? 0 : loc > sz ? 2 : 1;
    };
    ++quads[which(c.first, size.first)][which(c.second, size.second)];
  }
  return quads[0][0] * quads[0][2] * quads[2][0] * quads[2][2];
}

bool lobby::has_long_run(int t, size_t threshold) const {
  vector<coord> cs;
  cs.reserve(robots.size());
  for (auto const &r : robots)
    cs.push_back(at(r, t));
  sort(cs.begin(), cs.end());
  size_t run = 0;
  for (size_t i = 0; i + 1 < cs.size(); ++i)
    if (cs[i] + coord{0, 1} == cs[i + 1]) {
      ++run;
      if (run >= threshold)
        return true;
    } else
      run = 0;
  return false;
}

void part1() {
  // for input1, cout << lobby({11, 7}).safety_factor(100) << '\n';
  cout << lobby({101, 103}).safety_factor(100) << '\n';
}

void part2() {
  lobby l({101, 103});
  int t = 0;
  while (!l.has_long_run(t, 10))
    ++t;
  cout << t << '\n';
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
