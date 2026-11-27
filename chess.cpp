#include "chess.h"
#include <string.h>

static const int8_t KN_D[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
static const int8_t KG_D[8][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
static const int PIECE_VAL[7] = {0, 100, 320, 330, 500, 900, 0};

static inline int sqOf(int r, int f) { return r * 8 + f; }
static inline bool inb(int r, int f) { return r >= 0 && r < 8 && f >= 0 && f < 8; }

void Chess::reset() {
  memset(b, 0, sizeof(b));
  static const int8_t back[8] = {CH_R, CH_N, CH_B, CH_Q, CH_K, CH_B, CH_N, CH_R};
  for (int f = 0; f < 8; f++) {
    b[sqOf(0, f)] = back[f];
    b[sqOf(1, f)] = CH_P;
    b[sqOf(6, f)] = -CH_P;
    b[sqOf(7, f)] = -back[f];
  }
  whiteToMove = true;
  castle = 15;
  ep = -1;
  halfmove = 0;
  hasLast = false;
  last = ChessMove();
}

bool Chess::attacked(int sq, bool byWhite) const {
  int r = sq / 8, f = sq % 8;
  int s = byWhite ? 1 : -1;
  // pawns: a white pawn attacks up the board, so it sits one rank BELOW the target
  int pr = r - s;
  for (int df = -1; df <= 1; df += 2)
    if (inb(pr, f + df) && b[sqOf(pr, f + df)] == s * CH_P) return true;
  for (auto &d : KN_D)
    if (inb(r + d[0], f + d[1]) && b[sqOf(r + d[0], f + d[1])] == s * CH_N) return true;
  for (auto &d : KG_D)
    if (inb(r + d[0], f + d[1]) && b[sqOf(r + d[0], f + d[1])] == s * CH_K) return true;
  for (int k = 0; k < 8; k++) {
    int dr = KG_D[k][0], df = KG_D[k][1];
    bool diag = (dr != 0 && df != 0);
    for (int rr = r + dr, ff = f + df; inb(rr, ff); rr += dr, ff += df) {
      int8_t p = b[sqOf(rr, ff)];
      if (!p) continue;
      if (p * s > 0) {
        int t = p * s;
        if (t == CH_Q || (diag ? t == CH_B : t == CH_R)) return true;
      }
      break;
    }
  }
  return false;
}

bool Chess::inCheck(bool white) const {
  int8_t k = white ? CH_K : -CH_K;
  for (int i = 0; i < 64; i++)
    if (b[i] == k) return attacked(i, !white);
  return false;
}

static void addMove(ChessMove *out, int &n, int from, int to, int promo = 0) {
  out[n].from = (uint8_t)from; out[n].to = (uint8_t)to; out[n].promo = (uint8_t)promo; n++;
}

int Chess::pseudo(ChessMove *out) const {
  int n = 0, s = whiteToMove ? 1 : -1;
  for (int sq = 0; sq < 64; sq++) {
    int8_t p = b[sq];
    if (!p || (p > 0) != whiteToMove) continue;
    int t = p * s, r = sq / 8, f = sq % 8;
    if (t == CH_P) {
      int nr = r + s;
      bool promo = (nr == 7 || nr == 0);
      if (inb(nr, f) && !b[sqOf(nr, f)]) {
        if (promo) for (int pp = CH_Q; pp >= CH_N; pp--) addMove(out, n, sq, sqOf(nr, f), pp);
        else {
          addMove(out, n, sq, sqOf(nr, f));
          int start = whiteToMove ? 1 : 6;
          if (r == start && !b[sqOf(r + 2 * s, f)]) addMove(out, n, sq, sqOf(r + 2 * s, f));
        }
      }
      for (int df = -1; df <= 1; df += 2) {
        if (!inb(nr, f + df)) continue;
        int to = sqOf(nr, f + df);
        if ((b[to] && (b[to] > 0) != whiteToMove) || to == ep) {
          if (promo) for (int pp = CH_Q; pp >= CH_N; pp--) addMove(out, n, sq, to, pp);
          else addMove(out, n, sq, to);
        }
      }
    } else if (t == CH_N || t == CH_K) {
      const int8_t (*D)[2] = (t == CH_N) ? KN_D : KG_D;
      for (int k = 0; k < 8; k++) {
        int rr = r + D[k][0], ff = f + D[k][1];
        if (!inb(rr, ff)) continue;
        int8_t q = b[sqOf(rr, ff)];
        if (!q || (q > 0) != whiteToMove) addMove(out, n, sq, sqOf(rr, ff));
      }
      if (t == CH_K) {
        int home = whiteToMove ? 0 : 7;
        if (r == home && f == 4 && !attacked(sq, !whiteToMove)) {
          uint8_t ks = whiteToMove ? 1 : 4, qs = whiteToMove ? 2 : 8;
          if ((castle & ks) && !b[sqOf(home, 5)] && !b[sqOf(home, 6)] &&
              b[sqOf(home, 7)] == s * CH_R &&
              !attacked(sqOf(home, 5), !whiteToMove) && !attacked(sqOf(home, 6), !whiteToMove))
            addMove(out, n, sq, sqOf(home, 6));
          if ((castle & qs) && !b[sqOf(home, 3)] && !b[sqOf(home, 2)] && !b[sqOf(home, 1)] &&
              b[sqOf(home, 0)] == s * CH_R &&
              !attacked(sqOf(home, 3), !whiteToMove) && !attacked(sqOf(home, 2), !whiteToMove))
            addMove(out, n, sq, sqOf(home, 2));
        }
      }
    } else {
      for (int k = 0; k < 8; k++) {
        int dr = KG_D[k][0], df = KG_D[k][1];
        bool diag = (dr != 0 && df != 0);
        if (t == CH_B && !diag) continue;
        if (t == CH_R && diag) continue;
        for (int rr = r + dr, ff = f + df; inb(rr, ff); rr += dr, ff += df) {
          int8_t q = b[sqOf(rr, ff)];
          if (!q) { addMove(out, n, sq, sqOf(rr, ff)); continue; }
          if ((q > 0) != whiteToMove) addMove(out, n, sq, sqOf(rr, ff));
          break;
        }
      }
    }
  }
  return n;
}

void Chess::apply(const ChessMove &m) {
  int8_t p = b[m.from];
  int t = p > 0 ? p : -p;
  bool white = p > 0;
  bool capture = b[m.to] != 0;
  // en passant: the captured pawn is beside the mover, not on the target square
  if (t == CH_P && m.to == ep && !b[m.to] && (m.from % 8) != (m.to % 8)) {
    b[(m.from / 8) * 8 + (m.to % 8)] = 0;
    capture = true;
  }
  // castling: bring the rook across
  if (t == CH_K && (m.to % 8) - (m.from % 8) == 2) { b[m.from + 1] = b[m.from + 3]; b[m.from + 3] = 0; }
  if (t == CH_K && (m.from % 8) - (m.to % 8) == 2) { b[m.from - 1] = b[m.from - 4]; b[m.from - 4] = 0; }
  // rights
  if (t == CH_K) castle &= white ? ~3 : ~12;
  if (m.from == 0 || m.to == 0) castle &= ~2;
  if (m.from == 7 || m.to == 7) castle &= ~1;
  if (m.from == 56 || m.to == 56) castle &= ~8;
  if (m.from == 63 || m.to == 63) castle &= ~4;
  ep = -1;
  if (t == CH_P && (m.to - m.from == 16 || m.from - m.to == 16)) ep = (int8_t)((m.from + m.to) / 2);
  b[m.to] = m.promo ? (white ? m.promo : -m.promo) : p;
  b[m.from] = 0;
  halfmove = (t == CH_P || capture) ? 0 : (uint8_t)(halfmove < 255 ? halfmove + 1 : 255);
  whiteToMove = !whiteToMove;
  last = m;
  hasLast = true;
}

int Chess::legalMoves(ChessMove *out) const {
  ChessMove tmp[256];
  int n = pseudo(tmp), k = 0;
  for (int i = 0; i < n; i++) {
    Chess c = *this;
    c.apply(tmp[i]);
    if (!c.inCheck(whiteToMove)) out[k++] = tmp[i];
  }
  return k;
}

int Chess::movesFrom(int sq, ChessMove *out) const {
  ChessMove all[256];
  int n = legalMoves(all), k = 0;
  for (int i = 0; i < n; i++) if (all[i].from == sq) out[k++] = all[i];
  return k;
}

bool Chess::play(int from, int to, int promo) {
  ChessMove all[256];
  int n = legalMoves(all);
  for (int i = 0; i < n; i++) {
    if (all[i].from != from || all[i].to != to) continue;
    if (all[i].promo && all[i].promo != (promo ? promo : CH_Q)) continue;
    apply(all[i]);
    return true;
  }
  return false;
}

ChessResult Chess::status() const {
  ChessMove m[256];
  if (legalMoves(m) == 0)
    return inCheck(whiteToMove) ? (whiteToMove ? CHR_BLACK_WINS : CHR_WHITE_WINS) : CHR_STALEMATE;
  if (halfmove >= 100) return CHR_DRAW;
  int minors = 0, others = 0;
  for (int i = 0; i < 64; i++) {
    int t = b[i] > 0 ? b[i] : -b[i];
    if (t == CH_N || t == CH_B) minors++;
    else if (t == CH_P || t == CH_R || t == CH_Q) others++;
  }
  if (!others && minors <= 1) return CHR_DRAW;   // K vs K, or K and one minor piece
  return CHR_PLAYING;
}

uint64_t Chess::perft(int depth) const {
  if (depth == 0) return 1;
  ChessMove m[256];
  int n = legalMoves(m);
  if (depth == 1) return (uint64_t)n;
  uint64_t total = 0;
  for (int i = 0; i < n; i++) { Chess c = *this; c.apply(m[i]); total += c.perft(depth - 1); }
  return total;
}

// ---- the opponent ----
static const int8_t PST_PAWN[8] = {0, 0, 5, 10, 20, 40, 80, 0};   // advancement by rank (white's view)

int Chess::eval() const {
  int score = 0;
  for (int i = 0; i < 64; i++) {
    int8_t p = b[i];
    if (!p) continue;
    int t = p > 0 ? p : -p, r = i / 8, f = i % 8;
    int v = PIECE_VAL[t];
    if (t == CH_P) v += PST_PAWN[p > 0 ? r : 7 - r];
    else if (t == CH_N || t == CH_B) { int cr = r < 4 ? r : 7 - r, cf = f < 4 ? f : 7 - f; v += (cr + cf) * 4; }
    score += p > 0 ? v : -v;
  }
  return whiteToMove ? score : -score;   // from the side to move's point of view
}

int Chess::search(int depth, int alpha, int beta, int ply) const {
  if (depth == 0) return eval();
  ChessMove m[256];
  int n = legalMoves(m);
  if (!n) return inCheck(whiteToMove) ? -30000 + ply : 0;
  // captures first: makes the cut-offs bite
  int order[256], key[256];
  for (int i = 0; i < n; i++) {
    int8_t v = b[m[i].to];
    key[i] = (v ? 10 * PIECE_VAL[v > 0 ? v : -v] - PIECE_VAL[b[m[i].from] > 0 ? b[m[i].from] : -b[m[i].from]] / 10 : 0) + (m[i].promo ? 800 : 0);
    order[i] = i;
  }
  for (int i = 1; i < n; i++) {
    int o = order[i], j = i - 1;
    while (j >= 0 && key[order[j]] < key[o]) { order[j + 1] = order[j]; j--; }
    order[j + 1] = o;
  }
  int best = -32000;
  for (int k = 0; k < n; k++) {
    Chess c = *this;
    c.apply(m[order[k]]);
    int sc = -c.search(depth - 1, -beta, -alpha, ply + 1);
    if (sc > best) best = sc;
    if (best > alpha) alpha = best;
    if (alpha >= beta) break;
  }
  return best;
}

ChessMove Chess::aiMove(int level, uint32_t seed) const {
  ChessMove m[256];
  int n = legalMoves(m);
  ChessMove none;
  if (!n) return none;
  // Easy rates its move without looking ahead to the player's reply.
  int depth = level <= 1 ? 1 : (level == 2 ? 3 : 4);
  int noise = level <= 1 ? 200 : (level == 2 ? 25 : 0);
  uint32_t x = seed ? seed : 0x9E3779B9u;
  int bestScore = -32001, bestIdx = 0;
  for (int i = 0; i < n; i++) {
    Chess c = *this;
    c.apply(m[i]);
    int sc = -c.search(depth - 1, -32000, 32000, 1);
    if (noise) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; sc += (int)(x % (2 * noise + 1)) - noise; }
    if (sc > bestScore) { bestScore = sc; bestIdx = i; }
  }
  return m[bestIdx];
}
