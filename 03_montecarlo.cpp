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

/*
原子モンテカルロ対応。
620/8940位。
最初のプレイアウト800回弱

TODO：
- randの高速化
- 3連結の高速化
- 1手必勝判定
- MCTS化

*/

mt19937 mt_for_action(0);

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
    auto diff = chrono::high_resolution_clock::now() - this->start_time_;
    return chrono::duration_cast<chrono::milliseconds>(diff).count() >=
           time_threshold_;
  }
};

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

using actionType = int;

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

 public:
  string board;
  bool is_x;
  // 次にどのゲームを着手するか
  int big_board_index = -1;
  // ミニゲーム自体の勝敗 (o, x, ., d := draw)
  string big_board;

  State() {
    board = string(81, '.');
    big_board = string(9, '.');
    is_x = true;
  }

  State(string _board, bool _is_x) : board(_board), is_x(_is_x) {};

  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  void advance(actionType action) {
    if (this->is_x) {
      this->board[action] = 'x';
    } else {
      this->board[action] = 'o';
    }
    this->is_x ^= 1;

    this->big_board_index = action_to_now_big_board[action];
    check_small_winning_status();
    this->big_board_index = action_to_next_big_board[action];
    if (big_board[this->big_board_index] != '.') this->big_board_index = -1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<actionType> legal_actions() {
    vector<actionType> ret;

    if (big_board_index == -1) {
      rep(i, 81) {
        if (big_board[action_to_now_big_board[i]] == '.' && board[i] == '.') {
          ret.push_back(i);
        }
      }
    } else {
      int start = big_board_to_board_start[big_board_index];
      rep(i, 3) rep(j, 3) {
        if (board[start + i * 9 + j] == '.') ret.push_back(start + i * 9 + j);
      }
    }

    return ret;
  }

  // スモールボードの勝敗
  void check_small_winning_status() {
    int start = big_board_to_board_start[big_board_index];

    // 3連続しているか
    rep(i, 3) {
      if (board[start + i * 9] == board[start + i * 9 + 1] &&
          board[start + i * 9] == board[start + i * 9 + 2]) {
        if (board[start + i * 9] == 'o') big_board[big_board_index] = 'o';
        if (board[start + i * 9] == 'x') big_board[big_board_index] = 'x';
      }
    }
    rep(j, 3) {
      if (board[start + j] == board[start + j + 9] &&
          board[start + j] == board[start + j + 18]) {
        if (board[start + j] == 'o') big_board[big_board_index] = 'o';
        if (board[start + j] == 'x') big_board[big_board_index] = 'x';
      }
    }
    if (board[start] == board[start + 10] &&
        board[start + 10] == board[start + 20]) {
      if (board[start + 10] == 'o') big_board[big_board_index] = 'o';
      if (board[start + 10] == 'x') big_board[big_board_index] = 'x';
    }
    if (board[start + 2] == board[start + 10] &&
        board[start + 10] == board[start + 18]) {
      if (board[start + 10] == 'o') big_board[big_board_index] = 'o';
      if (board[start + 10] == 'x') big_board[big_board_index] = 'x';
    }
    rep(i, 3) rep(j, 3) if (board[start + 9 * i + j] == '.') return;
    big_board[big_board_index] = 'd';
  }

  // 勝敗情報を取得する
  WinningStatus get_winning_status() {
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

  void print_board() {
    rep(i, 9) {
      rep(j, 9) cerr << this->board[i * 9 + j];
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

int playout(State& state) {
  // 既に決着しているならそれを返す
  WinningStatus winningStatus = state.get_winning_status();
  if (winningStatus == WinningStatus::WIN) return 1;
  if (winningStatus == WinningStatus::LOSE) return -1;
  if (winningStatus == WinningStatus::DRAW) return 0;
  // 合法手を取得
  auto legal_actions = state.legal_actions();
  // 置く場所がない時はtic toc toeでは引き分け
  if (legal_actions.empty()) return 0;

  // ランダムプレイ
  actionType action = legal_actions[mt_for_action() % (legal_actions.size())];
  state.advance(action);
  return -playout(state);
}

actionType exec_montecalro(State& state, const int64_t time_threshold = 90) {
  auto time_keeper = TimeKeeper(time_threshold);
  int turn = 0;
  int cnt = 0;

  vector<actionType> legal_actions = state.legal_actions();
  vector<int> actions_score(legal_actions.size());

  for (;; turn++) {
    if (time_keeper.isTimeOver()) break;
    rep(i, legal_actions.size()) {
      cnt++;
      State new_state = state;
      new_state.advance(legal_actions[i]);
      actions_score[i] -= playout(new_state);
    }
  }
  cerr << "cnt: " << cnt << endl;
  rep(i, legal_actions.size()) {
    cerr << legal_actions[i] << " " << actions_score[i] << endl;
  }

  int idx = 0, max_score = -1e9;
  rep(i, legal_actions.size()) if (chmax(max_score, actions_score[i])) idx = i;
  return legal_actions[idx];
}

}  // namespace montecarlo

int main() {
  State state;

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

    actionType ans = montecarlo::exec_montecalro(state);
    int row, col;
    if (ans == -1) {
      row = rows[0], col = cols[0];
    } else {
      row = ans / 9, col = ans % 9;
    }
    state.advance(row * 9 + col);
    state.print_board();

    cout << row << " " << col << endl;
  }
}