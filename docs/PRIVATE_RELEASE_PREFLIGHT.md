# Private playable-release preflight

## November 24 consolidated local verification

The earlier November 24 notes below describe the state before a complete local
workspace and pinned toolchain were available. For firmware source commit
`489a3fd9cba2adabb047a208526b3a5961efbbbe`, the current Work session restored
the existing scene/sprite assets and completed the missing local checks:

- 46 emulator regression suites passed; the 9 app-only package guard tests
  passed.
- The actual 94-plate scene archive passed its C++17 loader regression with
  185 distinct day/night outputs.
- Real emulator screenshots were captured for all 91 trainer slots by day
  and night, the three training games, Pokédex, battle, and living-room modes.
  The review contact sheets were visually inspected.
- The pinned ESP32-S3/OPI PSRAM/16 MB build passed: app 1,856,304 bytes,
  compiler-reported static RAM 67,028 bytes. `esptool image-info` validated the
  ESP32-S3 image checksum and validation hash.
- Candidate partition-table bytes match the prior 3.24 private installer.
  Installer range validation confirms the staged write ranges do not overlap
  NVS or FFat, provided the erase option remains unchecked.

See `docs/CONSOLIDATED_LOCAL_REVIEW_2026-11-24.md` and root
`WORK_TO_CHAT_HANDOFF.md` for capture/package locations and remaining gates.
These results do not establish device touch response, refresh rate, live PSRAM
pressure, battery use, SD load time, or the board's active app slot/security
configuration. The private installer remains review-only. Do not flash or
publish it without owner approval. No hardware operation, GitHub Actions run,
merge, or public release was performed.

## November 24 checkpoint

Parent artwork checkpoint: db15910a0be6c4df1dafa7fe632055189d1bba89.
No firmware code or save/partition definitions changed in this release-preflight pass. New work: app-only packaging tool, nine synthetic partition guard tests, emulator capture selectors for all 91 trainer slots in day/night and three training games.

Rechecked the actual exported source and SD ZIPs against the hashes in PAINTED_SCENES.md. Both matched. All 94 plate sizes, individual SHA-256 values, CRC32 values and atlas hash passed. The raw plates total 5,713,176 bytes. Exact locally compiled header/test Git blob hashes matched the existing repository: painted_scenes.h 485205502ea2eb9b6d250d2e72bc4649f10814cc; textured_scenes.h 4c1c549c1fecc860b3446195b897e1612e736f20; training_scenes.h f281831957d8dc64a8706608125933bf7ca89aa5; painted_scenes_test.cpp 1f2c2b5662ef5223184e8f5f8f3921624a0713c3. Rebuilt the actual test with C++17, -O2 -Wall -Wextra -Werror: 185 distinct outputs, corruption and fallback passed; no-pack fixture passed. Host run measured 0.17 s total, 2176 KiB process RSS; neither is a device performance measurement. Previous sanitizer evidence remains valid for these unchanged headers.

The new private-update tool and its nine synthetic tests passed locally; exact Git blob hashes matched. This is a structural gate, not an actual firmware image verification. The capture selector changes were source-reviewed against the sketch declarations, not compiled. The full repository cannot be cloned from the Chat container (DNS/network unavailable); arduino-cli and SDL2 development tooling are absent. No fresh full emulator run, sprite-roster audit, firmware binary, hardware timing, or flash performed. No Actions run intentionally initiated.

## One consolidated local validation

Use a disposable clone of the current draft branch, not the user's installed save directory. Preserve both artwork archives outside the clone, verify hashes and extract the scenes folder under tools/sdcard/scenes. Restore existing bundled sprites with tools/unpack_emulator_assets.py --out tools/sdcard/mons followed by the existing installer sprite bundles (locate them in the checkout; do not substitute arbitrary downloads). Run tools/audit_sprite_clips.py against that directory. Re-run standalone painted_scenes_test with the real scenes directory as its argument, not just the no-pack fixture.

Use the pinned core esp32:esp32@3.3.10 and library versions in .github/workflows/build-firmware.yml. Build the full emulator using tools/emu/build.sh, run tools/emu/tests/run.sh, and capture:

- --shot sack, --shot speed, --shot ball
- --shot gymday-R-T and --shot gymnight-R-T (R=0..6, T=0..12)
- --shot starterwalk, startereat, startersleep, starterbattle for normal/shiny species with original sprite packs. Use --out - for behavioral audit and selected full 24-frame captures for visual inspection.

Verify actual PPMs exist, show the intended art rather than fallback, and keep names, targets, score, timer, HP, battle controls and sprites inside the round bezel. Check training results as well as active play, easy/hard battles, opponent swaps and missing-art fallback. Do not equate a distinct asset hash or successful screenshot exit code with a correct game screen. Measure SD cold-load latency, actual free PSRAM, minimum heap and frame rate on the board before release approval.

## Compile and package

The pinned FQBN is esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB. Use the local workflow's sketch-folder naming convention and collect actual compiled app and partition binaries. The normal host runner also compiles firmware if arduino-cli is on PATH; avoid duplicating builds unnecessarily. Do not publish docs/firmware or invoke a public release.

Run python tools/test_stage_private_update.py. The app-only stager requires the candidate app, candidate partition table, independently trusted partition table from the installed build/device, exact full commit SHA, and a new output path. Verify the real firmware using esptool image-info and the actual installed device's layout/active slot. Example command after those inputs exist:

    python tools/stage_private_update.py --app out/TamaPoke.ino.bin --partitions out/TamaPoke.ino.partitions.bin --installed-baseline /path/to/trusted-installed-partitions.bin --commit FULL_SOURCE_SHA --out private-update.zip

Never substitute a synthetic fixture or an older app under the new commit name. The tool stages only app.bin, release.json and README.txt; it does not flash. Require backup, installed-layout and active-slot verification, security configuration compatibility and specific owner approval. Preserve NVS/FFat/SD; never erase-all or format the card. The private app-only package is not the browser installer; do not imply a browser one-click install is ready.

## Character animation review

The earlier 83af4ba checkpoint reports 43 host suites, 6320 emulator sequences and a pinned firmware build. That is historical evidence, not verification of this new painted branch. The source audit covers TPK2 idle/walk left/walk right/sleep/eat/hurt/attack. Actual current per-action missing counts cannot be determined without restoring the binary sprite packs. The runtime uses fallback poses and motion when authored clips are missing. The existing realistic character art gate still rejects the geometric Bulbasaur pilot; canonical detailed replacement sprites and independently authored motions are not ready. Preserve existing species, shiny identity and animations for this release candidate; no unapproved new models or sprites.

## Release decision

Not yet installable. Keep PR #6 draft; no merge or flash. The next hard dependency is a network-enabled local checkout with pinned build tooling and the exported artwork packs, or an explicitly authorized private build environment. The user does not need to alter their existing SD card until the matching firmware has passed verification.
