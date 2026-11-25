# Local verification — draft firmware

Date: 2026-11-25 (America/Chicago)  
Repository: `gnarlyleechg/TamaPoked`  
Branch: `codex/tamapoke-hd-frame-fit`  
Verified code/test head: `af929dcb61abeff511a2e3fd98c8b79657688058`

## Results

| Check | Result | Notes |
|---|---|---|
| Pinned ESP32-S3 firmware compile | PASS | Arduino CLI 1.6.0; ESP32 core 3.3.10; GFX 1.6.9, SensorLib 0.5.1, XPowersLib 0.3.3, U8g2 2.36.19; Waveshare ESP32-S3, OPI PSRAM, 16 MB partition target |
| Full host emulator suite | PASS — 46 passed, 0 failed | Run from `tools/emu/tests/run.sh`; SDL2 host library and development headers available locally |
| Battle animation regression | PASS | Updated the test to dismiss the battle narration panel before selecting FIGHT and TACKLE; verifies both attacks and turn-ordered lunge/hit timing |
| Sprite pack validation | PASS | Restored local fixtures from the existing seven `web/sprites-*.pak` bundles; 790 sprites load, 19 are legitimately absent, and 528 species above dex 255 were compared against wrap-around twins |
| Purchase and save regression | PASS | Shop purchase and item inventory survive save reload; item use persists consumption and spends a turn |
| Wild run confirmation | PASS | The real tap handler keeps the fight open on NO and returns to the same Kanto region on YES |

The firmware compile reported 1,859,979 bytes of flash (59% of the 3,145,728-byte application partition) and 67,036 bytes of static RAM (20% of 327,680 bytes). Existing warnings remain for format specifiers, missing initializers and deprecated SensorLib headers; there were no compile errors.

The firmware compile used commit `3a6dfae92e020b9ee08fc2da6c97fd7dc2df9bc4`. Firmware source did not change after that compile. The later commits update the animation test's setup taps and this verification record; the complete emulator suite ran against those exact test changes before they were committed.

The emulator suite initially exposed two test setup failures. `anim_test` tapped through a stale battle sequence and never dismissed the narration panel. `sprite_test` ran without its local sprite fixtures. The animation test now performs the current three-step tap sequence. The sprite fixtures were extracted from the already checked-in installer bundles; no sprite source was downloaded or changed. Both targeted tests then passed, followed by the full 46-suite run.

## Reproduction

From the repository root, with `sdl2-config`, SDL2 development headers, and the pinned Arduino packages installed:

```sh
bash tools/emu/tests/run.sh
```

If `tools/sdcard/mons/` is absent, restore the test-only sprite fixtures from bundled packs first:

```sh
python3 tools/unpack_emulator_assets.py --out tools/sdcard/mons \
  web/sprites-kanto.pak web/sprites-johto.pak web/sprites-hoenn.pak \
  web/sprites-sinnoh.pak web/sprites-unova.pak web/sprites-kalos.pak \
  web/sprites-alola.pak
```

The firmware was compiled directly with the target shown by `.github/workflows/build-firmware.yml`:

```sh
arduino-cli compile --warnings=all \
  --fqbn 'esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB' \
  --output-dir out TamaPoke
```

The working copy used a case-matched `TamaPoke/` sketch staging directory because Arduino CLI requires the sketch directory and `.ino` basename to match.

## Follow-up touch-flow check

After the full 46-suite run, the touch test was extended to exercise the wild-battle RUN confirmation through the real tap handler. Choosing NO keeps the battle open and preserves the Kanto gym context; choosing YES closes the wild battle and returns to that same region. The targeted touch test passed with both paths. The firmware source did not change, so this adds focused coverage without repeating the full suite.

## Still open

- No physical-board test was run. Touch feel, audio, radio, battery behavior, reboot persistence and performance under real PSRAM/DMA pressure remain unverified.
- The 3D enclosure fit and child-use checks need physical review.
- Original Pokémon species and game data remain in use. Final realistic-art implementation and its source/licensing approval remain a separate open gate; this verification does not claim that art work is complete.
- The draft PR remains unmerged. No GitHub Actions run, flash, hardware operation or publication was performed for this verification.
