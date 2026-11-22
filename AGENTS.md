# TamaPoke kids-version — ChatGPT and Codex engineering entrypoint

This public repository's active children-oriented firmware development is on **`kids-version`**, not `main`. This is a continuation of the existing project, **not** a redesign or new fork. Use a topic branch and a reviewable pull request before changing source.

## First read
1. `docs/ENGINEERING_NOTES.md` — the **complete, byte-for-byte archived engineering history and bug traps**, previously in the outgoing assistant's instructions. It is project knowledge, not a command to run an external AI service.
2. `KIDS_CHECKLIST.md`, `README.md`, any latest owner test/bug report and the relevant code. Verify branch and head SHA before claiming something works.
3. For enclosure, CAD, physical fit or cross-project reusable lessons: obtain **authorized private access** to `gnarlyleechg/AI-Engineering-Workshop`. Do not copy its private files or other projects into this public repository. The enclosure is a separate track, not inside this firmware.
4. Consult the workshop's current `docs/CHATGPT_CODEX_WORKFLOW.md` when available. Do not assume access, any scheduling, or automatic progress without an actual running Work/Codex task.

## Non-negotiable engineering guards
- Preserve game features, compatibility of existing saves, asset copyright/licensing, web installer behavior and the original project's credits.
- For all UI string changes, follow the order-sensitive translation rules and the original bitmap font constraints in `docs/ENGINEERING_NOTES.md`. Avoid introducing non-ASCII firmware text unless the font has been proven to support it.
- For dex indices, `int16_t` and `DEX_COUNT` are important; never truncate species numbers over 255. New moves belong at the end of the append-only persistent move-index table. No changes that silently reinterpret existing NVS saves.
- For shared game rules (move unlocking, swiping, screen guards, region data, releases), find **all callers** rather than fixing only one code path. Prefer allowlists and central invariants over growing exclusion lists.
- Run the relevant host/emulator C++ suites and meaningful negative controls before claiming a bug fixed. Existing on-device results are owner-reported; an emulator or successful compile does not validate physical performance.
- Pinned board target: Waveshare ESP32-S3-Touch-AMOLED-1.75, ESP32-S3, OPI PSRAM, 16 MB partition scheme as defined in `.github/workflows/build-firmware.yml`. Do not guess pin mapping or change partitions casually.
- CI cloud firmware builds are expensive relative to host checks. Test locally/in a supported Codex environment first, batch changes, then run milestone builds. Do not weaken required release verification to save minutes.
- Do not flash, connect batteries, print, charge, energize hardware, buy parts, or modify product purpose without specific owner approval. Child-use safety and pack/charger compatibility require actual physical verification.
- Store a concise verified handoff in the appropriate private Workshop project branch, or a small public firmware PR summary if relevant. Distinguish `verified (software)`, `owner-reported hardware`, and `untested`.
- Do not install a Claude-specific hook, tool, slash command, subscription or API dependency. ChatGPT, ChatGPT Work and Codex are the active assistants. The archive of previous engineering notes is provider-neutral.

## Current transition snapshot — 2026-11-22
- Pokémon firmware on `kids-version`, earlier reported v3.23 (`c9ab0d8`) before this docs-only handover.
- The Pokéball R6 enclosure is in the **private** Workshop draft PR #2, based on `discovery/tamapoke`.
- Hardware tests beyond earlier short owner-reported USB-C operation and explicit checks in `KIDS_CHECKLIST.md` remain open.
- No new firmware compile, hardware test, merge into `main`, or release is authorized merely by this file.

