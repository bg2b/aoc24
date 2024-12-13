// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

using num = long long;
using coord = pair<num, num>;

coord operator+(coord const &c1, coord const &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

coord operator*(num a, coord const &c) {
  return {a * c.first, a * c.second};
}

struct claw {
  coord A;
  coord B;
  coord prize;

  // Construct from the lines describing the machine
  claw(string const &a, string const &b, string const &target, bool part2);

  // Return number of tokens needed to win, if possible
  num to_win() const;
};

claw::claw(string const &a, string const &b, string const &target, bool part2) {
  auto get = [](string const &s, char indicator) {
    auto pos = s.find(indicator);
    assert(pos != string::npos && pos + 2 < s.length());
    return stoll(s.substr(pos + 2));
  };
  auto xy = [&](string const &s) { return coord{get(s, 'X'), get(s, 'Y')}; };
  A = xy(a);
  B = xy(b);
  prize = xy(target);
  if (part2)
    prize = prize + coord{10000000000000, 10000000000000};
}

num claw::to_win() const {
  auto det = A.first * B.second - A.second * B.first;
  // Degenerate case could still win a prize, though there are no such
  // examples in the input.  I'm disappointed...
  assert(det != 0);
  // inv([Ax Ay; Bx By]) * [px; py] = [na; nb]
  // [By -Ay; -Bx Ax] * [px; py] / det = [na; nb]
  // (By*px - Bx*py) / det = na
  // (-Ay*px + Ax*py) / det = nb
  auto na = (B.second * prize.first - B.first * prize.second) / det;
  auto nb = (-A.second * prize.first + A.first * prize.second) / det;
  if (na < 0 || nb < 0 || na * A + nb * B != prize)
    return 0;
  return 3 * na + nb;
}

void solve(bool part2) {
  num result = 0;
  string a, b, c, blank;
  while (getline(cin, a) && getline(cin, b) && getline(cin, c)) {
    result += claw(a, b, c, part2).to_win();
    // Skip a line
    getline(cin, blank);
  }
  cout << result << '\n';
}

void part1() { solve(false); }
void part2() { solve(true); }

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
