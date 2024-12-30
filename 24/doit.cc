// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <random>
#include <cassert>

using namespace std;

// Part 1 is whatever, but this "solution" for part 2 seems pretty
// ugly.  It works and is fast, but it's very dependent on the
// structure of the circuit.  The idea is that I know exactly what the
// correct circuit looks like.  I'll have two sets of gates, one for
// incorrect input, and the second produced according to the known
// structure.  I generate a bunch of random inputs (each a vector of
// bits) and evaluate both circuits for those inputs.  That gives me a
// "signature" for all the gates.  Then I'll scan the outputs low to
// high.  When I encounter an output where the (partially-fixed)
// incorrect circuit does not match the correct one, I'll find a swap
// to fix it.  After fixing, just re-evaluate and repeat.  Once
// everything matches, return the list of swapped gates.

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
  gates(size_t num_bits);

  // Evaluate (wire is either an input name or a gate name)
  bitvec eval(string const &wire);
  // Compute the number represented by the z's (part 1)
  size_t output_num();

  // Reset gate values to reevaluate
  void reset() { for (auto &[_, g] : logic) g.value.reset(); }

  // Maps a bit vector signature to the gate or input.  The inputs are
  // chosen so that the correct circuit has unique signatures.
  map<bitvec, string> signatures();

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

gates::gates(size_t num_bits) {
  for (size_t i = 0; i < num_bits; ++i) {
    inputs[sig("x", i)] = 0;
    inputs[sig("y", i)] = 0;
  }
  logic["z00"] = gate{"XOR", "x00", "y00"};
  logic["carry00"] = gate{"AND", "x00", "y00"};
  for (size_t i = 1; i < num_bits; ++i) {
    auto xi = sig("x", i);
    auto yi = sig("y", i);
    logic[sig("sum", i)] = gate{"XOR", xi, yi};
    logic[sig("z", i)] = gate{"XOR", sig("sum", i), sig("carry", i - 1)};
    logic[sig("gen", i)] = gate{"AND", xi, yi};
    logic[sig("prop", i)] = gate{"AND", sig("sum", i), sig("carry", i - 1)};
    logic[sig("carry", i)] = gate{"OR", sig("gen", i), sig("prop", i)};
  }
  logic[sig("z", num_bits)] = logic[sig("carry", num_bits - 1)];
  logic.erase(sig("carry", num_bits - 1));
  // Make random inputs, ensuring that all inputs and gates can be
  // distinguished
  default_random_engine rand;
  uniform_int_distribution<bitvec> uniform;
  do {
    for (auto &[_, value] : inputs)
      value = uniform(rand);
    reset();
  } while (signatures().size() != inputs.size() + logic.size());
}

bitvec gates::eval(string const &wire) {
  if (auto p = inputs.find(wire); p != inputs.end())
    return p->second;
  assert(logic.find(wire) != logic.end());
  auto &g = logic.at(wire);
  if (g.value.has_value())
    return *g.value;
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

map<bitvec, string> gates::signatures() {
  map<bitvec, string> result;
  for (auto const &[input, val] : inputs)
    result[val] = input;
  for (auto const &[output, _] : logic)
    result[eval(output)] = output;
  return result;
}

string gates::fix() {
  size_t num_bits = inputs.size() / 2;
  gates correct(num_bits);
  // Use random bit vectors for signatures
  inputs = correct.inputs;
  auto correct_sigs = correct.signatures();
  vector<string> swapped;
  while (true) {
    // Evaluate, collect signatures
    reset();
    auto sigs = signatures();
    // Look for something broken
    optional<pair<string, string>> to_swap;
    for (size_t i = 0; i <= num_bits; ++i) {
      auto zi = sig("z", i);
      auto right = correct.eval(zi);
      if (eval(zi) != right) {
        // Found a problem
        if (sigs.contains(right))
          // Output exists, but is mislabeled
          to_swap = {zi, sigs[right]};
        else {
          // Logic for this output is wrong
          auto const &g = logic.at(zi);
          // I have no idea what possibilities there really are; my
          // case has an XOR with wrong inputs.  One input is a carry,
          // and the other input is something else.  The correct gate
          // should have a carry and the XOR of the two inputs at this
          // stage.
          assert(g.type == "XOR");
          auto wrong = correct_sigs[eval(g.in1)].starts_with("carry") ?
               g.in2 : g.in1;
          auto sum = sigs[correct.eval(sig("sum", i))];
          to_swap = {wrong, sum};
        }
        break;
      }
    }
    if (!to_swap)
      break;
    // Remember what was swapped and fix this output
    swapped.push_back(to_swap->first);
    swapped.push_back(to_swap->second);
    swap_outputs(to_swap->first, to_swap->second);
  }
  // Sorted list of swapped things
  sort(swapped.begin(), swapped.end());
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
