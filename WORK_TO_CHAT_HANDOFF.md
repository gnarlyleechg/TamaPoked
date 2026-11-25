# Asher TamaPoke — Work to Chat handoff

## Exact project state

- Firmware repository: `gnarlyleechg/TamaPoked`
- Development branch: `codex/tamapoke-hd-frame-fit`
- Last firmware source/build commit verified with the pinned board toolchain: `489a3fd9cba2adabb047a208526b3a5961efbbbe`
- Current branch head: `ed06a771384775604fbfc0700a08fbae397d95ce` (wild encounters and battle items; targeted local checks recorded in `docs/LOCAL_VERIFICATION_2026-11-25.md`; no pinned board build yet)
- Pull request: [#6](https://github.com/gnarlyleechg/TamaPoked/pull/6), open and draft
- Engineering repository: `gnarlyleechg/AI-Engineering-Workshop`
- Board: Waveshare ESP32-S3-Touch-AMOLED-1.75, circular 466 × 466 display
- Target date: December 25, 2026

This handoff adds review documentation to the existing firmware branch. No firmware code, save schema, partition definition, sprite bundle, or public release was changed in this session.

## Completed and verified

- Recovered and hash-verified the existing original painted environment archives.
- Built emulator screenshots for all three training games, Pokédex, battle day/night, living-room day/night, and all 91 gym slots under day and night lighting.
- Captured Bulbasaur, Charmander, Squirtle, and Pikachu in walk, eat, sleep, and battle states. Produced a four-starter walking GIF.
- Visually reviewed the screenshot contact sheets. The environments are distinct and fit the circular display. The Pokémon remain the existing canonical sprites.
- Passed all 46 current emulator regression suites and all 9 private-update stager tests.
- Passed the painted-scene loader regression on all 94 actual plates (185 distinct day/night outputs) and validated the restored normal/shiny sprite files.
- Completed one full pinned ESP32-S3 build: 1,856,304-byte app; 67,028 bytes static RAM. `esptool image-info` verified the ESP32-S3 image checksum and validation hash.
- Confirmed the candidate partition table matches the previous 3.24 installer and the existing branch installer. The installer range check preserves NVS and FFat when its erase option is left unchecked.
- Created a private review package with compiled firmware, embedded browser installer, matching SD scene archive, instructions, checksums, screenshots, and animated preview.

## Artifact locations

The private Library package is named `TamaPoke_3.24_489a3fd_Private_Test_Package.zip` (SHA-256 `2f6c18e23a42dcd3a7bcafcf14ef6e885a4d81e6c299d92ae9d4afa8c043fd65`). It contains:

- `Install_TamaPoke_3.24_489a3fd.html`
- Four exact compiled firmware parts and `OFFSETS.txt`
- `sd-card/tamapoke_painted_sd_pack.zip`
- `README_INSTALL.txt`, `SHA256SUMS.txt`, and `firmware/esptool-image-info.txt`
- Current emulator evidence in `evidence/`: gameplay contact sheet, living-room day/night, starter action review, all-gym day/night contact sheets, and `four-starters-walk.gif`

The original art sources remain in Library as `tamapoke_painted_artwork_source.zip` and `tamapoke_painted_sd_pack.zip`. The SD archive SHA-256 is `488b50042b3f5e3312c3d35f5f63961938d90eff76ecfa7f52ead90ebb1d7756`.

## Known limits and outstanding work

- Realistic textured Pokémon replacement art is not ready. No redistributed candidate found in the current source gate has permission covering rendered Pokémon assets. Existing sprites, shiny variants, and fallback animations remain intact. See `docs/REALISTIC_ART_SOURCE_GATE.md`.
- The sprite audit reports many species have no authored eat clip; runtime fallback remains. Do not describe all action animations as authored for every species.
- The emulator does not verify physical touch, refresh rate, battery use, SD cold-load latency, or live PSRAM use.
- Actual board baseline, active app slot, and security settings were not read. No flash or SD operation occurred. No action may flash until explicit owner approval.
- The private installer is a review candidate only. Do not publish or flash it.

## Next tasks

### Efficient in regular Chat

- Review the private package, screenshots, and GIF; give one consolidated visual direction rather than checking each small edit.
- Continue research for character art/model terms that explicitly allow distribution of rendered game assets. Preserve the existing art unless those permissions are clear.
- Review the documented remaining gates and prepare the next focused work session.

### Work mode

- Import a properly licensed or owner-authorized textured character pilot, then validate it in actual living-room and battle emulator captures.
- Continue code changes, run targeted regressions, and perform one consolidated ESP32-S3 build after the next integrated change.
- Package an updated private candidate only after the code and SD assets change and pass validation.
- Any physical touch, timing, memory, save, or install validation requires the owner to handle the device. Do not flash without approval.

## Do not repeat

Unless the source or assets change, do not repeat the 46-suite run, the painted plate validation, the sprite bundle restoration/audit, or the pinned full build. Their source commit, measurements, and limits are recorded in `docs/CONSOLIDATED_LOCAL_REVIEW_2026-11-24.md`.

## Ready-to-copy continuation prompt

```text
Resume Asher's existing TamaPoke project from gnarlyleechg/TamaPoked, branch codex/tamapoke-hd-frame-fit, draft PR #6. The verified firmware source/build commit is 489a3fd9cba2adabb047a208526b3a5961efbbbe. Read WORK_TO_CHAT_HANDOFF.md and docs/CONSOLIDATED_LOCAL_REVIEW_2026-11-24.md first. Do not repeat the 46 passing emulator suites, 9 private-update tests, painted-scene validation, sprite audit, or pinned ESP32-S3 build unless code/assets change. Keep original Pokémon sprites and shiny variants until an art/model source has documented permission for redistributing rendered game assets. The private test package is TamaPoke_3.24_489a3fd_Private_Test_Package.zip. It is review-only: do not publish, merge, or flash. Continue with one consolidated visual review and permission-qualified character art research; then make any justified code/art changes and run targeted checks plus one consolidated build. No purchases, GitHub Actions, hardware, or save-destructive operations.
```

## Work-mode exit status

It is safe to leave Work mode now. The review package and test evidence are assembled, the handoff documentation is committed on the existing draft PR, and remaining tasks are clearly separated above.


## Audit update — 2026-11-25

The current review screenshots were inspected. The distinct training and gym scenes are present in the emulator captures. The existing Pokémon remain recognizable canonical pixel sprites; they do not yet meet the requested realistic-art direction.

A goal audit found a battle-menu gap that the earlier handoff missed:

- The current battle screen offers FIGHT, POKÉMON, and RUN. It has no ITEM option.
- The current save/game model has coins and immediate-use shop gifts, but no held battle-item inventory. The item menu is therefore not a label-only change.
- To meet the requested classic battle controls, add a save-compatible item inventory, item selection and use during battle, and a four-action layout that preserves Pokémon switching and running. Test item effects, turn order, battle exit, save migration, and touch hit areas in the emulator; then make one pinned firmware build. Do not claim Fight/Item/Pokémon/Run is complete until those checks pass.

Art-source research was updated in [REALISTIC_ART_SOURCE_GATE.md](docs/REALISTIC_ART_SOURCE_GATE.md). The search found no candidate permission covering redistribution of rendered Pokémon game assets. Retain current sprites unless that gate changes.

The enclosure source review found the manufacturer listing for the Waveshare Type B 8 Ω, 2 W speaker. It lists a 2-pin PH1.25 connector and about 120 mm of cable, but does not document the actual exit direction or minimum bend radius. Confirm the user's delivered speaker matches that model. The full terminal route, retention, and service path remain open.

The R6 digital enclosure baseline remains unchanged. Its partial 9 mm route screen is not a complete speaker route. Keep physical-fit, printing, device, and release gates open as listed below.

## Verification update — current firmware head

At `ed06a771384775604fbfc0700a08fbae397d95ce`, the branch implements the four-action battle menu, save-backed Potion / Super Potion / Full Heal inventory, battle-item shop, and region-pack-backed wild encounters. Targeted battle, save, touch-flow and six-language ASCII/table checks pass locally; exact commands and limits are in `docs/LOCAL_VERIFICATION_2026-11-25.md`.

This is not a replacement for the pinned ESP32-S3 build previously verified at `489a3fd9cba2adabb047a208526b3a5961efbbbe`. The current head has not received that board build or a full SDL emulator run. No GitHub Actions run, merge, installer publication, or device operation was performed. Next software gate: run the complete local emulator suites and one pinned board build when the required SDL2 and Arduino CLI/core/library toolchains are available. Hardware touch, item persistence across real reboot, audio and wild-region return still need owner testing on the board.
