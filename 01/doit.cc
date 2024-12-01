// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

void part1(vector<int> const &l1, vector<int> const &l2) {
  assert(l1.size() == l2.size());
  int result = 0;
  for (size_t i = 0; i < l1.size(); ++i)
    result += abs(l1[i] - l2[i]);
  cout << result << '\n';
}

void part2(vector<int> const &l1, vector<int> const &l2) {
  // Advance past maximal range matching n, return length of range
  auto advance = [](int n, size_t &i, vector<int> const &l) {
    while (i < l.size() && l[i] < n)
      ++i;
    auto start = i;
    while (i < l.size() && l[i] == n)
      ++i;
    return i - start;
  };
  int result = 0;
  for (size_t i1 = 0, i2 = 0; i1 < l1.size(); ) {
    auto n = l1[i1];
    result += n * advance(n, i1, l1) * advance(n, i2, l2);
  }
  cout << result << '\n';
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  vector<int> l1, l2;
  int n1, n2;
  while (cin >> n1 >> n2) {
    l1.push_back(n1);
    l2.push_back(n2);
  }
  sort(l1.begin(), l1.end());
  sort(l2.begin(), l2.end());
  if (*argv[1] == '1')
    part1(l1, l2);
  else
    part2(l1, l2);
  return 0;
}
