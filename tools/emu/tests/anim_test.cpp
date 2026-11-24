// Battle animation state: a hit must flag the right side, the HP bar must ease
// rather than snap, and none of it may change the actual battle result.
#include "Arduino.h"
#include "Arduino_GFX_Library.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include "battle.h"
#include "moves.h"
#include <cstdio>
uint32_t g_seed=31; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX=0,g_touchY=0; volatile bool g_touchDown=false;
void FakeESP::restart(){exit(0);}
int FakeSerial::available(){return 0;}
String FakeSerial::readStringUntil(char){return String("");}
void setup(); void render();
extern Pet pet;
extern Arduino_Canvas *gfx;
extern uint8_t btlMenu;
extern bool battleOpen, btlOver;
extern Combatant btlYou, btlFoe;
extern uint8_t btlMsgCount;
extern uint32_t btlHitUntil[2], btlLungeUntil[2];
extern uint16_t btlHpShown[2];
void startBattle(int16_t dex, uint8_t lvl);
void battleTap(int16_t x, int16_t y);
void emuSetCaptureTime(uint32_t now);

static int bad=0;
static void ck(bool ok,const char*w){printf("%s  %s\n",ok?"PASS":"FAIL",w); if(!ok)bad++;}

int main(){
  setup(); for(int i=0;i<4;i++) render();
  if (pet.awaitingStarter()) pet.chooseStarter(4);
  if (pet.isEgg()) pet.dbgHatchAs(6,false);
  pet.ageMinutes = 50UL*MINUTES_PER_LEVEL;
  pet.relearnFromLevel();
  while (pet.hasLearnOffer()) pet.declineLearn();

  startBattle(9, 50);
  ck(battleOpen, "battle starts");
  ck(btlHpShown[0]==btlYou.maxHp && btlHpShown[1]==btlFoe.maxHp,
     "the eased bars start full");

  // Read the actual rendered framebuffer, not a separate approximation of
  // the arena painter. These unobstructed coordinates lie on the round panel.
  gfx->frameReady = false;
  render();
  ck(gfx->frameReady, "battle visual frame flushes");
  const uint16_t *frame = gfx->buffer();
  const uint16_t farFloor = frame[267 * 466 + 233];
  const uint16_t nearFloor = frame[400 * 466 + 233];
  ck(farFloor != nearFloor, "arena ground is shaded instead of one flat color");
  ck(((nearFloor >> 11) & 31) > ((farFloor >> 11) & 31),
     "foreground ground is brighter than distant ground");
  ck(frame[254 * 466 + 233] != farFloor,
     "horizon separator is visible");
  ck(frame[279 * 466 + 120] == RGB565_WHITE,
     "FIGHT control retains its highlight");

  // The added rendering must not silently change the actual combatant IDs
  // or turn/menu state. The normal HP animation checks below cover HP easing.
  const int16_t youDex = btlYou.dex, foeDex = btlFoe.dex;
  const uint16_t youHp = btlYou.hp, foeHp = btlFoe.hp;
  const uint8_t menuBefore = btlMenu;
  render();
  ck(btlYou.dex == youDex && btlFoe.dex == foeDex &&
     btlYou.hp == youHp && btlFoe.hp == foeHp && btlMenu == menuBefore,
     "battle visual refresh preserves species, actual HP and menu state");

  // drop the foe's health and confirm the bar walks down instead of jumping
  btlFoe.hp = btlFoe.maxHp / 2;
  uint16_t first = btlHpShown[1];
  render();
  uint16_t afterOne = btlHpShown[1];
  ck(afterOne < first, "the bar starts draining after a hit");
  ck(afterOne > btlFoe.hp, "and does NOT snap straight to the new value");
  int frames = 1;
  while (btlHpShown[1] != btlFoe.hp && frames < 400) { render(); frames++; }
  printf("     bar reached the real value in %d frames\n", frames);
  ck(btlHpShown[1]==btlFoe.hp, "it does converge");
  ck(frames > 3 && frames < 200, "over a sensible number of frames");

  // healing must ease upward too, not only downward
  btlFoe.hp = btlFoe.maxHp;
  int up=0; while (btlHpShown[1] != btlFoe.hp && up < 400) { render(); up++; }
  ck(btlHpShown[1]==btlFoe.maxHp, "it eases upward on a heal as well");

  // animation timers must never alter the fight itself
  uint16_t hp = btlYou.hp;
  for (int i=0;i<20;i++) render();
  ck(btlYou.hp==hp, "rendering alone never changes anyone's health");
  // Drive an actual exchange. A reply must not overlap the first lunge or
  // show its target's flinch before that reply starts.
  emuSetCaptureTime(10000);
  startBattle(19, 50);
  for (int slot = 0; slot < MOVE_SLOTS; ++slot)
    btlYou.moves[slot] = btlFoe.moves[slot] = 0;
  btlYou.moves[0] = btlFoe.moves[0] = MV_TACKLE;
  btlYou.base[SI_SPE] = 999;
  btlFoe.base[SI_SPE] = 1;
  btlYou.maxHp = btlYou.hp = btlFoe.maxHp = btlFoe.hp = 1000;
  battleTap(233, 296);
  battleTap(149, 296);
  ck(btlYou.hp < 1000 && btlFoe.hp < 1000, "both moves deal damage once");
  ck(btlLungeUntil[0] > 10000 && btlLungeUntil[1] > btlLungeUntil[0],
     "the reply has its own lunge window");
  ck(btlHitUntil[1] > 10000 &&
     btlHitUntil[0] - btlHitUntil[1] == btlLungeUntil[1] - btlLungeUntil[0],
     "each hit reaction follows its attack in turn order");

  printf("%s\n", bad?"FAILURES":"all good");
  return bad?1:0;
}
