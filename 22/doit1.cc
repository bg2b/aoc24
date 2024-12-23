// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit1 doit1.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int const num_steps = 2000;

// I'll use a table mapping a sequence to the total of prices for all
// buyers that produce that sequence.  For speed, the sequence will be
// represented as a single sequence number.  For potential changes in
// the range -9 to 9 (potential changes), the low 5 bits suffice to
// distinguish among the possibilities.  So a sequence of four changes
// can be represented by 20 bits.  Rather than allocating that much
// memory, I'll use a table, to_packed, to map 10-bit quantities (two
// changes) into one of the 19 * 19 = 361 unique possibilites.  The
// table is used twice reduce 20 bits perfectly to 19 * 19 * 19 * 19
// possiblities.  Since the mapping is perfect, the table can be
// represented by a simple array.
short to_packed[1024];

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

  // The table is shared among all the monkeys.  Each table bin is two
  // numbers: the total prices of buyers so far, and the index for the
  // last buyer to update the total.  The index is used to ensure that
  // if a buyer has the sequence more than once, only the price from
  // the first occurrence gets added.
  void sequences(short index, vector<pair<short, short>> &totals);
};

void monkey::sequences(short index, vector<pair<short, short>> &totals) {
  int seq_num = 0;
  int p = price();
  for (int i = 0; i < num_steps; ++i) {
    // Compute next price and change
    int change = -p;
    next();
    p = price();
    change += p;
    // Update sequence encoding (20 bits)
    seq_num &= 0x7fff;
    seq_num <<= 5;
    seq_num |= change & 0x1f;
    if (i >= 4) {
      // If there have been at least four changes (a full sequence),
      // convert the 20-bit seq_num to one of 19 * 19 * 19 * 19 unique
      // possibilities.
      int si = (to_packed[seq_num & 0x3ff] +
                19 * 19 * int(to_packed[seq_num >> 10]));
      auto &[total, last_index] = totals[si];
      if (last_index != index) {
        // First time this monkey has seen this sequence
        total += p;
        last_index = index;
      } // else this monkey already added a price to the total
    }
  }
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
  // Initialize the table for converting two changes (10 bits) to the
  // one of the unique 19 * 19 possibilites
  for (int c1 = -9; c1 <= 9; ++c1)
    for (int c2 = -9; c2 <= 9; ++c2) {
      int unpacked = ((c1 & 0x1f) << 5) | (c2 & 0x1f);
      int packed = 19 * (c1 + 9) + (c2 + 9);
      to_packed[unpacked] = packed;
    }
  vector<pair<short, short>> total_prices(19 * 19 * 19 * 19);
  int n;
  short index = 0;
  while (cin >> n)
    monkey(n).sequences(++index, total_prices);
  assert(!total_prices.empty());
  int best_total = -1;
  for (auto [total, _] : total_prices)
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
