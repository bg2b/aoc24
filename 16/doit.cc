// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <optional>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return coord{c1.first + c2.first, c1.second + c2.second};
}

coord operator-(coord const &c1, coord const &c2) {
  return coord{c1.first - c2.first, c1.second - c2.second};
}

int manhattan(coord const &c1, coord const &c2) {
  return abs(c1.first - c2.first) + abs(c1.second - c2.second);
}

// Directions for movement, east first, ccw
coord dirs[4] = {{+1, 0}, {0, -1}, {-1, 0}, {0, +1}};

// Search state is a movement direction and a position
using state = pair<int, coord>;

struct maze {
  coord start;
  coord end;
  vector<string> grid;

  // Construct from cin
  maze();

  // Access
  char at(coord const &c) const { return grid[c.second][c.first]; }

  // Dijklmnopqrstra-like search, return length of fastest path to the
  // end and a map from search states to the fastest way to get to
  // that state.  If all_paths is false, stops as soon as one fastest
  // path is found.  Otherwise keeps searching until all it exhausts
  // all possibilities.
  pair<int, map<state, int>> search(bool all_paths) const;

  // For part 2, return the number of spots for observing some fastest
  // path
  int good_spots() const;
};

maze::maze() {
  string line;
  while (getline(cin, line)) {
    if (auto pos = line.find('S'); pos != string::npos)
      start = coord{pos, grid.size()};
    if (auto pos = line.find('E'); pos != string::npos)
      end = coord{pos, grid.size()};
    grid.push_back(line);
    assert(grid.back().length() == grid.front().length());
  }
}

pair<int, map<state, int>> maze::search(bool all_paths) const {
  // Score to this point and state
  using to_search = pair<int, state>;
  // Comparison for priority queue
  auto comp = [&](to_search const &ts1, to_search const &ts2) {
    auto const &[score1, s1] = ts1;
    auto const &[score2, s2] = ts2;
    if (score1 != score2)
      // Negate because of C++'s annoying max queues
      return !(score1 < score2);
    // Otherwise go by some vague bound.  Could be a bit smarter about
    // required turns...
    auto d1 = manhattan(s1.second, end);
    auto d2 = manhattan(s2.second, end);
    return !(d1 < d2);
  };
  priority_queue<to_search, vector<to_search>, decltype(comp)> q(comp);
  q.push({0, {0, start}});
  // State => shortest distance found so far
  map<state, int> best;
  // Fastest path to end
  optional<int> fastest;
  while (!q.empty()) {
    auto [score, s] = q.top();
    q.pop();
    auto [facing, c] = s;
    if (auto p = best.find(s); p != best.end() && p->second <= score)
      // This is no better than other paths to this state
      continue;
    best[s] = score;
    if (c == end) {
      // Reached the end, no point in going further
      fastest = min(score, fastest.value_or(score));
      if (!all_paths)
        // Just want one fast path
        break;
      continue;
    }
    // Consider turns
    for (auto turn : {(facing + 1) % 4, (facing + 3) % 4}) {
      if (at(c + dirs[turn]) != '#')
        q.push({score + 1000, {turn, c}});
    }
    // Try forward
    if (auto c1 = c + dirs[facing]; at(c1) != '#')
      q.push({score + 1, {facing, c1}});
  }
  assert(fastest.has_value());
  return {*fastest, best};
}

int maze::good_spots() const {
  auto [fastest, best] = search(true);
  // States in best that are along fastest paths
  set<state> bestest;
  // Does state s have a specific score?
  auto has_score = [&](state const &s, int score) {
    auto p = best.find(s);
    return p != best.end() && p->second == score;
  };
  for (int end_facing = 0; end_facing < 4; ++end_facing)
    if (has_score({end_facing, end}, fastest)) {
      // Coming into the end with this direction is fastest.  Work
      // backwards by following the scores in best.
      vector<state> good{{end_facing, end}};
      while (!good.empty()) {
        auto [facing, c] = good.back();
        good.pop_back();
        if (bestest.contains({facing, c}))
          // Already marked this
          continue;
        bestest.insert({facing, c});
        auto maybe_good = [&](state const &s, int wanted_score) {
          if (has_score(s, wanted_score))
            good.push_back(s);
        };
        int score = best.at({facing, c});
        // See if unturning or backing up is also bestest
        maybe_good({(facing + 3) % 4, c}, score - 1000);
        maybe_good({(facing + 1) % 4, c}, score - 1000);
        maybe_good({facing, c - dirs[facing]}, score - 1);
      }
    }
  // Got the states, now collect just the coordinates
  set<coord> passed;
  for (auto const &[_, c] : bestest)
    passed.insert(c);
  return passed.size();
}

void part1() { cout << maze().search(false).first << '\n'; }
void part2() { cout << maze().good_spots() << '\n'; }

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
