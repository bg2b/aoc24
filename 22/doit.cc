// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <unordered_map>
#include <cassert>

using namespace std;

int const num_steps = 2000;

// Encode a sequence of price changes into a single number for hashing
inline int encode(int c1, int c2, int c3, int c4) {
  return 19 * (19 * (19 * (c4 + 9) + (c3 + 9)) + (c2 + 9)) + (c1 + 9);
}

struct monkey {
  // Monkey's secrets are public ;-)
  int secret;

  monkey(int secret_) : secret(secret_) {}

  void next() {
    // The pseuorandom generator
    secret ^= secret << 6;
    secret &= 0xffffff;
    secret ^= secret >> 5;
    secret &= 0xffffff;
    secret ^= secret << 11;
    secret &= 0xffffff;
  }

  int price() const { return secret % 10; }

  // Return a hash table mapping an (encoded) sequence to the price
  // when the sequence first occurred
  unordered_map<int, int> sequences();
};

unordered_map<int, int> monkey::sequences() {
  // The recent price changes
  int window[4];
  // A few more buckets than steps to keep from rehashing
  unordered_map<int, int> result(num_steps + 10);
  for (int i = 0; i < num_steps; ++i) {
    window[i % 4] = -price();
    next();
    window[i % 4] += price();
    if (i >= 4) {
      int seq_num = encode(window[(i + 1) % 4], window[(i + 2) % 4],
                           window[(i + 3) % 4], window[(i + 0) % 4]);
      result.try_emplace(seq_num, price());
    }
  }
  return result;
}

void part1() {
  size_t result = 0;
  int n;
  while (cin >> n) {
    monkey m(n);
    for (int _ = 0; _ < num_steps; ++_)
      m.next();
    result += m.secret;
  }
  cout << result << '\n';
}

void part2() {
  unordered_map<int, int> total_prices;
  int n;
  while (cin >> n)
    for (auto [seq_num, price] : monkey(n).sequences())
      // Accumulate prices from all the monkeys
      total_prices[seq_num] += price;
  assert(!total_prices.empty());
  int best_total = -1;
  for (auto [seq_num, total] : total_prices)
    if (total >= best_total)
      best_total = total;
  cout << best_total << '\n';
}

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
