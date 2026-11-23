# Battle presentation slice — digital prototype

**Status:** Committed on a topic branch stacked on `codex/pokemon-visual-pilot` (draft PR #4). Not merged, flashed, hardware tested, or release-ready.

## Purpose

Continue the owner's approved, recognizable-Pokémon visual refresh without changing battle rules. This is a small **battle staging and interface pass**, not a replacement for the existing species sprite artwork and not a claim that the sprites themselves have become realistic.

## What changed

- Replaced the plain lower battle band with a low-cost RGB565 ground gradient and narrow horizon separator.
- Added elliptical-looking, two-tone ground shadows using rounded rectangles anchored to each combatant's real baseline: PMD sprites use `sy + 78`; the thumbnail fallback uses `sy + 48`. Shadows stay fixed while attack/hurt sprites lunge or shake.
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
