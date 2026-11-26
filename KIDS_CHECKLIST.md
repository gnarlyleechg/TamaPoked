# Things to check on the real board (kids build)

Nothing here has run on hardware yet. Tick off as you try each one. Flash with the installer page; your pet is kept.

## Menu and settings
- [ ] The round BATTLE button (left) opens the gym list; the MENU grid button (right) opens the menu. Both are easy to hit with a finger.
- [ ] Swipe up from the very bottom edge opens the MENU (grid of buttons). Swipe up from the middle still opens the creature card.
- [ ] Tapping the name band at the top also opens the menu.
- [ ] Every button works: STATS, POKEDEX, BATTLE, PARTY, SHOP, CHESS, ACTIVITIES, SETTINGS, RETIRE, CLOSE.
- [ ] SETTINGS from the menu works (the BOOT button is optional: it is not reachable on the 3D-printed case yet)
- [ ] The BOOT button opens SETTINGS (brightness bars, volume, speaker icon).
- [ ] Brightness bars: each tap changes the level and the screen really gets brighter or darker.

## Sound
- [ ] Eat, sleep, bath (water), fart on poop, purr when petting.
- [ ] Ball bounce, punching bag, target game arrow and sad sound: no delay, none missing.

## Steps and coins
- [ ] Walk about 100 steps carrying it: coins and the step count go up.
- [ ] Shop: buy something, coins drop, item works.
- [ ] A gym win pays coins.
- [ ] Fishing, Treasure and Penalty pay coins (penalty: 5 kicks, 2 coins a goal); after 60 coins in a day they say "daily limit".

## Chess
- [ ] Squares are easy to tap with a finger. Computer answers in about 1 second on HARD.
- [ ] A finished game pays coins.

## Battles
- [ ] Swiping during a battle does nothing; RUN asks "Run away?".
- [ ] Battle music plays.

## Saving
- [ ] After about a minute running on a new day, a backup is written to the SD card (/backup/save.bin).
- [ ] Reflash with the installer: the pet and coins are still there.

## Still on hold
- Overnight USB-only test (unplug at night, check the pet is OK in the morning).
- Battery, charging, 3D printing: need your approval first.

## Previously checked on the PC emulator (no board needed)
Before Paint was added, emulator coverage included the existing menu buttons, both side buttons, the bottom swipe, fishing, treasure, penalty, the shop and chess; each screen drew and refreshed. Coin payouts and the daily limit were also checked. Paint is new; its emulator and device checks remain unchecked above. The earlier suite runs with `tools/emu/tests/run.sh walk`. The board-only items above (sound, steps, touch feel, SD backup, timing) cannot be checked this way.

## Battery-saving changes (v3.22) - check these too
- [ ] Touch still feels instant while playing (the loop now naps 3 ms between passes).
- [ ] Press PWR to turn the screen off, then press again: it wakes at once, picture normal, sound fine.
- [ ] After 5 minutes untouched the screen dims and the animation is a bit choppier (on purpose); touching it wakes it.
- [ ] Steps still count with the screen off (walk, wake, check the count).
- [ ] If you have a USB power meter, note the current with the screen on, dimmed and off. This is the first real battery number.

## Paint activity
- [ ] Open Paint from the bottom entry in the main menu; open the color picker, choose a color and tap DONE. The canvas returns with the chosen color.
- [ ] Draw with the pen, marker and brush. Eraser removes marks. Bottom tools stay clear of the drawing area.
- [ ] Undo removes the last stroke. Clear asks first; NO keeps the picture and YES clears it.
- [ ] Save a picture, leave Paint and reopen it. The saved drawing returns.
- [ ] Restart the device and reopen Paint with the SD card installed. The saved drawing returns and the pet's existing save remains intact.
- [ ] Remove the SD card or boot without one. Painting still works and SAVE reports that it could not save.
- [ ] Tap the home icon, cancel once, then confirm. It returns to the main game screen.
