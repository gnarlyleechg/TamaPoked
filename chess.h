#pragma once
// A small, dependency-free chess engine for the kids build: full legal move
// generation (castling, en passant, promotion), game-end detection and a
// shallow alpha-beta opponent. No Arduino symbols, so the host tests can run
// perft against known node counts.
#include <stdint.h>

enum : int8_t { CH_EMPTY = 0, CH_P = 1, CH_N, CH_B, CH_R, CH_Q, CH_K };   // white = +, black = -

struct ChessMove {
  uint8_t from = 0, to = 0, promo = 0;   // promo: 0 none, else CH_N..CH_Q
};

enum ChessResult : uint8_t { CHR_PLAYING = 0, CHR_WHITE_WINS, CHR_BLACK_WINS, CHR_STALEMATE, CHR_DRAW };

struct Chess {
  int8_t b[64];                // sq = rank * 8 + file; rank 0 is white's back rank
  bool whiteToMove;
  uint8_t castle;              // 1 white king side, 2 white queen side, 4 black king side, 8 black queen side
  int8_t ep;                   // en-passant target square, or -1
  uint8_t halfmove;            // plies since a pawn move or capture
  ChessMove last;              // the move just played (for highlighting)
  bool hasLast;

  void reset();
  bool attacked(int sq, bool byWhite) const;
  bool inCheck(bool white) const;
  int legalMoves(ChessMove *out) const;            // fills up to 256, returns the count
  int movesFrom(int sq, ChessMove *out) const;     // legal moves of the piece on sq
  bool play(int from, int to, int promo = 0);      // legal moves only; false if refused
  ChessResult status() const;
  ChessMove aiMove(int level, uint32_t seed) const;   // level 1 easy .. 3 hard
  void apply(const ChessMove &m);                  // unchecked: used by search and tests
  uint64_t perft(int depth) const;
 private:
  int pseudo(ChessMove *out) const;
  int search(int depth, int alpha, int beta, int ply) const;
  int eval() const;
};
