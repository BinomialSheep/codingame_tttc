#include <bits/stdc++.h>
using namespace std;
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
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

// 勝敗判定が入る（0：o勝ち、1：x勝ち、2：引き分け、3：未決着）
// 存在しない盤面（oooxxx...など）も入るが気にしない
// TODO
constexpr int pow_4_9 = 262144;
array<int, pow_4_9> big_winning_status_map = {};
array<int, pow_4_9> small_winning_status_map = {};
// 必勝手の1つが入る（なければ-1）
vector<vector<int>> big_winning_move_map_o(pow_4_9);
vector<vector<int>> big_winning_move_map_x(pow_4_9);
array<int, pow_4_9> small_winning_move_map_o = {};
array<int, pow_4_9> small_winning_move_map_x = {};
// debug用
unordered_map<string, int> string_to_board_idx_map;
unordered_map<int, int> real_to_compressed_bit_map;
unordered_map<int, int> compressed_to_real_bit_map;

class MapInitialize {
 private:
  int tmp_cnt = 0;
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
  void set_big_winning_move_map(const int bit, const string& str) {
    if (big_winning_status_map[bit] != 3) return;
    tmp_cnt++;
    rep(ci, 2) {
      char c = (ci ? 'o' : 'x');
      // 横
      rep(y, 3) {
        if (str[y * 3] == '.' && str[y * 3 + 1] == c && str[y * 3 + 2] == c) {
          if (ci)
            big_winning_move_map_o[bit].push_back(y * 3);
          else
            big_winning_move_map_x[bit].push_back(y * 3);
        } else if (str[y * 3] == c && str[y * 3 + 1] == '.' &&
                   str[y * 3 + 2] == c) {
          if (ci)
            big_winning_move_map_o[bit].push_back(y * 3 + 1);
          else
            big_winning_move_map_x[bit].push_back(y * 3 + 1);
        } else if (str[y * 3] == c && str[y * 3 + 1] == c &&
                   str[y * 3 + 2] == '.') {
          if (ci)
            big_winning_move_map_o[bit].push_back(y * 3 + 2);
          else
            big_winning_move_map_x[bit].push_back(y * 3 + 2);
        }
      }
      // 縦
      rep(x, 3) {
        if (str[x] == '.' && str[x + 3] == c && str[x + 6] == c) {
          if (ci)
            big_winning_move_map_o[bit].push_back(x);
          else
            big_winning_move_map_x[bit].push_back(x);
        } else if (str[x] == c && str[x + 3] == '.' && str[x + 6] == c) {
          if (ci)
            big_winning_move_map_o[bit].push_back(x + 3);
          else
            big_winning_move_map_x[bit].push_back(x + 3);
        } else if (str[x] == c && str[x + 3] == c && str[x + 6] == '.') {
          if (ci)
            big_winning_move_map_o[bit].push_back(x + 6);
          else
            big_winning_move_map_x[bit].push_back(x + 6);
        }
      }
      // 斜め
      if (str[0] == '.' && str[4] == c && str[8] == c) {
        if (ci)
          big_winning_move_map_o[bit].push_back(0);
        else
          big_winning_move_map_x[bit].push_back(0);
      } else if (str[0] == c && str[4] == '.' && str[8] == c) {
        if (ci)
          big_winning_move_map_o[bit].push_back(4);
        else
          big_winning_move_map_x[bit].push_back(4);
      } else if (str[0] == c && str[4] == c && str[8] == '.') {
        if (ci)
          big_winning_move_map_o[bit].push_back(8);
        else
          big_winning_move_map_x[bit].push_back(8);
      }
      if (str[2] == '.' && str[4] == c && str[6] == c) {
        if (ci)
          big_winning_move_map_o[bit].push_back(2);
        else
          big_winning_move_map_x[bit].push_back(2);
      } else if (str[2] == c && str[4] == '.' && str[6] == c) {
        if (ci)
          big_winning_move_map_o[bit].push_back(4);
        else
          big_winning_move_map_x[bit].push_back(4);
      } else if (str[2] == c && str[4] == c && str[6] == '.') {
        if (ci)
          big_winning_move_map_o[bit].push_back(6);
        else
          big_winning_move_map_x[bit].push_back(6);
      }
    }
    if (big_winning_move_map_o[bit].size())
      small_winning_move_map_o[bit] = big_winning_move_map_o[bit][0];
    if (big_winning_move_map_x[bit].size())
      small_winning_move_map_x[bit] = big_winning_move_map_x[bit][0];

    // unify
    sort(all(big_winning_move_map_o[bit]));
    sort(all(big_winning_move_map_x[bit]));
    big_winning_move_map_o[bit].erase(unique(all(big_winning_move_map_o[bit])),
                                      big_winning_move_map_o[bit].end());
    big_winning_move_map_x[bit].erase(unique(all(big_winning_move_map_x[bit])),
                                      big_winning_move_map_x[bit].end());
  }

  void set_intercomversion_real_bit_and_compressed_bit(const int compressed,
                                                       const string& str) {
    int real = 0;
    rep(i, 9) {
      switch (str[i]) {
        case '.':
          break;
        case 'o':
          real |= 1 << (i * 2);
          break;
        case 'x':
          real |= 2 << (i * 2);
          break;
        case 'd':
          real |= 3 << (i * 2);
          break;
        default:
          assert(false);
          break;
      }
    }
    real_to_compressed_bit_map[real] = compressed;
    compressed_to_real_bit_map[compressed] = real;
  }

  //
  void dfs(int bit = 0, string str = "", int depth = 0) {
    if (depth == 9) {
      string_to_board_idx_map[str] = bit;

      set_winning_status_map(bit, str);
      set_big_winning_move_map(bit, str);

      // bitboard上のbit値と今の座圧したbit値の相互変換
      set_intercomversion_real_bit_and_compressed_bit(bit, str);

      return;
    }
    bit *= 4;
    dfs(bit, str + ".", depth + 1);
    dfs(bit + 1, str + "o", depth + 1);
    dfs(bit + 2, str + "x", depth + 1);
    dfs(bit + 3, str + "d", depth + 1);
  }

  int set_winning_status_vec(string& str) {
    int ret = 3;
    // 勝敗判定
    rep(y, 3) {
      if (str[y * 3] == str[y * 3 + 1] && str[y * 3] == str[y * 3 + 2]) {
        if (str[y * 3] == 'o') ret = 0;
        if (str[y * 3] == 'x') ret = 1;
      }
    }
    rep(x, 3) {
      if (str[x] == str[x + 3] && str[x] == str[x + 6]) {
        if (str[x] == 'o') ret = 0;
        if (str[x] == 'x') ret = 1;
      }
    }
    if (str[0] == str[4] && str[4] == str[8]) {
      if (str[4] == 'o') ret = 0;
      if (str[4] == 'x') ret = 1;
    }
    if (str[2] == str[4] && str[4] == str[6]) {
      if (str[4] == 'o') ret = 0;
      if (str[4] == 'x') ret = 1;
    }
    bool is_end = true;
    rep(i, 9) if (str[i] == '.') is_end = false;
    if (is_end) {
      int cnt_x = 0, cnt_o = 0;
      rep(i, 9) {
        if (str[i] == 'o') cnt_o++;
        if (str[i] == 'x') cnt_x++;
      }
      if (cnt_x < cnt_o)
        ret = 0;
      else if (cnt_x > cnt_o)
        ret = 1;
      else
        ret = 2;
    }
    return ret;
  }

  //
 public:
  unordered_set<string> dfs2() {
    unordered_set<string> ret;

    auto dfs3 = [&](auto rec, string str = ".........") {
      if (ret.count(str)) return;
      ret.insert(str);

      if (set_winning_status_vec(str) != 3) return;

      rep(i, 9) {
        if (str[i] != '.') continue;
        str[i] = 'o';
        rec(rec, str);
        str[i] = 'x';
        rec(rec, str);
        str[i] = 'd';
        rec(rec, str);
        str[i] = '.';
      }
    };

    dfs3(dfs3);
    cout << ret.size() << endl;
    return ret;
  }

 public:
  MapInitialize() {
    fill(all(small_winning_move_map_o), -1);
    fill(all(small_winning_move_map_x), -1);
    dfs();
    cout << tmp_cnt << endl;
  }
};

int main() {
  int cnt = 0;

  MapInitialize map_initialize;

  rep(i, 10) {
    // if (small_winning_status_map[i] != 3) continue;
    cout << small_winning_move_map_x[i] << endl;
  }

  assert(big_winning_status_map[string_to_board_idx_map["....x...o"]] == 3);
  assert(big_winning_status_map[string_to_board_idx_map["ooo......"]] == 0);
  assert(big_winning_status_map[string_to_board_idx_map["o...ox..o"]] == 0);
  assert(big_winning_status_map[string_to_board_idx_map["...xxx..."]] == 1);
  assert(big_winning_status_map[string_to_board_idx_map["..oxxo..o"]] == 0);
  assert(big_winning_status_map[string_to_board_idx_map["oo.xoxo.x"]] == 3);
  assert(big_winning_status_map[string_to_board_idx_map["oodxoxodx"]] == 0);
  assert(big_winning_status_map[string_to_board_idx_map["ddddddddd"]] == 2);
  assert(big_winning_status_map[string_to_board_idx_map["ddddddddo"]] == 0);
  assert(big_winning_status_map[string_to_board_idx_map["ddddddddx"]] == 1);
  assert(small_winning_status_map[string_to_board_idx_map["....x...o"]] == 3);
  assert(small_winning_status_map[string_to_board_idx_map["ooo......"]] == 0);
  assert(small_winning_status_map[string_to_board_idx_map["o...ox..o"]] == 0);
  assert(small_winning_status_map[string_to_board_idx_map["...xxx..."]] == 1);
  assert(small_winning_status_map[string_to_board_idx_map["..oxxo..o"]] == 0);
  assert(small_winning_status_map[string_to_board_idx_map["oo.xoxo.x"]] == 3);
  assert(small_winning_status_map[string_to_board_idx_map["oodxoxodx"]] == 2);
  assert(small_winning_status_map[string_to_board_idx_map["ddddddddd"]] == 2);
  assert(small_winning_status_map[string_to_board_idx_map["ddddddddo"]] == 2);
  assert(small_winning_status_map[string_to_board_idx_map["ddddddddx"]] == 2);
  assert(compressed_to_real_bit_map[string_to_board_idx_map["dd..x...o"]] ==
         3 + 3 * 4 + 2 * 256 + 1 * 65536);
  assert(compressed_to_real_bit_map[string_to_board_idx_map[".oxox...."]] ==
         1 * 4 + 2 * 16 + 1 * 64 + 2 * 256);
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