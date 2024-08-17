#include <bits/stdc++.h>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
template <typename T>
inline bool chmax(T& a, T b) {
  return ((a < b) ? (a = b, true) : (false));
}
template <typename T>
inline bool chmin(T& a, T b) {
  return ((a > b) ? (a = b, true) : (false));
}
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

// 勝敗判定が入る（0：o勝ち、1：x勝ち、2：引き分け、3：未決着）
// 存在しない盤面（oooxxx...など）も入るが気にしない
const int pow_4_9 = 262144;
vector<int> big_winning_status_map(pow_4_9);
vector<int> small_winning_status_map(pow_4_9);
// 必勝手の1つが入る（なければ-1）
vector<int> big_winning_move_map_o(pow_4_9, -1);
vector<int> big_winning_move_map_x(pow_4_9, -1);
vector<int> small_winning_move_map_o(pow_4_9, -1);
vector<int> small_winning_move_map_x(pow_4_9, -1);
// debug用
unordered_map<int, string> bit_to_string_map;
unordered_map<string, int> string_to_bit_map;

class MapInitialize {
 private:
  // big_winning_status_mapに追加する
  void set_winning_status_map(int bit, string& str) {
    big_winning_status_map[bit] = small_winning_status_map[bit] = 3;
    // 勝敗判定
    rep(y, 3) {
      if (str[y * 3] == str[y * 3 + 1] && str[y * 3] == str[y * 3 + 2]) {
        if (str[y * 3] == 'o')
          big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
        if (str[y * 3] == 'x')
          big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
      }
    }
    rep(x, 3) {
      if (str[x] == str[x + 3] && str[x] == str[x + 6]) {
        if (str[x] == 'o')
          big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
        if (str[x] == 'x')
          big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
      }
    }
    if (str[0] == str[4] && str[4] == str[8]) {
      if (str[4] == 'o')
        big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
      if (str[4] == 'x')
        big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
    }
    if (str[2] == str[4] && str[4] == str[6]) {
      if (str[4] == 'o')
        big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
      if (str[4] == 'x')
        big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
    }
    bool is_end = true;
    rep(i, 9) if (str[i] == '.') is_end = false;
    if (is_end) {
      small_winning_status_map[bit] = 2;
      int cnt_x = 0, cnt_o = 0;
      rep(i, 9) {
        if (str[i] == 'o') cnt_o++;
        if (str[i] == 'x') cnt_x++;
      }
      if (cnt_x < cnt_o)
        big_winning_status_map[bit] = 0;
      else if (cnt_x > cnt_o)
        big_winning_status_map[bit] = 1;
      else
        big_winning_status_map[bit] = 2;
    }
  }

  // big_winning_move_map_[ox]に追加する
  void set_big_winning_move_map(int bit, string& str) {
    rep(ci, 2) {
      char c = (ci ? 'o' : 'x');
      // 横
      rep(y, 3) {
        if (str[y * 3] == '.' && str[y * 3 + 1] == c && str[y * 3 + 2] == c) {
          if (c == 'o') big_winning_move_map_o[bit] = y * 3;
          if (c == 'x') big_winning_move_map_x[bit] = y * 3;
        }
        if (str[y * 3] == c && str[y * 3 + 1] == '.' && str[y * 3 + 2] == c) {
          if (c == 'o') big_winning_move_map_o[bit] = y * 3 + 1;
          if (c == 'x') big_winning_move_map_x[bit] = y * 3 + 1;
        }
        if (str[y * 3] == c && str[y * 3 + 1] == c && str[y * 3 + 2] == '.') {
          if (c == 'o') big_winning_move_map_o[bit] = y * 3 + 2;
          if (c == 'x') big_winning_move_map_x[bit] = y * 3 + 2;
        }
      }
      // 縦
      rep(x, 3) {
        if (str[x] == '.' && str[x + 3] == c && str[x + 6] == c) {
          if (c == 'o') big_winning_move_map_o[bit] = x;
          if (c == 'x') big_winning_move_map_x[bit] = x;
        }
        if (str[x] == c && str[x + 3] == '.' && str[x + 6] == c) {
          if (c == 'o') big_winning_move_map_o[bit] = x + 3;
          if (c == 'x') big_winning_move_map_x[bit] = x + 3;
        }
        if (str[x] == c && str[x + 3] == c && str[x + 6] == '.') {
          if (c == 'o') big_winning_move_map_o[bit] = x + 6;
          if (c == 'x') big_winning_move_map_x[bit] = x + 6;
        }
      }
      // 斜め
      if (str[0] == '.' && str[4] == c && str[8] == c) {
        if (c == 'o') big_winning_move_map_o[bit] = 0;
        if (c == 'x') big_winning_move_map_x[bit] = 0;
      }
      if (str[0] == c && str[4] == '.' && str[8] == c) {
        if (c == 'o') big_winning_move_map_o[bit] = 4;
        if (c == 'x') big_winning_move_map_x[bit] = 4;
      }
      if (str[0] == c && str[4] == c && str[8] == '.') {
        if (c == 'o') big_winning_move_map_o[bit] = 8;
        if (c == 'x') big_winning_move_map_x[bit] = 8;
      }
      if (str[2] == '.' && str[4] == c && str[6] == c) {
        if (c == 'o') big_winning_move_map_o[bit] = 2;
        if (c == 'x') big_winning_move_map_x[bit] = 2;
      }
      if (str[2] == c && str[4] == '.' && str[6] == c) {
        if (c == 'o') big_winning_move_map_o[bit] = 4;
        if (c == 'x') big_winning_move_map_x[bit] = 4;
      }
      if (str[2] == c && str[4] == c && str[6] == '.') {
        if (c == 'o') big_winning_move_map_o[bit] = 6;
        if (c == 'x') big_winning_move_map_x[bit] = 6;
      }
      small_winning_move_map_o[bit] = big_winning_move_map_o[bit];
      small_winning_move_map_x[bit] = big_winning_move_map_x[bit];

      // TODO：個数で決まるbig_winning_moveの追加
    }
  }

  //
  void dfs(int bit = 0, string str = "", int depth = 0) {
    if (depth == 9) {
      bit_to_string_map[bit] = str;
      string_to_bit_map[str] = bit;

      set_winning_status_map(bit, str);

      set_big_winning_move_map(bit, str);

      return;
    }
    bit *= 4;
    dfs(bit, str + "o", depth + 1);
    dfs(bit + 1, str + "x", depth + 1);
    dfs(bit + 2, str + ".", depth + 1);
    dfs(bit + 3, str + "d", depth + 1);
  }

 public:
  MapInitialize() { dfs(); }
};

int main() {
  int cnt = 0;

  MapInitialize map_initialize;

  assert(big_winning_status_map[string_to_bit_map["....x...o"]] == 3);
  assert(big_winning_status_map[string_to_bit_map["ooo......"]] == 0);
  assert(big_winning_status_map[string_to_bit_map["o...ox..o"]] == 0);
  assert(big_winning_status_map[string_to_bit_map["...xxx..."]] == 1);
  assert(big_winning_status_map[string_to_bit_map["..oxxo..o"]] == 0);
  assert(big_winning_status_map[string_to_bit_map["oo.xoxo.x"]] == 3);
  assert(big_winning_status_map[string_to_bit_map["oodxoxodx"]] == 0);
  assert(big_winning_status_map[string_to_bit_map["ddddddddd"]] == 2);
  assert(big_winning_status_map[string_to_bit_map["ddddddddo"]] == 0);
  assert(big_winning_status_map[string_to_bit_map["ddddddddx"]] == 1);
  assert(small_winning_status_map[string_to_bit_map["....x...o"]] == 3);
  assert(small_winning_status_map[string_to_bit_map["ooo......"]] == 0);
  assert(small_winning_status_map[string_to_bit_map["o...ox..o"]] == 0);
  assert(small_winning_status_map[string_to_bit_map["...xxx..."]] == 1);
  assert(small_winning_status_map[string_to_bit_map["..oxxo..o"]] == 0);
  assert(small_winning_status_map[string_to_bit_map["oo.xoxo.x"]] == 3);
  assert(small_winning_status_map[string_to_bit_map["oodxoxodx"]] == 2);
  assert(small_winning_status_map[string_to_bit_map["ddddddddd"]] == 2);
  assert(small_winning_status_map[string_to_bit_map["ddddddddo"]] == 2);
  assert(small_winning_status_map[string_to_bit_map["ddddddddx"]] == 2);
  //   for (auto [k, v] : big_winning_status_map) {
  //     if (++cnt == 20) break;
  //     cout << bit_to_string_map[k] << " " << k << " " << v << endl;
  //   }
  // for (auto [k, v] : big_winning_move_map_o) {
  //   if (++cnt == 20) break;
  //   cout << bit_to_string_map[k] << " " << k << " " << v << endl;
  // }

  return 0;
}