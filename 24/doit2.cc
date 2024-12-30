// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit2 doit2.cc
// ./doit2 1 < input  # part 1
// ./doit2 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <random>
#include <algorithm>
#include <cassert>

using namespace std;

// This is a little more flexible.  It uses the signature approach of
// doit.cc, so it's semanitically driven.  But instead of relying on
// the exact circuit structure, it tries all possibly relevant swaps
// to fix the lowest incorrect bit.  I'm sure it's possible to feed in
// circuits that this won't fix (it's greedy, and if it can't fix each
// bit in one swap it'll give up), or that it won't fix with the
// minimal number of swaps, but it's better than the original at
// least.

using bitvec = uint64_t;

struct gates {
  // Input values, either initial (0/1 only) or random integers
  // (representing a vector of bits) for part 2
  map<string, bitvec> inputs;
  // A logic gate
  struct gate {
    string type;
    string in1;
    string in2;
    // Filled in when the gate is evaluated
    optional<bitvec> value;
  };
  // output => gate for that output
  map<string, gate> logic;

  // Construct from cin
  gates();
  // Make the correct circuit for the given number of bits
  gates(size_t n);

  // How many input bits?
  size_t num_bits() const { return inputs.size() / 2; }

  // Evaluate (wire is either an input name or a gate name)
  bitvec eval(string const &wire);
  // Compute the number represented by the z's (part 1)
  size_t output_num();

  // Reset gate values to reevaluate
  void reset() { for (auto &[_, g] : logic) g.value.reset(); }

  // Return signatures for all outputs.  The correct circuit has
  // inputs randomly chosen so that everything has a unique signature.
  vector<bitvec> signatures();

  // Add fanins of the given wire to a set
  void get_fanins(string const &wire, set<string> &fanins) const;

  // Swap the outputs of two gates
  void swap_outputs(string const &output1, string const &output2) {
    swap(logic[output1], logic[output2]);
  }

  // Fix everything, return swaps used
  string fix();
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
      assert(value == 0 || value == 1);
      inputs[first] = value;
    } else {
      // Gate
      string type, second, arrow, output;
      ss >> type >> second >> arrow >> output;
      assert(arrow == "->");
      logic.emplace(output, gate{type, first, second});
    }
  }
  assert(inputs.size() % 2 == 0);
}

// prefix with 2-digit value of i, e.g., z00, z01, sum04, carry14
string sig(string const &prefix, size_t i) {
  return prefix + (i < 10 ? '0' + to_string(i) : to_string(i));
}

gates::gates(size_t n) {
  for (size_t i = 0; i < n; ++i) {
    inputs[sig("x", i)] = 0;
    inputs[sig("y", i)] = 0;
  }
  logic["z00"] = gate{"XOR", "x00", "y00"};
  logic["carry00"] = gate{"AND", "x00", "y00"};
  for (size_t i = 1; i < n; ++i) {
    auto xi = sig("x", i);
    auto yi = sig("y", i);
    logic[sig("sum", i)] = gate{"XOR", xi, yi};
    logic[sig("z", i)] = gate{"XOR", sig("sum", i), sig("carry", i - 1)};
    logic[sig("gen", i)] = gate{"AND", xi, yi};
    logic[sig("prop", i)] = gate{"AND", sig("sum", i), sig("carry", i - 1)};
    logic[sig("carry", i)] = gate{"OR", sig("gen", i), sig("prop", i)};
  }
  logic[sig("z", n)] = logic[sig("carry", n - 1)];
  logic.erase(sig("carry", n - 1));
  // Make random inputs, ensuring that all outputs are distinguished
  default_random_engine rand;
  uniform_int_distribution<bitvec> uniform;
  while (true) {
    for (auto &[_, value] : inputs)
      value = uniform(rand);
    reset();
    auto sigs = signatures();
    sort(sigs.begin(), sigs.end());
    if (unique(sigs.begin(), sigs.end()) == sigs.end())
      break;
  }
}

bitvec gates::eval(string const &wire) {
  if (auto p = inputs.find(wire); p != inputs.end())
    return p->second;
  assert(logic.find(wire) != logic.end());
  auto &g = logic.at(wire);
  if (g.value.has_value())
    return *g.value;
  // Guard against infinite recursion in the case when a candidate
  // swap has made something cyclic
  g.value = 0;
  auto v1 = eval(g.in1);
  auto v2 = eval(g.in2);
  if (g.type == "AND")
    v1 &= v2;
  else if (g.type == "OR")
    v1 |= v2;
  else
    v1 ^= v2;
  g.value = v1;
  return v1;
}

size_t gates::output_num() {
  size_t result = 0;
  auto num_bits = inputs.size() / 2;
  for (size_t i = 0; i <= num_bits; ++i) {
    auto zi = sig("z", i);
    // Example input1 does not contain a full set of output bits
    if (logic.contains(zi))
      result += eval(zi) * (size_t(1) << i);
  }
  return result;
}

vector<bitvec> gates::signatures() {
  reset();
  vector<bitvec> result(num_bits() + 1);
  for (size_t i = 0; i <= num_bits(); ++i)
    result[i] = eval(sig("z", i));
  return result;
}

void gates::get_fanins(string const &wire, set<string> &fanins) const {
  if (fanins.contains(wire) || inputs.contains(wire))
    return;
  fanins.insert(wire);
  auto const &g = logic.at(wire);
  get_fanins(g.in1, fanins);
  get_fanins(g.in2, fanins);
}

string gates::fix() {
  // Make correct signatures
  gates correct(num_bits());
  auto correct_sigs = correct.signatures();
  // Use the same random bit vector inputs in the broken circuit
  inputs = correct.inputs;
  auto sigs = signatures();
  // num_correct = Number of low-order correct outputs
  auto num_correct = [&]() {
    for (size_t i = 0; i <= num_bits(); ++i)
      if (sigs[i] != correct_sigs[i])
        return i;
    return num_bits() + 1;
  };
  // fix1 = Find a good swap
  auto fix1 = [&]() {
    optional<pair<string, string>> result;
    auto first_wrong = num_correct();
    // If the lowest incorrect output is mislabeled, just fix that
    for (auto const &[output, _] : logic)
      if (eval(output) == correct_sigs[first_wrong]) {
        result = {sig("z", first_wrong), output};
        break;
      }
    if (!result) {
      // Try all swaps that could affect the lowest incorrect output
      set<string> candidates;
      get_fanins(sig("z", first_wrong), candidates);
      // Everything feeding correct inputs cannot be changed
      set<string> frozen;
      for (size_t i = 0; i < first_wrong; ++i)
        get_fanins(sig("z", i), frozen);
      // Find the swap that fixes the most stuff
      auto best = first_wrong;
      for (auto const &i : candidates)
        if (!frozen.contains(i))
          for (auto const &[j, _] : logic) {
            if (frozen.contains(j))
              continue;
            swap_outputs(i, j);
            reset();
            sigs = signatures();
            if (auto this_correct = num_correct(); this_correct > best) {
              // Found a swap that fixes some stuff
              result = {i, j};
              best = this_correct;
            }
            swap_outputs(i, j);
          }
    }
    assert(result.has_value());
    // Do the swap for real, reevaluate signatures
    swap_outputs(result->first, result->second);
    reset();
    sigs = signatures();
    return result.value();
  };
  // Repeatedly fix things and collect swaps
  set<string> swapped;
  while (num_correct() < num_bits() + 1) {
    auto better = fix1();
    swapped.insert(better.first);
    swapped.insert(better.second);
  }
  string result;
  for (auto const &s : swapped)
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
