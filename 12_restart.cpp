/**
 * 12_restart.cpp
 *
 * FIX_ME!!!!!!!!!!
 */
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

static uint32_t randXor() {
  static uint32_t x = 123456789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;

  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

// 時間を管理するクラス
class TimeKeeper {
 private:
  high_resolution_clock::time_point start_time_;
  int64_t time_threshold_;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeper(const int64_t& time_threshold)
      : start_time_(high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const {
    auto diff = high_resolution_clock::now() - start_time_;
    return duration_cast<milliseconds>(diff).count() >= time_threshold_;
  }
};

// 勝敗判定が入る（0：o勝ち、1：x勝ち、2：引き分け、3：未決着）
// 存在しない盤面（oooxxx...など）も入るが気にしない
constexpr int pow_4_9 = 262144;
array<int, pow_4_9> big_winning_status_map = {};
array<int, pow_4_9> small_winning_status_map = {};
// 必勝手候補（9桁のbitで入っているので注意）
array<int, pow_4_9> big_winning_move_map_o = {};
array<int, pow_4_9> big_winning_move_map_x = {};
// 必勝手の1つが入る（なければ-1）
array<int, pow_4_9> small_winning_move_map_o = {};
array<int, pow_4_9> small_winning_move_map_x = {};
// debug用
array<string, pow_4_9> bit_to_string_map;
unordered_map<string, int> string_to_bit_map;

class MapInitialize {
 private:
  // big_winning_status_mapに追加する
  void set_winning_status_map(const int bit, const string& str) {
    big_winning_status_map[bit] = small_winning_status_map[bit] = 3;

    bool is_end = true;
    int cnt_x = 0, cnt_o = 0;
    rep(i, 9) {
      switch (str[i]) {
        case ('.'):
          is_end = false;
          break;
        case ('o'):
          cnt_o++;
          break;
        case ('x'):
          cnt_x++;
          break;
      }
    }
    if (cnt_o < 2 && cnt_x < 2) {
      if (is_end) {
        small_winning_status_map[bit] = 2;
        if (cnt_x < cnt_o)
          big_winning_status_map[bit] = 0;
        else if (cnt_x > cnt_o)
          big_winning_status_map[bit] = 1;
        else
          big_winning_status_map[bit] = 2;
      }
      return;
    }

    // 勝敗判定
    rep(y, 3) {
      if (str[y * 3] == str[y * 3 + 1] && str[y * 3] == str[y * 3 + 2]) {
        if (str[y * 3] == 'o') {
          big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
          return;
        } else if (str[y * 3] == 'x') {
          big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
          return;
        }
      }
    }
    rep(x, 3) {
      if (str[x] == str[x + 3] && str[x] == str[x + 6]) {
        if (str[x] == 'o') {
          big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
          return;
        } else if (str[x] == 'x') {
          big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
          return;
        }
      }
    }
    if (str[0] == str[4] && str[4] == str[8]) {
      if (str[4] == 'o') {
        big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
        return;
      } else if (str[4] == 'x') {
        big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
        return;
      }
    }
    if (str[2] == str[4] && str[4] == str[6]) {
      if (str[4] == 'o') {
        big_winning_status_map[bit] = small_winning_status_map[bit] = 0;
        return;
      } else if (str[4] == 'x') {
        big_winning_status_map[bit] = small_winning_status_map[bit] = 1;
        return;
      }
    }

    if (is_end) {
      small_winning_status_map[bit] = 2;
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
    // これで524288→391550まで減る
    if (big_winning_status_map[bit] != 3) return;

    int cnt_x = 0, cnt_o = 0;
    for (auto c : str) {
      if (c == 'o')
        cnt_o++;
      else if (c == 'x')
        cnt_x++;
    }

    rep(ci, 2) {
      // これで391550→261692まで減る
      if (ci == 0) {
        if (cnt_x < 2) continue;
        if (cnt_x == 5) {
          rep(i, 9) if (str[i] == '.') big_winning_move_map_x[bit] |= 1 << i;
          continue;
        }
      }
      if (ci == 1) {
        if (cnt_o < 2) continue;
        if (cnt_o == 5) {
          rep(i, 9) if (str[i] == '.') big_winning_move_map_o[bit] |= 1 << i;
          continue;
        }
      }
      char c = (ci ? 'o' : 'x');
      // 横
      rep(y, 3) {
        if (str[y * 3] == '.' && str[y * 3 + 1] == c && str[y * 3 + 2] == c) {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << (y * 3);
          else
            big_winning_move_map_x[bit] |= 1 << (y * 3);
        } else if (str[y * 3] == c && str[y * 3 + 1] == '.' &&
                   str[y * 3 + 2] == c) {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << (y * 3 + 1);
          else
            big_winning_move_map_x[bit] |= 1 << (y * 3 + 1);
        } else if (str[y * 3] == c && str[y * 3 + 1] == c &&
                   str[y * 3 + 2] == '.') {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << (y * 3 + 2);
          else
            big_winning_move_map_x[bit] |= 1 << (y * 3 + 2);
        }
      }
      // 縦
      rep(x, 3) {
        if (str[x] == '.' && str[x + 3] == c && str[x + 6] == c) {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << x;
          else
            big_winning_move_map_x[bit] |= 1 << x;
        } else if (str[x] == c && str[x + 3] == '.' && str[x + 6] == c) {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << (x + 3);
          else
            big_winning_move_map_x[bit] |= 1 << (x + 3);
        } else if (str[x] == c && str[x + 3] == c && str[x + 6] == '.') {
          if (c == 'o')
            big_winning_move_map_o[bit] |= 1 << (x + 6);
          else
            big_winning_move_map_x[bit] |= 1 << (x + 6);
        }
      }
      // 斜め
      if (str[0] == '.' && str[4] == c && str[8] == c) {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1;
        else
          big_winning_move_map_x[bit] |= 1;
      } else if (str[0] == c && str[4] == '.' && str[8] == c) {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1 << 4;
        else
          big_winning_move_map_x[bit] |= 1 << 4;
      } else if (str[0] == c && str[4] == c && str[8] == '.') {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1 << 8;
        else
          big_winning_move_map_x[bit] |= 1 << 8;
      }
      if (str[2] == '.' && str[4] == c && str[6] == c) {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1 << 2;
        else
          big_winning_move_map_x[bit] |= 1 << 2;
      } else if (str[2] == c && str[4] == '.' && str[6] == c) {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1 << 4;
        else
          big_winning_move_map_x[bit] |= 1 << 4;
      } else if (str[2] == c && str[4] == c && str[6] == '.') {
        if (c == 'o')
          big_winning_move_map_o[bit] |= 1 << 6;
        else
          big_winning_move_map_x[bit] |= 1 << 6;
      }
    }
    if (big_winning_move_map_o[bit]) {
      rep(i, 9) {
        if (big_winning_move_map_o[bit] >> i & 1) {
          small_winning_move_map_o[bit] = i;
          break;
        }
      }
    }
    if (big_winning_move_map_x[bit]) {
      rep(i, 9) {
        if (big_winning_move_map_x[bit] >> i & 1) {
          small_winning_move_map_x[bit] = i;
          break;
        }
      }
    }
  }

  void dfs(int bit = 0, string str = "", int depth = 0) {
    if (depth == 9) {
      reverse(all(str));
      string_to_bit_map[str] = bit;
      bit_to_string_map[bit] = str;

      set_winning_status_map(bit, str);
      set_big_winning_move_map(bit, str);

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

 public:
  MapInitialize() {
    fill(all(small_winning_move_map_o), -1);
    fill(all(small_winning_move_map_x), -1);
    dfs();
  }
};

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};
void print_winning_status(WinningStatus status) {
  switch (status) {
    case WIN:
      cerr << "WIN" << endl;
      break;
    case LOSE:
      cerr << "LOSE" << endl;
      break;
    case DRAW:
      cerr << "DRAW" << endl;
      break;
    case NONE:
      cerr << "NONE" << endl;
      break;
    default:
      break;
  }
}

using actionType = int;
using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;

class State {
 private:
  inline static const vector<int> action_to_now_big_board = {
      8, 8, 8, 7, 7, 7, 6, 6, 6, 8, 8, 8, 7, 7, 7, 6, 6, 6, 8, 8, 8,
      7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 5, 5, 5, 4, 4, 4,
      3, 3, 3, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0,
      2, 2, 2, 1, 1, 1, 0, 0, 0, 2, 2, 2, 1, 1, 1, 0, 0, 0};
  inline static const vector<int> action_to_next_big_board = {
      8, 7, 6, 8, 7, 6, 8, 7, 6, 5, 4, 3, 5, 4, 3, 5, 4, 3, 2, 1, 0,
      2, 1, 0, 2, 1, 0, 8, 7, 6, 8, 7, 6, 8, 7, 6, 5, 4, 3, 5, 4, 3,
      5, 4, 3, 2, 1, 0, 2, 1, 0, 2, 1, 0, 8, 7, 6, 8, 7, 6, 8, 7, 6,
      5, 4, 3, 5, 4, 3, 5, 4, 3, 2, 1, 0, 2, 1, 0, 2, 1, 0};
  inline static const vector<int> action_to_small_board_digit = {
      16, 14, 12, 16, 14, 12, 16, 14, 12, 10, 8,  6,  10, 8,  6,  10, 8,
      6,  4,  2,  0,  4,  2,  0,  4,  2,  0,  16, 14, 12, 16, 14, 12, 16,
      14, 12, 10, 8,  6,  10, 8,  6,  10, 8,  6,  4,  2,  0,  4,  2,  0,
      4,  2,  0,  16, 14, 12, 16, 14, 12, 16, 14, 12, 10, 8,  6,  10, 8,
      6,  10, 8,  6,  4,  2,  0,  4,  2,  0,  4,  2,  0};
  inline static const vector<int> big_board_shift_1 = {
      1 << (16 - 2 * 8), 1 << (16 - 2 * 7), 1 << (16 - 2 * 6),
      1 << (16 - 2 * 5), 1 << (16 - 2 * 4), 1 << (16 - 2 * 3),
      1 << (16 - 2 * 2), 1 << (16 - 2 * 1), 1 << (16 - 2 * 0)};
  inline static const vector<int> big_board_shift_2 = {
      2 << (16 - 2 * 8), 2 << (16 - 2 * 7), 2 << (16 - 2 * 6),
      2 << (16 - 2 * 5), 2 << (16 - 2 * 4), 2 << (16 - 2 * 3),
      2 << (16 - 2 * 2), 2 << (16 - 2 * 1), 2 << (16 - 2 * 0)};
  inline static const vector<int> big_board_shift_3 = {
      3 << (16 - 2 * 8), 3 << (16 - 2 * 7), 3 << (16 - 2 * 6),
      3 << (16 - 2 * 5), 3 << (16 - 2 * 4), 3 << (16 - 2 * 3),
      3 << (16 - 2 * 2), 3 << (16 - 2 * 1), 3 << (16 - 2 * 0)};
  // big boardのindexから、そのbig boardの左上のboard idxを引く
  inline static const vector<int> big_board_to_board_start = {
      60, 57, 54, 33, 30, 27, 6, 3, 0};
  // bit boardへの移行過程
  inline static const vector<int> small_board_start_i_idff = {
      20, 19, 18, 11, 10, 9, 2, 1, 0};

 public:
  // vector<string> board;
  array<int, 9> board_int = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  bool is_x = true;
  // 次にどのゲームを着手するか
  int big_board_index = -1;
  // ミニゲーム自体の勝敗 (o, x, ., d := draw)
  int big_board_int = 0;

  State() {}

  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  void advance(actionType action) {
    big_board_index = action_to_now_big_board[action];

    // big_board_indexのaaction_to_small_board_digit[action]桁目に指す
    if (is_x) {
      board_int[big_board_index] |= 2 << action_to_small_board_digit[action];
    } else {
      board_int[big_board_index] |= 1 << action_to_small_board_digit[action];
    }
    is_x ^= 1;

    check_small_winning_status();
    big_board_index = action_to_next_big_board[action];
    if (big_board_int & big_board_shift_3[big_board_index])
      big_board_index = -1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<actionType> legal_actions() {
    vector<actionType> ret;

    if (big_board_index == -1) {
      rep(i, 9) {
        if (big_board_int & big_board_shift_3[i]) continue;
        rep(j, 9) {
          if ((board_int[i] & (3 << (j * 2))) == 0) {
            ret.emplace_back(big_board_to_board_start[i] +
                             small_board_start_i_idff[j]);
          }
        }
      }
    } else {
      rep(j, 9) {
        if ((board_int[big_board_index] & (3 << (j * 2))) == 0) {
          ret.emplace_back(big_board_to_board_start[big_board_index] +
                           small_board_start_i_idff[j]);
        }
      }
    }
    return ret;
  }

  /**
   * スモールボードが決着した場合はビッグボードを更新する
   */
  void check_small_winning_status() {
    int bit = board_int[big_board_index];

    switch (small_winning_status_map[bit]) {
      case 0:
        big_board_int |= big_board_shift_1[big_board_index];
        break;
      case 1:
        big_board_int |= big_board_shift_2[big_board_index];
        break;
      case 2:
        big_board_int |= big_board_shift_3[big_board_index];
        break;
      default:
        break;
    }
  }

  /**
   * 1手必勝判定。
   * 1手で勝ちになるアクションがあればそれを返す。なければ-1を返す。
   */
  actionType find_winning_move() {
    if (is_x) {
      int moves = big_winning_move_map_x[big_board_int];
      if (moves == 0) return -1;
      rep(i, 9) {
        if (moves >> i & 1) {
          auto res = find_small_winning_move('x', i);
          if (res != -1) return res;
        }
      }
    } else {
      int moves = big_winning_move_map_o[big_board_int];
      if (moves == 0) return -1;
      rep(i, 9) {
        if (moves >> i & 1) {
          auto res = find_small_winning_move('o', i);
          if (res != -1) return res;
        }
      }
    }
    return -1;
  }
  actionType find_small_winning_move(char c, int board_idx) {
    return (c == 'x' ? small_winning_move_map_x[board_int[board_idx]]
                     : small_winning_move_map_o[board_int[board_idx]]);
  }

  // 勝敗情報を取得する
  WinningStatus get_winning_status() {
    switch (big_winning_status_map[big_board_int]) {
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

  bool is_done() { return get_winning_status() != WinningStatus::NONE; }

  void print_board() {
    vector<string> now_bord;
    for (int i = 8; i >= 0; i--)
      now_bord.emplace_back(bit_to_string_map[board_int[i]]);
    rep(i, 9) {
      rep(j, 9) {
        cerr << now_bord[8 - action_to_now_big_board[i * 9 + j]]
                        [action_to_next_big_board[i * 9 + j]];
      }
      cerr << endl;
    }
    string now_big_board = bit_to_string_map[big_board_int];
    reverse(all(now_big_board));
    rep(i, 3) {
      rep(j, 3) cerr << now_big_board[i * 3 + j];
      cerr << endl;
    }
    cerr << "big_board_index: " << big_board_index << endl;
  }

  bool operator==(const State& other) const {
    // TODO：keyでの比較に変えたいかも
    return this->big_board_int == other.big_board_int &&
           this->big_board_index == other.big_board_index &&
           this->board_int == other.board_int;
  }
};

namespace montecarlo {

/**
 * 決着するまでランダム手でプレイアウトする
 */
float playout(State& state) {
  // 既に決着しているならそれを返す
  WinningStatus winningStatus = state.get_winning_status();
  if (winningStatus == WinningStatus::WIN) return 1;
  if (winningStatus == WinningStatus::LOSE) return 0;
  if (winningStatus == WinningStatus::DRAW) return 0.5;
  // 必勝手があれば終わり
  if (state.find_winning_move() != -1) return 1;

  // 合法手を取得
  auto legal_actions = state.legal_actions();
  // 置く場所がない時は引き分け判定されているはず
  assert(!legal_actions.empty());

  // ランダムプレイ
  actionType action = legal_actions[randXor() % (legal_actions.size())];
  state.advance(action);
  return 1 - playout(state);
}

constexpr const double C = 0.5;             // UCB1の計算に使う定数
constexpr const int EXPAND_THRESHOLD = 10;  // ノードを展開する閾値

// MCTSの計算に使うノード
class Node {
 public:
  State state;
  list<Node> child_nodes;
  double win_count;  // 累計価値
  int visit_num;     // 試行回数
  // mcts-solver用
  WinningStatus winning_status;
  int must_lose_count;
  int must_draw_count;
  int action_count;

  Node(State& arg_state)
      : state(arg_state),
        win_count(0),
        visit_num(0),
        winning_status(WinningStatus::NONE),
        must_lose_count(0),
        must_draw_count(0),
        action_count(0) {}

  // ノードの評価を行う
  float evaluate() {
    if (state.is_done()) {
      float value;
      switch (state.get_winning_status()) {
        case (WinningStatus::WIN):
          value = 1;
          winning_status = WinningStatus::WIN;
          break;
        case (WinningStatus::LOSE):
          value = 0;
          winning_status = WinningStatus::LOSE;
          break;
        default:
          value = 0.5;
          winning_status = WinningStatus::DRAW;
          break;
      }
      win_count += value;
      visit_num++;
      return value;
    }
    // 葉ノードの場合
    if (child_nodes.empty()) {
      State next_state = state;
      float value = playout(next_state);
      win_count += value;
      visit_num++;
      // 閾値回試行したらノードを展開する
      if (visit_num == EXPAND_THRESHOLD) {
        expand();
        action_count = (int)child_nodes.size();
      }
      return value;
    } else {
      // 葉ノード以外では子ノードの値
      Node& next_node = nextChildNode();
      float value = 1 - next_node.evaluate();

      win_count += value;
      visit_num++;
      switch (next_node.winning_status) {
        case WinningStatus::NONE:
          break;
        case WinningStatus::WIN:
          must_lose_count++;
          if (must_lose_count + must_draw_count == action_count) {
            winning_status =
                (must_draw_count ? WinningStatus::DRAW : WinningStatus::LOSE);
          }
          break;
        case WinningStatus::LOSE:
          // 子が必負なら必勝を追加する
          winning_status = WinningStatus::WIN;
          break;
        case WinningStatus::DRAW:
          must_draw_count++;
          if (must_lose_count + must_draw_count == action_count) {
            winning_status = WinningStatus::DRAW;
          }
          break;
        default:
          break;
      }

      return value;
    }
  }

  // ノードを展開する
  void expand() {
    auto legal_actions = state.legal_actions();
    child_nodes.clear();
    for (const auto action : legal_actions) {
      child_nodes.emplace_back(state);
      child_nodes.back().state.advance(action);
    }
  }

  // どのノードを評価するか選択する
  Node& nextChildNode() {
    for (auto& child_node : child_nodes) {
      // 試行回数0のノードは優先的に選択する
      if (child_node.visit_num < 5) return child_node;
    }
    int sum_n = 0;  // 全ノードの試行回数の総和
    for (const auto& child_node : child_nodes) sum_n += child_node.visit_num;

    double best_value = -INF;
    // int best_idx = -1;
    vector<int> delete_idxes;

    list<montecarlo::Node>::iterator best_it = child_nodes.end();

    for (auto it = child_nodes.begin(); it != child_nodes.end();) {
      const auto& child_node = *it;
      if (child_node.winning_status != WinningStatus::NONE) {
        // 決着済みのノードは今後評価しない
        it = child_nodes.erase(it);
        continue;
      }
      double ucb1_value = 1. -
                          child_node.win_count / (double)child_node.visit_num +
                          C * sqrt(2. * log(sum_n) / child_node.visit_num);
      if (ucb1_value > best_value) {
        best_it = it;
        best_value = ucb1_value;
      }
      ++it;
    }

    assert(best_it != child_nodes.end());
    return *best_it;
  }
};

// 制限時間(ms)を指定してMCTSで行動を決定する
pair<actionType, double> exec_mcts(Node& root_node,
                                   const int64_t time_threshold = 90) {
  if (root_node.child_nodes.empty()) root_node.expand();
  auto time_keeper = TimeKeeper(time_threshold);
  int turn;

  for (turn = 1;; turn++) {
    if (turn & (1 << 7) && time_keeper.isTimeOver()) break;
    if (root_node.winning_status != WinningStatus::NONE) {
      print_winning_status(root_node.winning_status);
      break;
    }
    root_node.evaluate();
  }
  vector<actionType> legal_actions = root_node.state.legal_actions();

  cerr << "turn: " << turn << endl;

  int idx = 0;
  int max_score = -1e9;
  int win_idx = -1;
  int draw_idx = -1;
  int lose_count = 0;
  int i = 0;
  list<montecarlo::Node>::iterator best_it;

  cerr << "root_node.child_nodes.size(): " << root_node.child_nodes.size()
       << endl;

  for (auto it = root_node.child_nodes.begin();
       it != root_node.child_nodes.end(); ++it, i++) {
    int n = it->visit_num;
    cerr << legal_actions[i] << " " << n << endl;
    print_winning_status(it->winning_status);
    if (chmax(max_score, n)) best_it = it, idx = i;
    if (it->winning_status == WinningStatus::LOSE) win_idx = i;
    if (it->winning_status == WinningStatus::DRAW) draw_idx = i;
    if (it->winning_status == WinningStatus::WIN) lose_count++;
  }

  double win_rate =
      ((double)max_score - best_it->win_count) / (double)max_score;

  if (win_idx != -1) {
    cerr << "必勝" << endl;
    return make_pair(legal_actions[win_idx], 1);
  } else if (draw_idx != -1 && lose_count == (int)legal_actions.size()) {
    cerr << "引き分け" << endl;
    return make_pair(legal_actions[draw_idx], 0.5);
  }

  return make_pair(legal_actions[idx], win_rate);
}

Node advane_node(Node& root_node, actionType action) {
  root_node.state.advance(action);
  if (root_node.child_nodes.empty()) {
    cerr << "root_node.child_nodes.empty() == true" << endl;
    return Node(root_node.state);
  }
  vector<actionType> legal_actions = root_node.state.legal_actions();
  cerr << "legal_actions.size(): " << legal_actions.size() << endl;
  for (auto it = root_node.child_nodes.begin();
       it != root_node.child_nodes.end(); it++) {
    if (root_node.state == (*it).state) {
      return *it;
    }
  }
  // 削除済み（必敗）局面を相手が選んでいる場合は新規追加が必要
  cerr << "advane_node() 相手が必敗を選んだ場合" << endl;
  return Node(root_node.state);
}

}  // namespace montecarlo

int main() {
  cout << fixed << setprecision(2);
  MapInitialize map_initialize;

  State state;
  montecarlo::Node root_node(state);

  int turn = 0;
  while (true) {
    ++turn;
    int opp_row, opp_col;
    cin >> opp_row >> opp_col;
    cerr << "opp " << opp_row << " " << opp_col << endl;
    if (opp_row == -1) {
      // 先手の1ターン目
    } else {
      // 相手の手でrootを勧める
      root_node = montecarlo::advane_node(root_node, opp_row * 9 + opp_col);
    }
    cerr << "ボード入力開始" << endl;
    // ボード入力
    cin.ignore();
    int N;
    cin >> N;
    vector<int> rows(N), cols(N);
    cin.ignore();
    rep(i, N) {
      cin >> rows[i] >> cols[i];
      cin.ignore();
    }
    cerr << "ボード入力終了" << endl;

    // デバッグ用のボード出力
    root_node.state.print_board();

    // 初手はど真ん中で固定する
    if (opp_row == -1) {
      root_node = montecarlo::advane_node(root_node, 40);
      cout << 4 << " " << 4 << endl;
      continue;
    }
    // MCTSで手を求める
    auto p = montecarlo::exec_mcts(root_node);
    actionType ans = p.first;
    double win_rate = p.second * 100;

    // 出力
    int row, col;
    if (ans == -1) {
      // 必敗なので適当に打つ
      row = rows[0], col = cols[0];
    } else {
      row = ans / 9, col = ans % 9;
    }

    // rootを進める
    root_node = montecarlo::advane_node(root_node, row * 9 + col);

    // デバッグ用出力
    root_node.state.print_board();
    cout << row << " " << col << " 勝率：" << win_rate << "%" << endl;
  }
}