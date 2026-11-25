# Consolidated local review — 2026-11-24

## Source under review

- Repository: `gnarlyleechg/TamaPoked`
- Branch: `codex/tamapoke-hd-frame-fit`
- Firmware source commit: `489a3fd9cba2adabb047a208526b3a5961efbbbe`
- Draft pull request: #6 (keep draft)
- Board target: Waveshare ESP32-S3-Touch-AMOLED-1.75, ESP32-S3, OPI PSRAM, 16 MB

The firmware source was not changed in this review. The documentation and private review package record and validate the already integrated branch state.

## Verified results

- Recovered the existing painted SD archive and artwork source archive. Their SHA-256 values match `docs/PAINTED_SCENES.md`.
- The actual SD pack contains 94 plates. Its C++17 regression produced 185 distinct day/night results and passed the corruption, bounds, cache, missing-file, and fallback checks.
- Restored the repository's seven existing sprite bundles. `audit_sprite_clips.py` validated 790 normal and 789 shiny sprite files. It reports 787 authored walk-left and walk-right clips, 787 normal sleep clips, 202 normal eat clips, 786 hurt clips, and 786 attack clips. Missing authored clips continue to use existing runtime fallback behavior.
- Captured the real firmware emulator at 466 × 466 for the living room, all three training games, day/night battle, Pokédex, and all 91 gym trainer slots in day and night lighting. Captured walk/eat/sleep/battle sequences for Bulbasaur, Charmander, Squirtle, and Pikachu. Visually reviewed the current screenshots and the complete gym contact sheets. The backgrounds are distinct and legible within the circular display. Pokémon remain the existing canonical sprite art.
- All 46 emulator regression suites passed. This includes touch routing, screen flush, save migration, sprite identity above dex 255, shiny identity, and scene coverage.
- The private-update stager regression passed 9/9 tests.
- Full pinned ESP32-S3 compile passed with `esp32:esp32@3.3.10` and the repository's 16 MB OPI-PSRAM FQBN. Firmware image: 1,856,304 bytes; compiler-reported static RAM: 67,028 bytes (20%); free static RAM: 260,652 bytes.
- `esptool image-info` recognized ESP32-S3/16 MB and verified the image checksum and validation hash.
- Candidate partition-table SHA-256 `ace02503447d0f470692e65fa76002f2d77a92dc81cd3813d8aa66718d716da9` matches both the prior 3.24 private installer and the branch's existing installer files. The installer range check reports no writes over NVS `0x9000–0xE000` or FFat `0x610000–0xFF0000`. The browser manifest retains its unchecked erase confirmation.
- A private candidate package was assembled with all four compiled flash files, a browser installer, the matching SD scenes archive, installation and save-preservation instructions, SHA-256 checksums, screenshots, and an animated starter preview. Embedded installer binaries were byte-compared with the staged binaries; the ZIP integrity check passed.
- Package SHA-256: `2f6c18e23a42dcd3a7bcafcf14ef6e885a4d81e6c299d92ae9d4afa8c043fd65`. App SHA-256: `bee500905545de3797fcfe48036a8327e6d6fde4d438142ba35aa4c1be78e878`.

The emulator and compiler do not measure actual display refresh rate, touch behavior, battery use, SD cold-load latency, or board PSRAM pressure. No hardware was connected or flashed, no SD card was handled, no GitHub Actions run was launched, and no merge or public release was made.

## Character art and release limits

No replacement Pokémon art was imported. The project source gate found no candidate textured model with documented permission to redistribute renders of the Pokémon IP. The previously rejected geometric Bulbasaur pilot remains rejected. Existing canonical normal and shiny sprites, animations, and fallback behavior are retained. A licensed or explicitly authorized realistic character source is still needed before character rendering can be replaced or distributed.

The installer package is a review candidate, not approval to flash. The package's partition table matches the previous installer; the physical device's current active slot and security configuration were not read. Preserve existing SD contents, leave the installer erase option unchecked, and do not flash until the owner approves.

## Handoff files

Private Library deliverables:

- `TamaPoke_3.24_489a3fd_Private_Test_Package.zip` — four firmware binaries, embedded browser installer, matching scene pack, instructions, checksums, emulator screenshots, and GIF.
- Individual image/GIF deliverables: gameplay contact sheet, living-room day/night, starter action review, all-gym day/night contact sheets, and four-starter walking GIF.
- Original source and SD scene archives remain available as `tamapoke_painted_artwork_source.zip` and `tamapoke_painted_sd_pack.zip`.

## Remaining milestones

1. Owner reviews the visual evidence and decides whether to proceed with the still-open character-art permission search.
2. Find a model/art source with permission that covers rendered game assets and redistribution, then pilot Bulbasaur through the existing importer and real emulator scenes. Keep original sprites where that gate is not met.
3. If firmware or asset code changes, rerun targeted checks and one consolidated emulator regression plus pinned board compile. Do not repeat this review's checks unless source or assets change.
4. Before any installation, confirm the device baseline and active app slot, back up the save and SD card, and obtain explicit approval to flash. Then test touch, FPS, memory, SD loads, and save survival on hardware.

Regular Chat can review the package and screenshots, discuss visual direction, and continue source-license research. Work mode is needed for local repository editing, emulator captures, pinned compilation, and packaging. Physical board validation remains a separate owner-approved step.
