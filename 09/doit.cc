// -*- C++ -*-
// g++ -std=c++20 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <cctype>
#include <algorithm>
#include <cassert>

using namespace std;

struct disk {
  // The file ids for each block, or -1 for free
  vector<int> ids;

  // Construct from cin
  disk();

  // Defragment for part 1 or 2, return disk
  disk &defragment1();
  disk &defragment2();

  // Compute checksum
  long checksum() const;
};

disk::disk() {
  int next_id = 0;
  bool file_length = true;
  char c;
  while (cin >> c && isdigit(c)) {
    int fill = file_length ? next_id++ : -1;
    for (int _ = 0; _ < c - '0'; ++_)
      ids.push_back(fill);
    file_length = !file_length;
  }
}

disk &disk::defragment1() {
  size_t start = 0;
  size_t end = ids.size();
  while (true) {
    // Increment start to next free spot
    while (start < ids.size() && ids[start] != -1)
      ++start;
    // Decrement end to next file block
    while (end-- > 0 && ids[end] == -1)
      ;
    if (start >= end)
      break;
    swap(ids[start], ids[end]);
  }
  return *this;
}

disk &disk::defragment2() {
  // [start, contiguous(start)) is the range matching ids[start]
  auto contiguous = [&](size_t start) {
    size_t end;
    for (end = start + 1; end < ids.size() && ids[start] == ids[end]; ++end)
      ;
    assert(end - start < 10);
    return end;
  };
  // Positions of files
  vector<size_t> files;
  // Free spaces of sizes up to 9, sorted by position (size 0 is
  // always empty).  Note that the final disk can have larger free
  // spaces, but they only come from aggregation of free spaces by
  // moving a file to some lower position.  Since files are moved
  // starting from the end, nothing will ever be moved into these
  // larger spaces, so they can be ignored.
  array<list<size_t>, 10> spaces;
  for (size_t i = 0; i < ids.size(); ) {
    auto end = contiguous(i);
    if (ids[i] == -1)
      spaces[end - i].push_back(i);
    else
      files.push_back(i);
    i = end;
  }
  // Move files starting from the end
  while (!files.empty()) {
    size_t start = files.back();
    files.pop_back();
    int len = contiguous(start) - start;
    // Try to find a place to move the file
    list<size_t> *best = nullptr;
    int remaining = 0;
    for (int i = len; i < 10; ++i)
      if (!spaces[i].empty() && spaces[i].front() < start)
        // This is a candiate position
        if (!best || spaces[i].front() < best->front()) {
          // Earliest so far, save it and the remaining space
          best = &spaces[i];
          remaining = i - len;
        }
    if (best) {
      // Found a place
      auto to = best->front();
      best->pop_front();
      // Move the file
      swap_ranges(&ids[to], &ids[to + len], &ids[start]);
      if (remaining) {
        // There's free space after the moved file
        auto &new_spaces = spaces[remaining];
        size_t remaining_start = to + len;
        // Add to the new list of spaces, being sure to maintain the
        // sorted order
        auto pos = new_spaces.begin();
        while (pos != new_spaces.end() && *pos < remaining_start)
          ++pos;
        new_spaces.insert(pos, remaining_start);
      }
    }
  }
  return *this;
}

long disk::checksum() const {
  long result = 0;
  for (size_t i = 0; i < ids.size(); ++i)
    if (ids[i] != -1)
      result += i * ids[i];
  return result;
}

void part1() { cout << disk().defragment1().checksum() << '\n'; }
void part2() { cout << disk().defragment2().checksum() << '\n'; }

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
