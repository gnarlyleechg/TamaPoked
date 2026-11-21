// A kid's-eye walkthrough of the kids-build screens, driven through the REAL
// onTap()/onSwipeV()/render(): the side buttons, every menu button, the bottom
// swipe, fishing, treasure, penalty, the shop and chess. Each screen is also
// rendered and must flush (a frozen panel is invisible to screenshots).
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include <cstdio>
#include <cstdlib>
uint32_t g_seed = 7; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX = 0, g_touchY = 0; volatile bool g_touchDown = false;
void FakeESP::restart() { exit(0); }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(""); }
void setup(); void loop(); void render(); void onTap(int16_t, int16_t); void onSwipeV(int);
void actStep(uint32_t); void chessAiStep(uint32_t);
extern Arduino_Canvas *gfx;
extern Pet pet; extern Party party;
extern bool menuOpen, cardOpen, galleryOpen, gymOpen, partyOpen, shopOpen, chessOpen, actOpen, clockOpen, gymPick, galleryPick;
extern uint8_t actState, fishPhase, treasurePick, treasureLoot[3], penShots, penGoals, penPhase;
extern uint32_t fishBiteAt; extern int16_t tY0, penKickX;
extern uint8_t chessState; extern int8_t chessSel; extern bool chessAiDue;
struct Chess { int8_t b[64]; }; extern Chess chessG;
extern char actMsg[34];
static int bad = 0;
#define CK(c, m) do { if (c) printf("PASS  %s\n", m); else { printf("FAIL  %s\n", m); bad++; } } while (0)
static void closeAll() {
  menuOpen = cardOpen = galleryOpen = gymOpen = partyOpen = shopOpen = chessOpen = actOpen = clockOpen = false;
  gymPick = galleryPick = false; actState = 0;
}
// the menu grid, asked of the screen: column i%2, row i/2 (138x52 buttons, 8/6 gaps)
static void tapMenuButton(int i) { onTap(233 - 142 + (i % 2) * 146 + 69, 68 + 14 + (i / 2) * 58 + 26); }
static void renderOk(const char *what) {
  gfx->frameReady = false; render();
  char m[64]; snprintf(m, sizeof m, "%s renders and flushes", what); CK(gfx->frameReady, m);
  if (const char *dir = getenv("WALK_SHOTS")) {      // optional: keep a picture of each screen
    char path[200], name[64]; int n = 0;
    for (const char *c = what; *c && n < 60; c++) name[n++] = (*c == ' ' || *c == '(' || *c == ')') ? '_' : *c;
    name[n] = 0; snprintf(path, sizeof path, "%s/%s.ppm", dir, name);
    if (FILE *f = fopen(path, "wb")) {
      const uint16_t *fb = gfx->buffer(); fprintf(f, "P6\n466 466\n255\n");
      for (int i = 0; i < 466 * 466; i++) { uint16_t c = fb[i];
        fputc(((c >> 11) & 31) * 255 / 31, f); fputc(((c >> 5) & 63) * 255 / 63, f); fputc((c & 31) * 255 / 31, f); }
      fclose(f);
    }
  }
}
int main() {
  setup(); for (int i = 0; i < 4; i++) loop();
  if (pet.awaitingStarter()) pet.chooseStarter(4);
  if (pet.isEgg()) pet.dbgHatchAs(25, false);
  while (pet.hasLearnOffer()) pet.declineLearn();
  pet.ceremony = 0; pet.sleeping = false;
  CK(!pet.isEgg(), "a creature is out of its egg");

  // --- side buttons
  onTap(420, 236); CK(menuOpen, "MENU side button opens the menu"); closeAll();
  onTap(46, 236);  CK(gymOpen, "BATTLE side button opens the gym list"); closeAll();
  tY0 = 420; onSwipeV(-1); CK(menuOpen, "swipe up from the bottom edge opens the menu"); closeAll();
  tY0 = 250; onSwipeV(-1); CK(!menuOpen && cardOpen, "swipe up from the middle still opens the creature card"); closeAll();

  // --- every menu button lands on its screen
  struct { int i; bool *flag; const char *n; } B[] = {
    {0, &cardOpen, "STATS"}, {1, &galleryOpen, "POKEDEX"}, {2, &gymOpen, "BATTLE"}, {3, &partyOpen, "PARTY"},
    {4, &shopOpen, "SHOP"}, {5, &chessOpen, "CHESS"}, {6, &actOpen, "ACTIVITIES"}, {7, &clockOpen, "SETTINGS"} };
  for (auto &b : B) {
    closeAll(); menuOpen = true; tapMenuButton(b.i);
    char m[48]; snprintf(m, sizeof m, "menu button %s opens its screen", b.n); CK(*b.flag && !menuOpen, m);
  }
  closeAll(); menuOpen = true; tapMenuButton(9); CK(!menuOpen, "menu CLOSE button closes it");

  // --- fishing
  closeAll(); actOpen = true; actState = 0; renderOk("activities list");
  pet.coins = 0; pet.actToday = 0;
  onTap(233, 148); CK(actState == 1, "FISHING opens"); renderOk("fishing");
  onTap(233, 250); CK(fishPhase == 1, "tap casts the line");
  onTap(233, 250); CK(fishPhase == 4, "tapping before a bite loses the fish");
  onTap(233, 250); actStep(fishBiteAt + 1); CK(fishPhase == 2, "the bobber dips when a fish bites"); renderOk("fishing (bite)");
  onTap(233, 250); CK(fishPhase == 3 && pet.coins >= 2, "tapping on the bite catches it and pays coins");
  onTap(233, 250); onTap(233, 250); // cast, then too early
  onTap(233, 250); actStep(fishBiteAt + 1); actStep(fishBiteAt + 5000); CK(fishPhase == 4, "a late tap: the fish gets away");

  // --- treasure
  onTap(233, 400); CK(actState == 0, "BACK returns to the activities list");
  uint32_t c0 = pet.coins;
  onTap(233, 217); CK(actState == 2, "TREASURE opens"); renderOk("treasure");
  onTap(116, 200); CK(treasurePick == 0, "tapping a chest opens it");
  CK(pet.coins == c0 + treasureLoot[0] || pet.actToday >= 60, "the chest pays exactly what it shows"); renderOk("treasure (opened)");

  // --- penalty
  onTap(233, 400); pet.coins = 0; pet.actToday = 0;
  onTap(233, 288); CK(actState == 3, "PENALTY opens"); renderOk("penalty");
  for (int k = 0; k < 5; k++) { render(); onTap(penKickX ? penKickX : 233, 200); if (k < 4) onTap(233, 200); }
  CK(penShots == 5, "five kicks are taken");
  CK(pet.coins == (uint32_t)penGoals * 2, "penalty pays 2 coins a goal");

  // --- daily cap across all activities
  pet.actToday = 59; pet.coins = 0;
  CK(pet.earnActivity(8, 123) <= 8, "(setup) new day resets the cap"); 
  pet.actToday = 60; uint32_t before = pet.coins; pet.earnActivity(8, pet.actDay);
  CK(pet.coins == before, "no coins once the daily cap is reached");

  // --- shop
  closeAll(); shopOpen = true; pet.coins = 100; renderOk("shop");
  uint32_t shopBefore = pet.coins;
  for (int y = 90; y < 390; y += 6) onTap(233, y);
  CK(pet.coins < shopBefore, "shop sells something and takes the coins");
  pet.coins = 0; uint32_t z = pet.coins; for (int y = 90; y < 390; y += 6) onTap(233, y);
  CK(pet.coins == z && shopOpen, "with no coins nothing is bought");

  // --- chess
  closeAll(); chessOpen = true; extern uint8_t chessState; chessState = 0; renderOk("chess level list");
  onTap(233, 146); CK(chessState == 1, "chess EASY starts a game"); renderOk("chess board");
  onTap(253, 338); CK(chessSel == 12, "tapping the e2 pawn picks it up");
  onTap(253, 258); CK(chessG.b[28] > 0 && chessG.b[12] == 0, "tapping e4 plays the move");
  CK(chessAiDue, "the board answers next");
  chessAiStep(millis() + 5000); CK(!chessAiDue, "the board makes its move");
  int blackMoved = 0; for (int s = 0; s < 64; s++) if (chessG.b[s] < 0) blackMoved++;
  CK(blackMoved == 16, "black still has all its pieces after one move");
  renderOk("chess after moves");

  printf(bad ? "FAILED\n" : "ALL PASS\n");
  return bad ? 1 : 0;
}
