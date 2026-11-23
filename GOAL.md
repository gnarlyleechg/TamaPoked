# Codex Goal — Asher's TamaPoke: touch/chess reliability

**State:** DEFINED IN GITHUB, **NOT YET ACTIVATED IN A CODEX THREAD**.
**Repository:** public `gnarlyleechg/TamaPoked`; active firmware source `kids-version`, not `main`. Existing PR #2 `codex/touch-brightness-timing` concerns chess move confirmation and accidental dimming; inspect it before changing overlapping code.

## Outcome
Produce a **digitally verified, child-friendly Pokémon firmware update** that makes chess piece selection and destination confirmation reliable on the small screen, avoids unintended moves and touch-triggered dimming, and preserves the existing Pokémon game, saved progress, menus, battles, sound and installer behavior.

## Evidence required to finish this Goal
- [ ] Review the current firmware branch, owner-reported touch behavior, PR #2, `docs/ENGINEERING_NOTES.md` and `KIDS_CHECKLIST.md`. Reuse working code rather than restarting the game.
- [ ] Implement or review a clear confirm/cancel choice **after selecting the destination** (not merely highlighting a chess piece), preserving chess legality and existing controls; test accidental taps, invalid destination, cancel, legal/illegal moves, restart and display bounds in the real-source emulator.
- [ ] Run the relevant host/emulator regression suites and current-source firmware build; verify no regressions in daily coins, battery-dimming logic, NVS saves, translations/fonts, SD installer packaging and controls. Record the actual commit SHA, tests, and known board-only gaps.
- [ ] Prepare a reviewable PR and concise test/owner-hardware checklist for touchscreen accuracy, screen edge clipping, battery/dimming interaction and real-device chess, without marking board-only checks as passed.

## Constraints and iteration
Keep all original game credits and licences. This is public source: **never copy private Workshop documents, photos, board measurements or secrets** into this repo. Preserve append-only move indices, `DEX_COUNT`/int16_t species numbers and NVS compatibility. Do not rebase/merge another engineer's PR without review. Avoid unnecessary firmware rebuilds on docs-only changes. Continue useful safe digital tests and fixes until the defined code/CI outcome is supported.

## Stop conditions
Done on the firmware/emulator and PR evidence above; real on-device touch, sound, SD and battery verification remains owner-only. Stop on a specific hardware, user decision, quota or platform blocker and document exactly what is still needed. No flashing, physical power/battery, child safety signoff, payments, or unapproved release/merge.

## Activate in Codex
In a supported Codex thread opened on `TamaPoked/kids-version`, enter:

`/goal Finish Asher's TamaPoke chess/touch reliability milestone in GOAL.md using the latest PR #2, real-source tests and save-compatible firmware. Continue safe fixes until all digital evidence is complete; stop only on a specific verified hardware/owner or resource blocker.`

A GitHub `GOAL.md` **does not activate a native Goal**.