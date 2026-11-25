#!/usr/bin/env python3
"""Validate bundled TPK2 clips and count missing authored animations."""

import argparse
from collections import Counter
from pathlib import Path
import struct

ACTIONS = {
    0: "idle", 1: "walk left", 2: "walk right", 3: "sleep",
    4: "eat", 5: "hurt", 6: "attack",
}


def inspect_sprite(blob):
    if len(blob) < 7 or blob[:4] != b"TPK2":
        raise ValueError("invalid TPK2 header")
    count, palette_size = struct.unpack_from("<BH", blob, 4)
    offset = 7 + palette_size * 2
    if not 0 < palette_size <= 256 or offset > len(blob):
        raise ValueError("invalid palette")

    actions = set()
    for _ in range(count):
        if offset + 4 > len(blob):
            raise ValueError("truncated action header")
        action, width, height, frames = struct.unpack_from("4B", blob, offset)
        if action in actions or action >= 12 or not width or not height or not 0 < frames <= 24:
            raise ValueError(f"invalid action header for clip {action}")

        time_start = offset + 4
        pixel_start = time_start + frames * 2
        frame_size = width * height
        offset = pixel_start + frames * frame_size
        if offset > len(blob):
            raise ValueError(f"truncated clip {action}")
        durations = struct.unpack_from(f"<{frames}H", blob, time_start)
        if not all(durations):
            raise ValueError(f"zero duration in clip {action}")
        for start in range(pixel_start, offset, frame_size):
            indices = set(blob[start:start + frame_size]) - {255}
            if not indices or max(indices) >= palette_size:
                raise ValueError(f"empty frame or invalid index in clip {action}")
        actions.add(action)

    if offset != len(blob) or 0 not in actions:
        raise ValueError("trailing data or missing idle clip")
    return actions


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sprites", type=Path, help="directory restored by unpack_emulator_assets.py")
    args = parser.parse_args()
    normal = {path.stem[1:]: path for path in args.sprites.glob("p[0-9][0-9][0-9].bin")}
    shiny = {path.stem[2:]: path for path in args.sprites.glob("ps[0-9][0-9][0-9].bin")}
    if not normal or not shiny or shiny.keys() - normal.keys():
        parser.exit(1, "Missing sprite files or shiny files without a normal counterpart\n")

    for label, files in (("Normal", normal), ("Shiny", shiny)):
        coverage = Counter()
        for path in sorted(files.values()):
            try:
                coverage.update(inspect_sprite(path.read_bytes()))
            except ValueError as error:
                parser.exit(1, f"{path.name}: {error}\n")
        print(f"{label}: {len(files)} sprites")
        for action, name in ACTIONS.items():
            print(f"  {name}: {coverage[action]} authored, {len(files) - coverage[action]} missing")
    print(f"Shiny variants using the normal sprite: {len(normal.keys() - shiny.keys())}")


if __name__ == "__main__":
    main()
