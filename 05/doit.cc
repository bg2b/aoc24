// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <cassert>

using namespace std;

// From checking the size of the rule set, it's apparently a
// completely-specified total order.  If there are n numbers, then
// there are n*(n-1)/2 distinct rules.  I'll just save the ordering
// relation as a set of pairs.
//
// It would be possible to sort once and produce an index mapping each
// number to its position.  That would be a bit faster for processing
// individual updates.  And since the pages numbers turn out to be all
// two digits, it would be possible to represent things with
// length-100 arrays.  For clarity I haven't done these.

set<pair<int, int>> read_rules() {
  set<pair<int, int>> result;
  set<int> pages;
  string line;
  while (getline(cin, line) && !line.empty()) {
    stringstream ss(line);
    int n1, n2;
    char vert;
    ss >> n1 >> vert >> n2;
    assert(ss && vert == '|');
    pages.insert(n1);
    pages.insert(n2);
    result.emplace(n1, n2);
  }
  assert(!pages.empty());
  // This is necessary for a total ordering (though not sufficient
  // since there could be inconsistencies)
  assert(result.size() == pages.size() * (pages.size() - 1) / 2);
  return result;
}

void check_updates(bool fixing) {
  auto rules = read_rules();
  // The comparison function
  auto before = [&](int n1, int n2) { return rules.contains({n1, n2}); };
  string line;
  int result = 0;
  while (getline(cin, line)) {
    line.push_back(',');
    stringstream ss(line);
    vector<int> update;
    int n;
    char comma;
    while (ss >> n >> comma) {
      assert(comma == ',');
      update.push_back(n);
    }
    auto correct = true;
    for (size_t i = 0; correct && i + 1 < update.size(); ++i)
      if (!before(update[i], update[i + 1]))
        correct = false;
    if (fixing == correct)
      // If not fixing, don't count incorrect updates; else don't
      // count already-correct ones
      continue;
    if (fixing)
      sort(update.begin(), update.end(), before);
    assert(update.size() % 2 == 1);
    result += update[update.size() / 2];
  }
  cout << result << '\n';
}

void part1() { check_updates(false); }
void part2() { check_updates(true); }

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
