# Painted environment artwork checkpoint

Original AI-generated painted art sheet, 6 columns by 5 rows, 1374x1145. Cells 0-2 are the training environments; cells 3-28 are the 26 environment themes in the baker's explicit order. Cell 29 is unused. The 91 trainer plates reuse those themes with location-specific framing and grading. They are distinct raster assets, not 91 independently painted originals.

## Asset handoff

The binary archives were exported to the owner via Chat, not uploaded to GitHub: the available GitHub connection supports text edits but does not expose a direct container-file binary upload. Preserve these archives with the project before the next build.

- tamapoke_painted_sd_pack.zip SHA-256: 488b50042b3f5e3312c3d35f5f63961938d90eff76ecfa7f52ead90ebb1d7756
- tamapoke_painted_artwork_source.zip SHA-256: 45f3c1cf55bb92465ff6bdab802b05366321aa5c03269fb25cc2d9b3271a5b87
- painted_atlas.png SHA-256: 26bfbb58cc59306e7577a1ac9fd109bf8b039d3079789a04b14407694e05aa70
- artwork-manifest.json SHA-256: a6af84aa3efed894bec1dfcb13fabd0853d5511af0a44930392b5656bbf41f97

Rebuild using Python with Pillow and NumPy:

    python tools/bake_painted_scenes.py painted_atlas.png output
    g++ -std=c++17 -O2 -Wall -Wextra -Werror tools/emu/tests/painted_scenes_test.cpp -o /tmp/tpa-test
    /tmp/tpa-test output/scenes

For the default emulator, copy output/scenes to tools/sdcard/scenes. The painted loader uses the default compiled sprite directory's sibling scenes folder; it does not track the optional --sprites runtime override. For a device, the matching firmware reads /sdcard/scenes/p000.rgb through p093.rgb. Copy only the scenes folder to the microSD root with the unit powered off and an existing card backup. Never format, remove mons or alter saves. Old firmware may ignore the files.

## Format and memory

Each TPA1 plate has a 20-byte little-endian header: magic, ID, width, height, reserved zero, payload bytes, CRC32; followed by row-major little-endian RGB565. Training plates are 233x233, gym plates 233x127. 94 files total 5,713,176 bytes. A night palette grade is applied after load. The existing 108,578-byte PSRAM cache is reused; no additional persistent pixel buffer or save partition is introduced. File I/O, CRC and grading occur only on selection changes; invalid or missing artwork uses procedural fallback. Restart to retry files for the same scene after SD updates.

## Local evidence

Standalone C++17 test and ASan/UBSan test passed with all 94 plates, producing 185 distinct day/night outputs, checking cache replay, guard words, bad magic, dimensions, ID, CRC, truncation, extra bytes, missing files, and fallback. Default no-pack synthetic test passed. ZIP integrity and per-file SHA-256 manifest verified. No firmware image or actual on-device timing was produced. Full emulator and pinned ESP32 build, SD VFS integration, runtime HUD readability and frame timing remain pending. Do not flash or merge without approval. No Actions runs intentionally triggered; use a consolidated local build when available.
