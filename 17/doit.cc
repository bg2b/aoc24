// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <cassert>

using namespace std;

void print(vector<int> const &out) {
  auto sep = "";
  for (auto i : out) {
    cout << sep << char('0' + i);
    sep = ",";
  }
  cout << '\n';
}

using num = long long;

struct cpu {
  // A, B, C registers
  num reg[3];
  // Instruction pointer
  size_t ip{0};
  // int code ;-)
  vector<int> code;
  // Whatever was output
  vector<int> out;

  // Initialize from cin
  cpu();

  // Run and return output.  If a value is given, initialize the A
  // register with that
  vector<int> run(optional<num> A = nullopt);

  // Search for the minimum self-reproducing value
  optional<num> search(num A, size_t level);
  num search() { return *search(0, code.size() - 1); }
};

cpu::cpu() {
  string _;
  for (int i = 0; i < 3; ++i)
    cin >> _ >> _ >> reg[i];
  cin >> _;
  assert(_ == "Program:");
  string prog;
  cin >> prog;
  prog.push_back(',');
  stringstream ss(prog);
  char comma;
  unsigned c;
  while (ss >> c >> comma) {
    assert(c < 8 && comma == ',');
    code.push_back(c);
  }
}

vector<int> cpu::run(optional<num> A) {
  if (A.has_value()) {
    reg[0] = *A;
    reg[1] = 0;
    reg[2] = 0;
    ip = 0;
    out.clear();
  }
  while (ip + 1 < code.size()) {
    auto inst = code[ip++];
    num operand = code[ip++];
    if (inst != 1 && inst != 3) {
      // Combo operand
      assert(operand != 7);
      if (operand >= 4)
        // Load from A, B, or C
        operand = reg[operand - 4];
    } // else literal operand, unchanged
    switch (inst) {
    case 0:
    case 6:
    case 7: {
      // _dv
      int target = inst == 0 ? 0 : inst - 5;
      reg[target] = reg[0] / (num(1) << operand);
      break;
    }
    case 1:
      // bxl
      reg[1] ^= operand;
      break;
    case 2:
      // bst
      reg[1] = operand & 0x7;
      break;
    case 3:
      // jnz
      if (reg[0] != 0)
        ip = operand;
      break;
    case 4:
      // bxc
      reg[1] ^= reg[2];
      break;
    case 5:
      // out
      out.push_back(operand & 0x7);
      break;
    default:
      assert(0 < inst && inst < 8);
    }
  }
  return out;
}

optional<num> cpu::search(num A, size_t level) {
  // The idea is that the program is vaguely counter-like, and you can
  // work backwards from the high digits
  for (int i = 0; i < 8; ++i) {
    // Try next digit
    auto guess = 8 * A + i;
    auto out = run(guess);
    assert(out.size() <= code.size());
    if (out[level - (code.size() - out.size())] != code[level])
      // Didn't match, try next digit
      continue;
    if (level == 0)
      // Solution!
      return guess;
    // This digit is OK, look for the rest
    if (auto a = search(guess, level - 1); a.has_value())
      return *a;
  }
  return nullopt;
}

void part1() { print(cpu().run()); }
void part2() { cout << cpu().search() << '\n'; }

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
