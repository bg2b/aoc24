// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <cassert>

using namespace std;

// Part 2 here is slightly less ugly, but again very brittle in that
// it depends exactly on the circuit structure.  It "works" by using
// the fact that fanout counts don't change when gate outputs are
// swapped.  Between those and the gate types, it can tell when
// something must be wrong.  The result just needs to be a list of
// what was wrong, not the actual swaps, so this is somewhat simpler
// than it otherwise would need to be.

struct gates {
  // Inputs plus cached values
  map<string, bool> values;
  // A logic gate
  struct gate {
    string type;
    string in1;
    string in2;
  };
  // output => gate for that output
  map<string, gate> logic;

  // Construct from cin
  gates();

  // Evaluate (wire is either an input name or a gate name)
  bool eval(string const &wire);
  // Compute the number represented by the z's (part 1)
  size_t output_num();
  // Find problems, return the list of things involved in swaps
  string fix() const;
};

gates::gates() {
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string first;
    ss >> first;
    if (first.back() == ':') {
      // Initial value
      first.pop_back();
      assert(!first.empty());
      int value;
      ss >> value;
      values[first] = value != 0;
    } else {
      // Gate
      string type, second, arrow, output;
      ss >> type >> second >> arrow >> output;
      assert(arrow == "->");
      logic.emplace(output, gate{type, first, second});
    }
  }
  assert(values.size() % 2 == 0);
}

// prefix with 2-digit value of i, e.g., z00, z01, sum04, carry14
string sig(string const &prefix, size_t i) {
  return prefix + (i < 10 ? '0' + to_string(i) : to_string(i));
}

bool gates::eval(string const &wire) {
  if (auto p = values.find(wire); p != values.end())
    return p->second;
  assert(logic.find(wire) != logic.end());
  auto &g = logic.at(wire);
  auto v1 = eval(g.in1);
  auto v2 = eval(g.in2);
  if (g.type == "AND")
    v1 = v1 && v2;
  else if (g.type == "OR")
    v1 = v1 || v2;
  else
    v1 = v1 != v2;
  return values[wire] = v1;
}

size_t gates::output_num() {
  size_t result = 0;
  auto num_bits = values.size() / 2;
  for (size_t i = 0; i <= num_bits; ++i) {
    auto zi = sig("z", i);
    // Example input1 does not contain a full set of output bits
    if (logic.contains(zi))
      if (eval(zi))
        result += size_t(1) << i;
  }
  return result;
}

string gates::fix() const {
  auto num_bits = values.size() / 2;
  // Inputs to gates do not change from output swapping, so fanout
  // counts can be used to help distinguish things
  map<string, int> fanouts;
  for (auto const &[_, g] : logic) {
    ++fanouts[g.in1];
    ++fanouts[g.in2];
  }
  // Things that should be (single-bit) sums, carrys, or output zs
  set<string> sum;
  set<string> carry;
  set<string> zs;
  // Everything that looks wrong
  set<string> wrong;
  for (auto const &[output, g] : logic) {
    if (g.type == "OR") {
      // OR gates are the carry
      carry.insert(output);
      if (output == sig("z", num_bits))
        // The last z is is a correct output
        zs.insert(output);
      else if (output.front() == 'z')
        // Other zs would be wrong
        wrong.insert(output);
    } else if (g.type == "XOR") {
      if (g.in1 == "x00" || g.in1 == "y00") {
        // First sum, x00 XOR y00 should be z00
        zs.insert(output);
        if (output != "z00")
          wrong.insert(output);
      } else if (g.in1.front() == 'x' || g.in1.front() == 'y') {
        // One of the other sums of input bits
        sum.insert(output);
        if (output.front() == 'z' || fanouts[output] != 2)
          // A z output from a sum is wrong (except for the first bit
          // handled above).  And a non-z that does not have two
          // fanouts is wrong.
          wrong.insert(output);
      } else {
        // Other XORs should be outputs...
        zs.insert(output);
        if (output.front() != 'z')
          // Non-z's are wrong
          wrong.insert(output);
      }
    } else if (g.in1 == "x00" || g.in1 == "y00")
      // x00 AND y00 is a carry
      carry.insert(output);
  }
  for (size_t i = 0; i <= num_bits; ++i)
    if (!zs.contains(sig("z", i)))
      // Any z that wasn't an output is wrong
      wrong.insert(sig("z", i));
  for (auto const &[wire, n] : fanouts)
    if (n == 2 && wire.front() != 'x' && wire.front() != 'y' &&
        !carry.contains(wire) && !sum.contains(wire))
      // Anything with two fanouts that isn't an input or a carry or a
      // sum is wrong
      wrong.insert(wire);
  string result;
  for (auto const &s : wrong)
    result += s + ',';
  result.pop_back();
  return result;
}

void part1() { cout << gates().output_num() << '\n'; }
void part2() { cout << gates().fix() << '\n'; }

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
