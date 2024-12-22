// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

coord operator-(coord const &c1, coord const &c2) {
  return {c1.first - c2.first, c1.second - c2.second};
}

int sgn(int a) { return a < 0 ? -1 : a > 0 ? +1 : 0; }

struct keypad {
  // The arrangement of the keys on this keypad (row-by-row, reading
  // top down)
  vector<string> keys;
  // Memoization table for num_final_presses, see the info for that
  // function
  map<pair<string, int>, size_t> num_presses;

  // Construct from a single string, with '\n' separating the rows.  A
  // ' ' indicates a missing key.  The robots cannot scan over one of
  // those.
  keypad(string const &s);

  // Access
  int w() const { return keys.front().size(); }
  int h() const { return keys.size(); }
  // Returns ' ' for out-of-bounds, so that's treated just like a
  // missing key
  char at(coord const &c) const;

  // What are the coordinates of the given key?
  coord location_of(char ch) const;

  // Return all sensible sequences of arrow key presses between the
  // two positions.  Going in loops and such is not considered.  Also,
  // once over a key, if it'll be needed again then may as well press
  // it immediately since that costs only 1 extra press (all the
  // robots are already lined up for it).  The latter isn't
  // necessarily true of all keypads, e.g.,
  // 123
  //   45A
  // would require something like <<^<< for A to 1.  But it is true
  // for the keypads in the problem.
  vector<string> moves(coord const &from, coord const &to) const;
  // Return all sensible sequences between the two keys
  vector<string> moves(char from, char to) const {
    return moves(location_of(from), location_of(to));
  }

  // Given a sequence s of keys that end with A, how many presses on
  // the outermost dirpad will be needed?  num_dirpads is the number
  // of dirpads between this level and the outermost one.  E.g., if
  // this keypad is itself the outermost dirpad, then num_dirpads is
  // 0, and if this were the numpad being punched by a robot and and
  // there was just one dirpad directly controlling that robot, then
  // num_dirpads is 1.
  size_t num_final_presses(string const &s, int num_dirpads);
};

keypad numpad("789\n456\n123\n 0A");
keypad dirpad(" ^A\n<v>");

keypad::keypad(string const &s) {
  for (char ch : s) {
    if (keys.empty() || ch == '\n')
      keys.push_back("");
    if (ch != '\n')
      keys.back().push_back(ch);
  }
}

char keypad::at(coord const &c) const {
  if (c.first < 0 || c.first >= w() || c.second < 0 || c.second >= h())
    return ' ';
  return keys[c.second][c.first];
}

coord keypad::location_of(char ch) const {
  optional<coord> result;
  for (int x = 0; x < w(); ++x)
    for (int y = 0; y < h(); ++y)
      if (auto c = coord{x, y}; at(c) == ch)
        result = c;
  assert(result.has_value());
  return *result;
}

vector<string> keypad::moves(coord const &from, coord const &to) const {
  if (from == to)
    return {""};
  auto dxy = to - from;
  string xs(abs(dxy.first), dxy.first > 0 ? '>' : '<');
  string ys(abs(dxy.second), dxy.second > 0 ? 'v' : '^');
  if (dxy.second == 0)
    // Going only horizontally, no vertical deviations make sense
    return {xs};
  if (dxy.first == 0)
    // Going only vertically, no horizontal deviations make sense
    return {ys};
  // Have to go both horizontally and vertically.  If horizontal-first
  // is legal, include horizontal->vertical.  If vertical-first is
  // legal, include vertical->horizontal.
  vector<string> result;
  if (at(from + coord{dxy.first, 0}) != ' ')
    // Can go horizontally first
    result.push_back(xs + ys);
  if (at(from + coord{0, dxy.second}) != ' ')
    // Can go vertically first
    result.push_back(ys + xs);
  return result;
}

size_t keypad::num_final_presses(string const &s, int num_dirpads) {
  assert(!s.empty() && s.back() == 'A');
  if (num_dirpads == 0)
    // At the last level, just press the keys
    return s.length();
  if (auto p = num_presses.find({s, num_dirpads}); p != num_presses.end())
    // Already did this one
    return p->second;
  // Starting from the A, figure out what to press
  char ch = 'A';
  size_t total_presses = 0;
  // I have to execute a sequence of steps on the next dirpad.  Each
  // step for pressing one of this stage's keys consists of moving the
  // arm to the right spot using the arrows, followed by an A.
  // Crucially, after the A:
  // 1. All the following dirpads are lined up at A again, so the
  //    state for recursion has been reset.
  // 2. The best way to press the following key for this stage is
  //    independent of the choice of path used for pressing the
  //    current key.
  for (auto next : s) {
    optional<size_t> best_presses;
    // Consider all ways to move from ch to next
    for (auto seq : moves(ch, next)) {
      // After the movement, press the key
      seq.push_back('A');
      // How many presses did that take on the final dirpad?  Note
      // that this is calling dirpad's num_final_presses!  So numpad
      // calls dirpad, and dirpad recurses.
      auto this_presses = dirpad.num_final_presses(seq, num_dirpads - 1);
      best_presses = min(this_presses, best_presses.value_or(this_presses));
    }
    assert(best_presses.has_value());
    total_presses += *best_presses;
    ch = next;
  }
  return num_presses[{s, num_dirpads}] = total_presses;
}

void solve(int num_dirpads) {
  size_t result = 0;
  string to_press;
  while (cin >> to_press)
    result += stoi(to_press) * numpad.num_final_presses(to_press, num_dirpads);
  cout << result << '\n';
}

void part1() { solve(3); }
void part2() { solve(26); }

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
