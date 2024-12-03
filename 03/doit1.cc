// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <regex>
#include <cassert>

using namespace std;

// Concise, but ~5x slower compile and ~10x larger executable than
// the solution in doit.cc

void solve(char const *instructions) {
  regex re(instructions);
  bool enabled = true;
  int result = 0;
  string line;
  while (getline(cin, line))
    for (auto i = sregex_iterator(line.begin(), line.end(), re);
         i != sregex_iterator(); ++i) {
      if (i->str() == "do()")
        enabled = true;
      else if (i->str() == "don't()")
        enabled = false;
      else {
        assert(i->size() == 3);
        if (enabled)
          result += stoi((*i)[1]) * stoi((*i)[2]);
      }
  }
  cout << result << '\n';
}

void part1() { solve(R"(mul\((\d+),(\d+)\))"); }
void part2() { solve(R"(mul\((\d+),(\d+)\)|do\(\)|don't\(\))"); }

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
