# Battle presentation slice — digital prototype

**Status:** Committed on a topic branch stacked on `codex/pokemon-visual-pilot` (draft PR #4). Not merged, flashed, hardware tested, or release-ready.

## Purpose

Continue the owner's approved, recognizable-Pokémon visual refresh without changing battle rules. This is a small **battle staging and interface pass**, not a replacement for the existing species sprite artwork and not a claim that the sprites themselves have become realistic.

## What changed

- Replaced the plain lower battle band with a low-cost RGB565 ground gradient and narrow horizon separator.
- Added elliptical-looking, two-tone ground shadows using rounded rectangles anchored to each combatant's real baseline: PMD sprites use `sy + 78`; the thumbnail fallback derives its center and lower edge from `GAL_CELL` and the actual bitmap height; no sprite means no shadow. Shadows stay fixed while attack/hurt sprites lunge or shake.
- Added a restrained accent marker and depth shadow to the existing name/HP panel. Accent is read from the existing canonical species table.
- Reused one decorative button painter across battle menus, keeping **all existing button dimensions, text, switch/run/move states, and hit areas**.

This uses primitive drawing calls; there are no new images, IDs, rules, allocations, Flash partitions, NVS fields, Pokémon art assets, or translations. The original biome/day-night background and existing PMD attack/hurt/idle playback remain in place.

## Screens and invariants to inspect

- 466 × 466 round display, top edge clipping, player/foe shadows and HP plate contrast.
- Home/day/night visuals from the previous pilot are unchanged.
- FIGHT, switch/POKÉMON, RUN, move list, inactive/fainted team entries, narration, and LAN waiting state all need emulator screenshots.
- Actual touchscreen accuracy is not established by source review.
- Verify SD sprite and missing-SD thumbnail fallback, day/night battle backgrounds, long species names, hit effects, large/small Pokémon, both faint/enter transitions, and low memory operation.

## Verification at handoff

- Static source-level checks after the edit: same `BTL_GRID_X/Y`, `BTL_CELL_X/Y`, `BTL_HIT_X/Y` definitions, `drawPmdActM` call, `FW_VERSION`, and no changes to the `battle.h` game model. Decorative changes are confined to `btlSide` and `renderBattle` helpers.
- **Not run:** SDL2 host emulator tests, Arduino ESP32-S3 firmware compile, real-device 466 × 466 screenshots, performance/fps profiling, RAM high-water measurement, touch testing. These are required before merge/release.
- Code review still needed to confirm visual placement and that the added per-frame draw calls are acceptable.

## Next digital milestone

Produce a real-source emulator screenshot for a canonical Pokémon such as Bulbasaur in battle, confirm no controls clip on the circular panel, run the host walkthrough tests, then an ESP32-S3 build with OPI PSRAM and size/headroom review. Only after that should the visual system be adapted to the Pokédex and other screens. Richer creature art needs a traceable licensed source per species and should not invent or alter Pokémon identities.

## Follow-up verification work

- `2c8cb8b`: corrected fallback shadow placement after reading the actual `drawThumb` implementation; the previous fixed 48px offset did not account for the 80px centering cell or variable thumbnail heights.
- `3e0e91d`: extended `anim_test.cpp` with real-framebuffer checks for floor gradient, horizon, button highlight and unchanged actual combatant IDs, HP and menu state across rendering. Existing HP easing checks remain intact.
- `892954a`: added an isolated draft-branch workflow `.github/workflows/battle-visual-check.yml` that installs SDL2, runs existing host suites, exports four emulator screenshots, and independently compiles with the same pinned ESP32 target and libraries as the existing firmware build. It does not update the web installer or publish flash files. Workflow triggers are scoped to the battle branch and relevant code/test/workflow changes. No results have been verified yet; available commit checks showed no pull-request-triggered run or combined status at the time of inspection. Local full compile could not run because the container has no Arduino CLI/SDL2 and cannot resolve GitHub for a checkout.

Do not confuse the new tests being committed with passing tests. The next evidence is a completed host/firmware run and review of the real screenshots.

## Visual continuation — renderer, collection and fishing

- Added `sprite_runs.h`: contiguous same-palette-index spans (and opaque silhouette spans) replace individual pixel rectangles in the existing PMD and thumbnail painters. This changes draw-call grouping only, not the canonical art, animation selection, frame anchors, species data or save format.
- Local isolated C++17 regression: `g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -g sprite_runs_test.cpp -o test && ./test` passed 800 pixel-exact cases and the null-input guard. Locally tested files matched the GitHub blob IDs `406ccd9c709ea4cf7a9d807f824c557610d80aa9` and `f87434611e30c9847af9bf3559d7150d17a3b2bd`. This does not establish integrated firmware timing or compile status.
- Pokédex: species-accented collection cards and display stage, corrected thumbnail centering, neutral stage for unknown species, and explicit grid bounds before C++ signed division. Host coverage added for gutter taps and IDs above 255; not yet executed in the full emulator.
- Fishing: original procedural sunset artwork inspired by the park's lake, trees and views of the I-210 bridge, with water reflections and the original fishing coordinates. Geographic references: https://www.calcasieu.gov/services/parks/prien-lake-park and https://www.calcasieu.gov/Home/Components/FacilityDirectory/FacilityDirectory/32/1466 . No third-party photos were copied; the scene is stylized, not a faithful survey. Fishing rules, rewards and bite timing unchanged.
- Screenshot harness adds the actual Bulbasaur collection-detail selection and captures fishing through the existing walkthrough. CI also includes sprite regression and refuses zero matched tests. Full emulator and board compilation are not verified; local environment has g++ but no SDL2 or Arduino CLI, and GitHub DNS resolution failed.
- Next: inspect completed CI evidence and compare screenshots with the approved visual direction. Realistic replacement species artwork is still a separate, unfinished task. Do not change the installer or merge until validation is complete.
