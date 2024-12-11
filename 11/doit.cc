// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <map>
#include <utility>
#include <cassert>

using namespace std;

using num = unsigned long long;

// The Oracle meditates for a moment and then intones...

int num_digits(num n) {
  int digits = 1;
  while (n >= 10) {
    n /= 10;
    ++digits;
  }
  return digits;
}

struct oracle {
  // Remembered values of power()
  map<pair<num, int>, num> cache;

  // How many stones will the given stone become after the specified
  // number of blinks?
  num power(num stone, int blinks);
};

num oracle::power(num stone, int blinks) {
  if (blinks == 0)
    // Stare for eternity, but the stone will never change.  You owe
    // the Oracle a lump of coal.
    return 1;
  if (auto p = cache.find({stone, blinks}); p != cache.end())
    // You are not the first to seek this knowledge; go ask your
    // mother.  You owe the Oracle a piece of string.
    return p->second;
  auto answer = [&](num n) {
    cache.emplace(make_pair(stone, blinks), n);
    return n;
  };
  if (stone == 0)
    // The answer lies in comprehending unity.  You owe the Oracle a
    // partridge in a pear tree.
    return answer(power(1, blinks - 1));
  if (int d = num_digits(stone); d % 2 == 0) {
    // The whole can only be understood in terms of its parts.  You
    // owe the Oracle some milk and cookies.
    num p10 = 1;
    for (int _ = 0; _ < d / 2; ++_)
      p10 *= 10;
    return answer(power(stone % p10, blinks - 1) +
                  power(stone / p10, blinks - 1));
  }
  // Reply hazy, try again.  You owe the Oracle some non-blinking
  // Christmas lights.
  return answer(power(stone * 2024, blinks - 1));
}

void solve(int blinks) {
  oracle delphi;
  num result = 0;
  num stone;
  while (cin >> stone)
    result += delphi.power(stone, blinks);
  cout << result << '\n';
}

void part1() { solve(25); }
void part2() { solve(75); }

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
