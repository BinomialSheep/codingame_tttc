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
Wood用。
単純なminimax。
*/

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

using actionType = int;

class State {
 private:
 public:
  string board;
  bool is_x;

  State() {
    board = string(9, '.');
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
    is_x ^= 1;
  }
  // 着手を戻す
  void retrive(actionType action) {
    this->board[action] = '.';
    is_x ^= 1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<actionType> legal_actions() {
    vector<actionType> ret;
    rep(i, board.size()) if (board[i] == '.') ret.push_back(i);
    return ret;
  }

  // 勝敗情報を取得する
  WinningStatus get_winning_status() {
    // 3連続しているか
    rep(i, 3) {
      if (board[i * 3] == board[i * 3 + 1] &&
          board[i * 3] == board[i * 3 + 2]) {
        if (board[i * 3] == 'o') {
          return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
        }
        if (board[i * 3] == 'x') {
          return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
        }
      }
    }
    rep(j, 3) {
      if (board[j] == board[j + 3] && board[j + 3] == board[j + 6]) {
        if (board[j] == 'o') {
          return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
        }
        if (board[j] == 'x') {
          return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
        }
      }
    }
    if (board[0] == board[4] && board[4] == board[8]) {
      if (board[4] == 'o') {
        return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
      }
      if (board[4] == 'x') {
        return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
      }
    }
    if (board[2] == board[4] && board[4] == board[6]) {
      if (board[4] == 'o') {
        return (is_x ? WinningStatus::LOSE : WinningStatus::WIN);
      }
      if (board[4] == 'x') {
        return (is_x ? WinningStatus::WIN : WinningStatus::LOSE);
      }
    }
    return WinningStatus::NONE;
  }

  void print_board() {
    rep(i, 3) {
      rep(j, 3) cerr << board[i * 3 + j];
      cerr << endl;
    }
  }
};

namespace minimax {
int mini_max_score(State& state) {
  // 既に決着しているならそれを返す
  WinningStatus winningStatus = state.get_winning_status();
  if (winningStatus == WinningStatus::WIN) return 1;
  if (winningStatus == WinningStatus::LOSE) return -1;
  if (winningStatus == WinningStatus::DRAW) return 0;
  // 合法手を取得
  auto legal_actions = state.legal_actions();
  // 置く場所がない時はtic toc toeでは引き分け
  if (legal_actions.empty()) return 0;

  int best_score = -2;

  for (auto action : legal_actions) {
    state.advance(action);
    int score = -mini_max_score(state);
    state.retrive(action);
    chmax(best_score, score);
  }
  return best_score;
}

actionType exec_minimax(State& state) {
  actionType best_action = -1;
  int best_score = -2;

  auto legal_actions = state.legal_actions();
  for (auto action : legal_actions) {
    state.advance(action);
    int score = -mini_max_score(state);
    state.retrive(action);
    cerr << action << " " << score << endl;
    if (chmax(best_score, score)) best_action = action;
  }

  return best_action;
}

}  // namespace minimax

int main() {
  State state;

  while (1) {
    int opp_row;
    int opp_col;
    cin >> opp_row >> opp_col;
    if (opp_row == -1) {
      // 自分が先手
    } else {
      state.advance(opp_row * 3 + opp_col);
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

    actionType ans = minimax::exec_minimax(state);
    int row, col;
    if (ans == -1) {
      row = rows[0], col = cols[0];
    } else {
      row = ans / 3, col = ans % 3;
    }
    state.advance(row * 3 + col);

    cout << row << " " << col << endl;
  }
}