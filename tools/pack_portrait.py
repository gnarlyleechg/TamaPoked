#!/usr/bin/env python3
"""Convert a locally authorized canonical transparent PNG into an optional TPA1 asset."""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from PIL import Image

MAX_SIDE = 192

def encode(image, dex, shiny=False):
    if not 1 <= dex <= 999:
        raise ValueError("dex must be 1..999")
    image = image.convert("RGBA")
    alpha = image.getchannel("A")
    box = alpha.getbbox()
    if box is None:
        raise ValueError("empty image")
    image = image.crop(box)
    image.thumbnail((MAX_SIDE, MAX_SIDE), Image.Resampling.LANCZOS)
    w, h = image.size
    data = bytearray(struct.pack("<4sHHHBB", b"TPA1", dex, w, h, int(shiny), 0))
    visible = False
    for r, g, b, a in image.getdata():
        opaque = a >= 128
        visible |= opaque
        color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3) if opaque else 0
        data.extend(struct.pack("<HB", color, 255 if opaque else 0))
    if not visible:
        raise ValueError("no opaque pixels after conversion")
    return bytes(data)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("png", type=Path)
    p.add_argument("--dex", type=int, required=True)
    p.add_argument("--shiny", action="store_true")
    p.add_argument("--source", required=True, help="exact source URL or local provenance")
    p.add_argument("--rights", required=True, help="rights/permission or personal-use restriction")
    p.add_argument("--out", type=Path, required=True, help="local SD mons directory")
    a = p.parse_args()
    if not a.source.strip() or not a.rights.strip():
        p.error("nonempty source and rights required")
    raw = a.png.read_bytes()
    with Image.open(a.png) as image:
        data = encode(image, a.dex, a.shiny)
    name = f"a{'s' if a.shiny else ''}{a.dex:03d}.tpa"
    a.out.mkdir(parents=True, exist_ok=True)
    manifest = a.out / "portrait-manifest.json"
    entries = json.loads(manifest.read_text()) if manifest.exists() else {}
    if not isinstance(entries, dict):
        raise ValueError("invalid existing portrait manifest")
    entries[name] = {"dex": a.dex, "shiny": a.shiny, "source": a.source,
                     "rights": a.rights, "source_sha256": hashlib.sha256(raw).hexdigest(),
                     "asset_sha256": hashlib.sha256(data).hexdigest()}
    (a.out / name).write_bytes(data)
    manifest.write_text(json.dumps(entries, indent=2, sort_keys=True) + "\n")
    print(f"{name}: {len(data)} bytes (local only)")

if __name__ == "__main__":
    main()
