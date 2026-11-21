// The chess engine against known perft node counts (the standard way to prove a
// move generator), plus the rules a kid will actually run into.
#include "chess.h"
#include <cstdio>
#include <cstring>
static int bad = 0;
static void ck(bool ok, const char *w) { printf("%s  %s\n", ok ? "PASS" : "FAIL", w); if (!ok) bad++; }
static int sq(const char *s) { return (s[1] - '1') * 8 + (s[0] - 'a'); }

// minimal FEN loader (board, side, castling, en passant)
static void fen(Chess &c, const char *f) {
  memset(c.b, 0, sizeof(c.b));
  int r = 7, file = 0;
  const char *p = f;
  for (; *p && *p != ' '; p++) {
    if (*p == '/') { r--; file = 0; }
    else if (*p >= '1' && *p <= '8') file += *p - '0';
    else {
      const char *pcs = "pnbrqk";
      char lo = *p | 0x20;
      int t = (int)(strchr(pcs, lo) - pcs) + 1;
      c.b[r * 8 + file++] = (*p >= 'a') ? -t : t;
    }
  }
  p++;
  c.whiteToMove = (*p == 'w'); p += 2;
  c.castle = 0;
  for (; *p && *p != ' '; p++) c.castle |= (*p == 'K') ? 1 : (*p == 'Q') ? 2 : (*p == 'k') ? 4 : (*p == 'q') ? 8 : 0;
  p++;
  c.ep = (*p == '-') ? -1 : (int8_t)sq(p);
  c.halfmove = 0; c.hasLast = false;
}

int main() {
  Chess c; c.reset();
  ck(c.perft(1) == 20, "start position: 20 moves");
  ck(c.perft(2) == 400, "perft 2 = 400");
  ck(c.perft(3) == 8902, "perft 3 = 8902");
  ck(c.perft(4) == 197281, "perft 4 = 197281");

  Chess k; fen(k, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
  ck(k.perft(1) == 48, "Kiwipete perft 1 = 48 (castling, pins, ep)");
  ck(k.perft(2) == 2039, "Kiwipete perft 2 = 2039");
  ck(k.perft(3) == 97862, "Kiwipete perft 3 = 97862");

  Chess e; fen(e, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
  ck(e.perft(4) == 43238, "endgame position perft 4 = 43238 (en passant, promotion edge cases)");

  Chess pr; fen(pr, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - -");
  ck(pr.perft(1) == 24 && pr.perft(2) == 496 && pr.perft(3) == 9483, "promotion position perft 1-3 = 24, 496, 9483");

  // the rules a kid meets
  Chess g; g.reset();
  ck(!g.play(sq("e2"), sq("e5")), "a pawn cannot jump three squares");
  ck(g.play(sq("e2"), sq("e4")), "e4 is legal");
  ck(!g.play(sq("e4"), sq("e5")), "and it is black's turn now");
  Chess m; m.reset();
  m.play(sq("f2"), sq("f3")); m.play(sq("e7"), sq("e5"));
  m.play(sq("g2"), sq("g4")); m.play(sq("d8"), sq("h4"));
  ck(m.status() == CHR_BLACK_WINS, "fool's mate is detected as a win for black");
  Chess sm; fen(sm, "7k/5Q2/6K1/8/8/8/8/8 b - -");
  ck(sm.status() == CHR_STALEMATE, "stalemate is a draw, not a win");
  Chess ca; fen(ca, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq -");
  ck(ca.play(sq("e1"), sq("g1")) && ca.b[sq("f1")] == CH_R, "castling moves the rook too");
  Chess ep; fen(ep, "4k3/8/8/3pP3/8/8/8/4K3 w - d6");
  ck(ep.play(sq("e5"), sq("d6")) && ep.b[sq("d5")] == 0, "en passant removes the passed pawn");
  Chess pm; fen(pm, "8/P7/8/8/8/8/8/k6K w - -");
  ck(pm.play(sq("a7"), sq("a8")) && pm.b[sq("a8")] == CH_Q, "a pawn reaching the end becomes a queen");
  Chess dk; fen(dk, "8/8/8/8/8/8/8/k6K w - -");
  ck(dk.status() == CHR_DRAW, "king against king is a draw");

  // the opponent never makes an illegal move and takes a free queen
  Chess a; a.reset();
  bool legal = true;
  for (int i = 0; i < 40 && a.status() == CHR_PLAYING; i++) {
    ChessMove mv = a.aiMove(1 + i % 3, 1234 + i);
    ChessMove all[256]; int n = a.legalMoves(all); bool found = false;
    for (int j = 0; j < n; j++) if (all[j].from == mv.from && all[j].to == mv.to && all[j].promo == mv.promo) found = true;
    if (!found) legal = false;
    a.apply(mv);
  }
  ck(legal, "the AI only plays legal moves over a 40-ply game at all levels");
  Chess q; fen(q, "4k3/8/8/3q4/8/8/8/3RK3 w - -");
  ChessMove t = q.aiMove(3, 1);
  ck(t.from == sq("d1") && t.to == sq("d5"), "hard level takes a free queen");

  printf("%s\n", bad ? "FAILED" : "ALL PASS");
  return bad ? 1 : 0;
}
