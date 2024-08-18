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
#pragma GCC target("avx")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

/*
高速化
c 1.0 EXPAND_THRESHOLD = 10 210位
c 1.2 EXPAND_THRESHOLD = 10 226位
c 0.8 EXPAND_THRESHOLD = 10 225位
*/

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
  chrono::high_resolution_clock::time_point start_time_;
  int64_t time_threshold_;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeper(const int64_t& time_threshold)
      : start_time_(chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const {
    auto diff = chrono::high_resolution_clock::now() - start_time_;
    return chrono::duration_cast<chrono::milliseconds>(diff).count() >=
           time_threshold_;
  }
};

// 勝敗判定が入る（0：o勝ち、1：x勝ち、2：引き分け、3：未決着）
// 存在しない盤面（oooxxx...など）も入るが気にしない
const int pow_4_9 = 262144;
vector<int> big_winning_status_map(pow_4_9);
vector<int> small_winning_status_map(pow_4_9);
// 必勝手候補
vector<vector<int>> big_winning_move_map_o(pow_4_9);
vector<vector<int>> big_winning_move_map_x(pow_4_9);
// 必勝手の1つが入る（なければ-1）
vector<int> small_winning_move_map_o(pow_4_9, -1);
vector<int> small_winning_move_map_x(pow_4_9, -1);
// debug用
unordered_map<int, string> bit_to_string_map;
unordered_map<string, int> string_to_bit_map;

class MapInitialize {
 private:
  // winning_status_mapに追加する
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
    if (big_winning_status_map[bit] != 3) return;
    rep(ci, 2) {
      char c = (ci ? 'o' : 'x');
      // 横
      rep(y, 3) {
        if (str[y * 3] == '.' && str[y * 3 + 1] == c && str[y * 3 + 2] == c) {
          if (c == 'o') big_winning_move_map_o[bit].push_back(y * 3);
          if (c == 'x') big_winning_move_map_x[bit].push_back(y * 3);
        }
        if (str[y * 3] == c && str[y * 3 + 1] == '.' && str[y * 3 + 2] == c) {
          if (c == 'o') big_winning_move_map_o[bit].push_back(y * 3 + 1);
          if (c == 'x') big_winning_move_map_x[bit].push_back(y * 3 + 1);
        }
        if (str[y * 3] == c && str[y * 3 + 1] == c && str[y * 3 + 2] == '.') {
          if (c == 'o') big_winning_move_map_o[bit].push_back(y * 3 + 2);
          if (c == 'x') big_winning_move_map_x[bit].push_back(y * 3 + 2);
        }
      }
      // 縦
      rep(x, 3) {
        if (str[x] == '.' && str[x + 3] == c && str[x + 6] == c) {
          if (c == 'o') big_winning_move_map_o[bit].push_back(x);
          if (c == 'x') big_winning_move_map_x[bit].push_back(x);
        }
        if (str[x] == c && str[x + 3] == '.' && str[x + 6] == c) {
          if (c == 'o') big_winning_move_map_o[bit].push_back(x + 3);
          if (c == 'x') big_winning_move_map_x[bit].push_back(x + 3);
        }
        if (str[x] == c && str[x + 3] == c && str[x + 6] == '.') {
          if (c == 'o') big_winning_move_map_o[bit].push_back(x + 6);
          if (c == 'x') big_winning_move_map_x[bit].push_back(x + 6);
        }
      }
      // 斜め
      if (str[0] == '.' && str[4] == c && str[8] == c) {
        if (c == 'o') big_winning_move_map_o[bit].push_back(0);
        if (c == 'x') big_winning_move_map_x[bit].push_back(0);
      }
      if (str[0] == c && str[4] == '.' && str[8] == c) {
        if (c == 'o') big_winning_move_map_o[bit].push_back(4);
        if (c == 'x') big_winning_move_map_x[bit].push_back(4);
      }
      if (str[0] == c && str[4] == c && str[8] == '.') {
        if (c == 'o') big_winning_move_map_o[bit].push_back(8);
        if (c == 'x') big_winning_move_map_x[bit].push_back(8);
      }
      if (str[2] == '.' && str[4] == c && str[6] == c) {
        if (c == 'o') big_winning_move_map_o[bit].push_back(2);
        if (c == 'x') big_winning_move_map_x[bit].push_back(2);
      }
      if (str[2] == c && str[4] == '.' && str[6] == c) {
        if (c == 'o') big_winning_move_map_o[bit].push_back(4);
        if (c == 'x') big_winning_move_map_x[bit].push_back(4);
      }
      if (str[2] == c && str[4] == c && str[6] == '.') {
        if (c == 'o') big_winning_move_map_o[bit].push_back(6);
        if (c == 'x') big_winning_move_map_x[bit].push_back(6);
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

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

using actionType = int;
using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;

class State {
 private:
  inline static const vector<int> action_to_now_big_board = {
      0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0,
      1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 3, 3, 3, 4, 4, 4,
      5, 5, 5, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8,
      6, 6, 6, 7, 7, 7, 8, 8, 8, 6, 6, 6, 7, 7, 7, 8, 8, 8};
  inline static const vector<int> action_to_next_big_board = {
      0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5, 3, 4, 5, 6, 7, 8,
      6, 7, 8, 6, 7, 8, 0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5,
      3, 4, 5, 6, 7, 8, 6, 7, 8, 6, 7, 8, 0, 1, 2, 0, 1, 2, 0, 1, 2,
      3, 4, 5, 3, 4, 5, 3, 4, 5, 6, 7, 8, 6, 7, 8, 6, 7, 8};
  // big boardのindexから、そのbig boardの左上のboard idxを引く
  inline static const vector<int> big_board_to_board_start = {
      0, 3, 6, 27, 30, 33, 54, 57, 60};
  // bit boardへの移行過程
  inline static const vector<int> small_board_start_i_idff = {0,  1,  2,  9, 10,
                                                              11, 18, 19, 20};

 public:
  vector<string> board;
  bool is_x;
  // 次にどのゲームを着手するか
  int big_board_index = -1;
  // ミニゲーム自体の勝敗 (o, x, ., d := draw)
  string big_board;

  State() {
    rep(i, 9) board.push_back(string(9, '.'));
    big_board = string(9, '.');
    is_x = true;
  }

  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  void advance(actionType action) {
    this->big_board_index = action_to_now_big_board[action];

    if (this->is_x) {
      this->board[big_board_index][action_to_next_big_board[action]] = 'x';
    } else {
      this->board[big_board_index][action_to_next_big_board[action]] = 'o';
    }
    this->is_x ^= 1;

    check_small_winning_status();
    this->big_board_index = action_to_next_big_board[action];
    if (big_board[this->big_board_index] != '.') this->big_board_index = -1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<actionType> legal_actions() {
    vector<actionType> ret;

    if (big_board_index == -1) {
      rep(i, 9) {
        if (big_board[i] != '.') continue;
        rep(j, 9) {
          if (board[i][j] == '.') {
            int pos = big_board_to_board_start[i] + small_board_start_i_idff[j];
            ret.push_back(pos);
          }
        }
      }
    } else {
      int start = big_board_to_board_start[big_board_index];
      rep(i, 9) {
        if (board[big_board_index][i] == '.')
          ret.push_back(start + small_board_start_i_idff[i]);
      }
    }
    return ret;
  }

  // スモールボードの勝敗
  void check_small_winning_status() {
    int bit = string_to_bit_map[board[big_board_index]];

    switch (small_winning_status_map[bit]) {
      case 0:
        big_board[big_board_index] = 'o';
        break;
      case 1:
        big_board[big_board_index] = 'x';
        break;
      case 2:
        big_board[big_board_index] = 'd';
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
      for (auto v : big_winning_move_map_x[string_to_bit_map[big_board]]) {
        auto res = find_small_winning_move('x', v);
        if (res != -1) return res;
      }
    } else {
      for (auto v : big_winning_move_map_o[string_to_bit_map[big_board]]) {
        auto res = find_small_winning_move('o', v);
        if (res != -1) return res;
      }
    }
    return -1;
  }
  actionType find_small_winning_move(char c, int board_idx) {
    return (
        c == 'x'
            ? small_winning_move_map_x[string_to_bit_map[board[board_idx]]]
            : small_winning_move_map_o[string_to_bit_map[board[board_idx]]]);
  }

  // 勝敗情報を取得する
  WinningStatus get_winning_status() {
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

  bool is_done() { return get_winning_status() != WinningStatus::NONE; }

  void print_board() {
    rep(i, 9) {
      rep(j, 9) {
        cerr << this->board[action_to_now_big_board[i * 9 + j]]
                           [action_to_next_big_board[i * 9 + j]];
      }
      cerr << endl;
    }
    rep(i, 3) {
      rep(j, 3) cerr << this->big_board[i * 3 + j];
      cerr << endl;
    }
    cerr << "big_board_index: " << big_board_index << endl;
  }
};

namespace montecarlo {

double playout(State& state) {
  // 既に決着しているならそれを返す
  WinningStatus winningStatus = state.get_winning_status();
  if (winningStatus == WinningStatus::WIN) return 1;
  if (winningStatus == WinningStatus::LOSE) return 0;
  if (winningStatus == WinningStatus::DRAW) return 0.5;
  // 必勝手があれば終わり
  if (state.find_winning_move() != -1) return 1;

  // 合法手を取得
  auto legal_actions = state.legal_actions();
  // 置く場所がない時はtic toc toeでは引き分け
  if (legal_actions.empty()) return 0;

  // ランダムプレイ
  actionType action = legal_actions[randXor() % (legal_actions.size())];
  state.advance(action);
  return 1 - playout(state);
}

constexpr const double C = 1.0;             // UCB1の計算に使う定数
constexpr const int EXPAND_THRESHOLD = 15;  // ノードを展開する閾値

// MCTSの計算に使うノード
class Node {
 private:
  State state;

 public:
  std::vector<Node> child_nodes;
  double win_count;  // 累計価値
  int visit_num;  // 試行回数

  Node(const State& arg_state) : state(arg_state), win_count(0), visit_num(0) {}

  // ノードの評価を行う
  double evaluate() {
    if (state.is_done()) {
      double value;
      switch (state.get_winning_status()) {
        case (WinningStatus::WIN):
          value = 1;
          break;
        case (WinningStatus::LOSE):
          value = 0;
          break;
        default:
          value = 0.5;
          break;
      }
      win_count += value;
      visit_num++;
      return value;
    }
    // 葉ノードの場合
    if (child_nodes.empty()) {
      State next_state = state;
      double value = playout(next_state);
      win_count += value;
      visit_num++;
      // 閾値回試行したらノードを展開する
      if (visit_num == EXPAND_THRESHOLD) expand();

      return value;
    } else {
      // 葉ノード以外では子ノードの値
      double value = 1 - nextChildNode().evaluate();
      win_count += value;
      visit_num++;
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
      if (child_node.visit_num == 0) return child_node;
    }
    int sum_n = 0;  // 全ノードの試行回数の総和
    for (const auto& child_node : child_nodes) sum_n += child_node.visit_num;

    double best_value = -INF;
    int best_idx = -1;
    rep(i, child_nodes.size()) {
      const auto& child_node = child_nodes[i];
      double ucb1_value =
          1. - child_node.win_count / child_node.visit_num +
          (double)C * std::sqrt(2. * std::log(sum_n) / child_node.visit_num);
      if (ucb1_value > best_value) {
        best_idx = i;
        best_value = ucb1_value;
      }
    }
    return child_nodes[best_idx];
  }
};

// 制限時間(ms)を指定してMCTSで行動を決定する
pair<actionType, double> exec_mcts(State& state,
                                   const int64_t time_threshold = 90) {
  Node root_node = Node(state);
  root_node.expand();
  auto time_keeper = TimeKeeper(time_threshold);
  int turn;
  for (turn = 0;; turn++) {
    if (time_keeper.isTimeOver()) break;
    root_node.evaluate();
  }
  vector<actionType> legal_actions = state.legal_actions();

  cerr << "turn: " << turn << endl;

  int idx = 0, max_score = -1e9;
  rep(i, legal_actions.size()) {
    int n = root_node.child_nodes[i].visit_num;
    cerr << legal_actions[i] << " " << n << endl;
    if (chmax(max_score, n)) idx = i;
  }
  double win_rate =
      (max_score - root_node.child_nodes[idx].win_count) / max_score;

  return make_pair(legal_actions[idx], win_rate);
}

}  // namespace montecarlo

// using namespace chrono;
int main() {
  // auto startClock = system_clock::now();
  MapInitialize map_initialize;
  // double time =
  //     ((double)duration_cast<microseconds>(system_clock::now() - startClock)
  //          .count() *
  //      1e-6);
  // cerr << time << endl;

  State state;
  cout << fixed << setprecision(2);

  while (1) {
    int opp_row;
    int opp_col;
    cin >> opp_row >> opp_col;
    if (opp_row == -1) {
      // 自分が先手
    } else {
      state.advance(opp_row * 9 + opp_col);
    }
    cin.ignore();
    int N;
    cin >> N;
    vector<int> rows(N), cols(N);
    cin.ignore();
    rep(i, N) {
      cin >> rows[i] >> cols[i];
      cin.ignore();
    }

    state.print_board();

    // 初手はさすがにど真ん中でいい
    if (opp_row == -1) {
      cout << 4 << " " << 4 << endl;
      state.advance(40);
      continue;
    }
    auto p = montecarlo::exec_mcts(state);
    actionType ans = p.first;
    double win_rate = p.second * 100;
    int row, col;
    if (ans == -1) {
      row = rows[0], col = cols[0];
    } else {
      row = ans / 9, col = ans % 9;
    }
    state.advance(row * 9 + col);
    state.print_board();
    cout << row << " " << col << " 勝率：" << win_rate << "%" << endl;
  }
}