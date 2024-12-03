// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <optional>
#include <cctype>
#include <cassert>

using namespace std;

// I was tempted to use std::regex, but couldn't bring myself to face
// the horror...

// If looking at digits, advance and return the number
optional<int> number(string const &s, size_t &i) {
  if (!isdigit(s[i])) return nullopt;
  int result = s[i] - '0';
  for (auto c = s[++i]; isdigit(c); c = s[++i])
    result = 10 * result + c - '0';
  return result;
}

// Return the result from a multiply instruction at position i, or 0
// if there's no instruction.  In either case, i is advanced to the
// postion for continued scanning.
int maybe_mul(string const &s, size_t &i) {
  // Always advance at least one position
  if (s[i++] != 'm') return 0;
  // If not a u, the next mul could start at i
  if (s[i] != 'u') return 0;
  // Skip previous u and check for l, etc.
  if (s[++i] != 'l') return 0;
  if (s[++i] != '(') return 0;
  // Skip previous ( and look for a number
  auto n1 = number(s, ++i);
  if (!n1) return 0;
  if (s[i] != ',') return 0;
  // Skip previous , and look for a number
  auto n2 = number(s, ++i);
  if (!n2) return 0;
  if (s[i] != ')') return 0;
  // Skip final )
  ++i;
  return *n1 * *n2;
}

void solve(bool enabling) {
  bool enabled = true;
  int result = 0;
  string line;
  while (getline(cin, line)) {
    for (size_t i = 0; i < line.size(); ) {
      if (enabling) {
        if (line.compare(i, 4, "do()") == 0) {
          i += 4;
          enabled = true;
          continue;
        }
        if (line.compare(i, 7, "don't()") == 0) {
          i += 7;
          enabled = false;
          continue;
        }
      }
      result += (enabled ? 1 : 0) * maybe_mul(line, i);
    }
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
