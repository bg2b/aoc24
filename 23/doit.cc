// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <cassert>

using namespace std;

struct network {
  map<string, vector<string>> connections;

  // Construct from cin
  network();

  // Part 1
  int num_triplets() const;

  // Search for the maximum clique from some current clique and all
  // the possible extensions.  Update best whenever a new
  // largest-so-far clique is found.
  void max_clique(vector<string> const &clique, vector<string> const &possible,
                  vector<string> &best) const;
  // Find the password for part 2
  string password() const;
};

network::network() {
  string line;
  while (cin >> line) {
    assert(line.length() == 5 && line[2] == '-');
    auto comp1 = line.substr(0, 2);
    auto comp2 = line.substr(3, 2);
    connections[comp1].push_back(comp2);
    connections[comp2].push_back(comp1);
  }
  for (auto &[comp1, comp2s] : connections)
    sort(comp2s.begin(), comp2s.end());
}

int network::num_triplets() const {
  set<string> triplets;
  for (auto const &[comp1, comp2s] : connections)
    if (comp1.front() == 't') {
      for (auto const &comp2 : comp2s) {
        auto const &comp22s = connections.at(comp2);
        vector<string> comp3s;
        set_intersection(comp2s.begin(), comp2s.end(),
                         comp22s.begin(), comp22s.end(),
                         back_inserter(comp3s));
        for (auto const &comp3 : comp3s) {
          vector<string> group{comp1, comp2, comp3};
          sort(group.begin(), group.end());
          triplets.insert(group[0] + group[1] + group[2]);
        }
      }
    }
  return triplets.size();
}

void network::max_clique(vector<string> const &clique,
                         vector<string> const &possible,
                         vector<string> &best) const {
  if (clique.size() + possible.size() <= best.size())
    // Can't possibly be bigger than the current maximum
    return;
  auto remaining = possible;
  while (!remaining.empty()) {
    // Choose the next possibility
    auto comp = remaining.back();
    // Important: drop it so that it's known as having been tried!
    remaining.pop_back();
    // Extend
    auto clique1 = clique;
    clique1.push_back(comp);
    if (clique1.size() > best.size())
      // Found a larger clique
      best = clique1;
    auto const &conns = connections.at(comp);
    // Possible extensions are the things in remain that also connect
    // to the newly-added member
    vector<string> possible1;
    set_intersection(remaining.begin(), remaining.end(),
                     conns.begin(), conns.end(),
                     back_inserter(possible1));
    max_clique(clique1, possible1, best);
  }
}

string network::password() const {
  // Find the maximum clique
  vector<string> best;
  for (auto const &[comp1, comp2s] : connections)
    max_clique({comp1}, comp2s, best);
  // Compute password
  sort(best.begin(), best.end());
  string result;
  for (auto const &comp : best)
    result += comp + ',';
  result.pop_back();
  return result;
}


void part1() { cout << network().num_triplets() << '\n'; }
void part2() { cout << network().password() << '\n'; }

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
