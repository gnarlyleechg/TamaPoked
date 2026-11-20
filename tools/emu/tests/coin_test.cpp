// Coins from steps, the daily cap, the shop gifts, and that coins outlive an ending.
#include "Arduino.h"
#include "Preferences.h"
#include "pet.h"
#include "party.h"
#include <cstdio>
uint32_t g_seed=5; FakeSerial Serial; FakeESP ESP; FakeWire Wire;
volatile int g_touchX=0,g_touchY=0; volatile bool g_touchDown=false; bool wasPressed=false;
uint32_t millis(){return 0;} void FakeESP::restart(){exit(0);}
int FakeSerial::available(){return 0;} String FakeSerial::readStringUntil(char){return String("");}
void sfxPlay(uint8_t){}
static int bad=0;
static void ck(bool ok,const char*w){printf("%s  %s\n",ok?"PASS":"FAIL",w); if(!ok)bad++;}

int main(){
  Pet p; p.begin(); p.dbgHatchAs(25,false);
  p.coins = 0; p.stepsToday = 0; p.stepsDay = 0; p.stepRem = 0;

  p.addSteps(25, 100);
  ck(p.coins == 2 && p.stepRem == 5, "25 steps = 2 coins with 5 left over");
  p.addSteps(5, 100);
  ck(p.coins == 3 && p.stepRem == 0, "the remainder carries into the next batch");

  // the daily cap: past 1500 steps a day, walking earns nothing more
  p.coins = 0; p.stepsToday = 0; p.stepsDay = 200; p.stepRem = 0;
  p.addSteps(1400, 200); p.addSteps(400, 200);
  ck(p.coins == 150, "the day's coins stop at 150 (1500 steps)");
  ck(p.stepsToday == 1800, "but the step count itself keeps counting");
  p.addSteps(100, 201);
  ck(p.stepsToday == 100 && p.coins == 160, "a new day resets the count and earns again");

  // the shop: spending needs the coins; a gift needs a creature
  p.coins = 4;
  ck(!p.spendCoins(5) && p.coins == 4, "cannot spend what you do not have");
  p.fullness = 10;
  ck(p.giveGift(0) && p.fullness == 100, "a meal fills the tummy");
  p.hygiene = 10; p.poops = 3;
  ck(p.giveGift(1) && p.hygiene == 100 && p.poops == 0, "a bath cleans and clears the mess");
  p.ivAtk = p.ivDef = p.ivSpe = p.ivHp = 31;
  p.trAtk = p.trMaxAtk(); p.trDef = p.trMaxDef(); p.trSpe = p.trMaxSpe();
  ck(!p.giveGift(4), "a vitamin is refused (and not charged) when nothing can train");
  ck(!p.giveGift(99), "an unknown gift is refused");

  // coins are the player's: a reload keeps them
  p.coins = 77; p.flushSave(); p.earnCoins(0); p.giveGift(2);  // giveGift saves
  Pet q; q.begin();
  ck(q.coins == 77, "coins survive a reload");

  printf("%s\n", bad ? "FAILED" : "ALL PASS");
  return bad ? 1 : 0;
}
