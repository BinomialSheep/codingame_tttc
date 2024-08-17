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

/**
 * get_winning_statusがbitboard化でバグったらしいので検証
 *
 */

// 勝敗判定が入る（0：o勝ち、1：x勝ち、2：引き分け、3：未決着）
// 存在しない盤面（oooxxx...など）も入るが気にしない
const int pow_4_9 = 262144;
vector<int> big_winning_status_map(pow_4_9);
vector<int> small_winning_status_map(pow_4_9);

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

  //
  void dfs(int bit = 0, string str = "", int depth = 0) {
    if (depth == 9) {
      bit_to_string_map[bit] = str;
      string_to_bit_map[str] = bit;

      set_winning_status_map(bit, str);

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

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

// 勝敗情報を取得する
WinningStatus landerwaffe(string big_board, bool is_x) {
  switch (big_winning_status_map[string_to_bit_map[big_board]]) {
    case 0:
      return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
    case 1:
      return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
    case 2:
      return WinningStatus::DRAW;
    default:
      return WinningStatus::NONE;
  }
}

WinningStatus salamander(string big_board, bool is_x) {
  // 3連続しているか
  rep(i, 3) {
    if (big_board[i * 3] == big_board[i * 3 + 1] &&
        big_board[i * 3] == big_board[i * 3 + 2]) {
      if (big_board[i * 3] == 'o') {
        return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
      }
      if (big_board[i * 3] == 'x') {
        return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
      }
    }
  }
  rep(j, 3) {
    if (big_board[j] == big_board[j + 3] &&
        big_board[j + 3] == big_board[j + 6]) {
      if (big_board[j] == 'o') {
        return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
      }
      if (big_board[j] == 'x') {
        return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
      }
    }
  }
  if (big_board[0] == big_board[4] && big_board[4] == big_board[8]) {
    if (big_board[4] == 'o') {
      return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
    }
    if (big_board[4] == 'x') {
      return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
    }
  }
  if (big_board[2] == big_board[4] && big_board[4] == big_board[6]) {
    if (big_board[4] == 'o') {
      return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
    }
    if (big_board[4] == 'x') {
      return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
    }
  }

  int cnt_o = 0, cnt_x = 0;
  rep(i, 9) {
    if (big_board[i] == '.') return WinningStatus::NONE;
    if (big_board[i] == 'o') cnt_o++;
    if (big_board[i] == 'x') cnt_x++;
  }
  if (cnt_o == cnt_x) {
    return WinningStatus::DRAW;
  } else if ((cnt_o > cnt_x && !is_x) || (cnt_x > cnt_o && is_x)) {
    return WinningStatus::WIN;
  } else {
    return WinningStatus::LOSE;
  }
}

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

  auto f = [&](string board) {
    if (landerwaffe(board, true) != salamander(board, true)) {
      cerr << board << " " << true;
      assert(false);
    }
    if (landerwaffe(board, false) != salamander(board, false)) {
      cerr << board << " " << false;
      assert(false);
    }
  };
  vector<string> vec = {"....x...o", "ooo......", "o...ox..o", "...xxx...",
                        "..oxxo..o", "oo.xoxo.x", "oodxoxodx", "ddddddddd",
                        "ddddddddo", "ddddddddx"};
  for (auto s : vec) f(s);

  return 0;
}