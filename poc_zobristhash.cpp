#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define all(x) (x).begin(), (x).end()

template <typename T>
inline bool chmax(T& a, T b) {
  return ((a < b) ? (a = b, true) : (false));
}
template <typename T>
inline bool chmin(T& a, T b) {
  return ((a > b) ? (a = b, true) : (false));
}
#pragma GCC target("avx")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

constexpr int pow_4_9 = 262144;
array<unordered_map<uint64_t, int>, pow_4_9> zobrist_node_map;
array<uint64_t, 9> zobrist_big_board;
array<array<uint64_t, pow_4_9>, 9> zobrist_bit_1;
array<array<uint64_t, pow_4_9>, 9> zobrist_bit_2;

class ZobristHash {
 private:
 public:
  ZobristHash() {
    const int SEED = 1;
    mt19937_64 rng(SEED);
    uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    rep(i, 9) rep(j, pow_4_9) {
      zobrist_bit_1[i][j] = dist(rng);
      zobrist_bit_2[i][j] = dist(rng);
    }
  }

  uint64_t compute_hash_1(array<int, 9>& board_int, int big_board_int) {
    uint64_t ret = 0;
    rep(i, 9) {
      if (big_board_int & (3 << i)) continue;
      ret ^= zobrist_bit_1[i][board_int[i]];
    }
    return ret;
  }
  uint64_t compute_hash_2(array<int, 9>& board_int, int big_board_int) {
    uint64_t ret = 0;
    rep(i, 9) {
      if (big_board_int & (3 << i)) continue;
      ret ^= zobrist_bit_2[i][board_int[i]];
    }
    return ret;
  }
};

int main() {
  //
  ZobristHash zobrist;

  rep(i, 9) rep(j, 9) {
    cout << i * 9 + j << " " << zobrist_bit_1[i][j] << " "
         << zobrist_bit_2[i][j] << endl;
  }
}