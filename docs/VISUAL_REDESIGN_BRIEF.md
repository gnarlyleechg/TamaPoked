# TamaPoke visual redesign brief

**State:** visual direction approved; the home-screen background slice is implemented on the pilot branch.
**Scope:** visual presentation only. Keep the current game, save format, species data,
controls, installer and licensed sprite pipeline working.

## Owner-approved direction

- Use the same recognizable home-room backgrounds used by Adeline's game for the
  daytime and nighttime home screens.
- Make Pokémon and scenes feel more richly rendered and polished, while keeping
  each Pokémon's canonical identity and design.
- Use the battle concept with one combatant on the left and one on the right,
  clear **Fight / Item / Run** controls, and animated battle poses.
- Give the fishing game a Prien Lake Park, Lake Charles setting.
- Keep training Pokémon-themed and treasure Pokémon-themed, with a more finished
  visual treatment. Carry the richer style through chess, settings, menus and
  the other minigames.
- Keep layouts readable and touch targets generous on the round 466 x 466 panel.

## Pokémon identity guard

The concept creature named Mosskin was only a visual placeholder. It must never
be added to the game. The existing Pokédex roster and species IDs remain the
source of truth. Art can improve rendering, lighting, texture and animation, but
must not create species, change a species' silhouette-defining features, rename
it, or alter its shiny form, evolution, type or game data.

The first visual pilot will use Bulbasaur as a representative existing Kanto
starter, using the actual Bulbasaur art already supplied by the game's sprite
pipeline. It will show the same species on the home-day, home-night, battle and
Pokédex screens. This does not select or replace Asher's saved Pokémon.

## Existing technical constraints

- Active firmware work is based on `kids-version`; this brief is stacked on the
  open `codex/touch-brightness-timing` branch and does not merge it.
- `PMD SpriteCollab` supplies the current multi-action Pokémon sprites. The
  current pipeline packs them as TPK2 files for the microSD card, with Pokémon
  and shiny variants, and renders idle, walk, sleep, eat, hurt and attack poses.
- Keep source and attribution compatible with the project's existing
  non-commercial asset license. Record the source and license for every new art
  asset. Do not replace the licensed sprite pipeline with untracked generated
  or scraped species art.
- Prefer SD-card visual assets over adding large embedded bitmaps to firmware.
  Preserve the existing firmware partition layout, web installer, and ability
  to operate when optional art is absent.
- All mockups and implementation previews must be composed at the actual round
  display size, with the circle edge, bottom controls, text and touch areas
  checked for clipping and legibility.

## First implementation slice

The home screen now uses the approved day/night living-room illustrations, with
the existing canonical species sprite and all current home controls left intact.
Source art stays at 233 x 156 and is packed into indexed RGB565 firmware data.
The renderer expands it once into a PSRAM cache and falls back to the previous
procedural scene if that allocation fails. The 466 x 466 host-emulator preview is
`docs/previews/home-room-bulbasaur-emulator.png`. This is emulator evidence only;
board rendering, memory headroom and frame timing are not yet verified.

## Pilot completion evidence

1. Preview a single real species across home day/night, battle and Pokédex using
   its existing game asset and the new screen treatment.
2. Confirm the battle keeps the existing turn logic and current animation data;
   the art pass must not change battle rules or create a new creature.
3. Compare the preview with 466 x 466 screenshots from the real-source emulator.
4. Check firmware and installer size before adding art to a release path. A
   successful compile does not prove board rendering or touch performance.
5. Keep original sprite, save, installer and license behavior available when a
   new optional visual asset is missing.

## Remaining screen inventory

After the pilot, apply the approved visual system to the Pokédex and collection
views, egg and evolution scenes, party and box, battle selection and results,
gym/battle locations, fishing, training, treasure, chess, settings, menus,
minigames, and connection/status screens. Treat this as staged work; do not
replace every screen in one unreviewable change.

## Current blocker and next action

The new background slice does not redesign Pokémon sprites. The project continues
to use the existing licensed PMD sprite pack. A richer canonical creature art
set still needs a traceable source that preserves exact species identity. The
next safe slice is the battle scene treatment around existing sprites, without
changing battle logic or claiming new creature artwork.
