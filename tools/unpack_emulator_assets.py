#!/usr/bin/env python3
"""Extract existing installer sprite bundles for host tests; no new downloads."""
import argparse
import re
import struct
from pathlib import Path

NAME = re.compile(r"mons/(?:p?s?\d{3}\.bin|thumbs\.bin)\Z")

def unpack(raw):
    if len(raw) < 6 or raw[:4] != b"TPAK":
        raise ValueError("bad bundle magic")
    count = struct.unpack_from("<H", raw, 4)[0]
    pos, entries = 6, []
    names = set()
    for _ in range(count):
        if pos >= len(raw):
            raise ValueError("truncated name length")
        length = raw[pos]
        pos += 1
        if pos + length + 4 > len(raw):
            raise ValueError("truncated index")
        name = raw[pos:pos+length].decode("ascii")
        pos += length
        size = struct.unpack_from("<I", raw, pos)[0]
        pos += 4
        if not NAME.fullmatch(name) or name in names or not size:
            raise ValueError("invalid or duplicate entry: " + name)
        names.add(name)
        entries.append((name, size))
    result = {}
    for name, size in entries:
        if pos + size > len(raw):
            raise ValueError("truncated payload")
        result[name.split("/")[1]] = raw[pos:pos+size]
        pos += size
    if pos != len(raw) or not result:
        raise ValueError("trailing bytes or empty bundle")
    return result

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("bundles", nargs="+", type=Path)
    a = p.parse_args()
    a.out.mkdir(parents=True, exist_ok=True)
    count = 0
    for bundle in a.bundles:
        for name, data in unpack(bundle.read_bytes()).items():
            dest = a.out / name
            if dest.exists() and dest.read_bytes() != data:
                raise ValueError("conflicting bundled asset: " + name)
            if not dest.exists():
                dest.write_bytes(data)
                count += 1
    if not (a.out / "p258.bin").is_file() or not (a.out / "thumbs.bin").is_file():
        raise ValueError("required canonical sprite test fixtures missing")
    print(f"Restored {count} loose assets from existing bundles")

if __name__ == "__main__":
    main()
