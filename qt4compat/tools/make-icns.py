#!/usr/bin/env python3
"""qt4compat/tools/make-icns.py - build a classic .icns from PNG files.

    make-icns.py out.icns 16.png 32.png 48.png 128.png

Writes the pre-10.7 entries (is32/s8mk, il32/l8mk, ih32/h8mk, it32/t8mk:
24-bit run-length colour data plus an 8-bit mask), which every macOS from
10.0 on reads. sips cannot write icns before 10.7 and libicns' png2icns
writes the 128x128 entry as a PNG-based ic07 that 10.6 ignores, hence this
script; it needs only the Python standard library. PNGs must be 8- or
16-bit RGB(A), non-interlaced, and exactly 16, 32, 48 or 128 pixels square.
The results live in qt4compat/data/*.icns and are used by
kate_macos_app_icon() in cmake/KateQt4Macros.cmake.
"""
import struct
import sys
import zlib

SIZES = {16: (b'is32', b's8mk'), 32: (b'il32', b'l8mk'), 48: (b'ih32', b'h8mk'), 128: (b'it32', b't8mk')}


def read_png(path):
    data = open(path, 'rb').read()
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise SystemExit('%s: not a PNG' % path)
    pos = 8
    idat = b''
    width = height = depth = ctype = None
    while pos < len(data):
        length, ctag = struct.unpack('>I4s', data[pos:pos + 8])
        chunk = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if ctag == b'IHDR':
            width, height, depth, ctype, _, _, interlace = struct.unpack('>IIBBBBB', chunk)
            if interlace:
                raise SystemExit('%s: interlaced PNG not supported' % path)
        elif ctag == b'IDAT':
            idat += chunk
        elif ctag == b'IEND':
            break
    if ctype not in (2, 6) or depth not in (8, 16):
        raise SystemExit('%s: only 8/16-bit RGB or RGBA PNGs are supported (type %s, depth %s)' % (path, ctype, depth))
    channels = 4 if ctype == 6 else 3
    bps = depth // 8
    stride = width * channels * bps
    raw = zlib.decompress(idat)
    rows = []
    prev = bytearray(stride)
    p = 0
    for _ in range(height):
        ftype = raw[p]
        line = bytearray(raw[p + 1:p + 1 + stride])
        p += 1 + stride
        bpp = channels * bps
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            c = prev[i - bpp] if i >= bpp else 0
            if ftype == 1:
                line[i] = (line[i] + a) & 0xff
            elif ftype == 2:
                line[i] = (line[i] + b) & 0xff
            elif ftype == 3:
                line[i] = (line[i] + (a + b) // 2) & 0xff
            elif ftype == 4:
                pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                pred = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                line[i] = (line[i] + pred) & 0xff
        rows.append(bytes(line))
        prev = line
    # to 8-bit RGBA
    pixels = []
    for line in rows:
        for x in range(width):
            base = x * channels * bps
            px = [line[base + k * bps] for k in range(channels)]  # high byte of 16-bit samples
            if channels == 3:
                px.append(255)
            pixels.append(px)
    return width, height, pixels


def rle_channel(values):
    """Apple's ICNS packbits variant: literal runs (n-1, then n bytes, n<=128)
    and repeat runs (n-3+128, then one byte, 3<=n<=130)."""
    out = bytearray()
    i = 0
    n = len(values)
    while i < n:
        run = 1
        while i + run < n and run < 130 and values[i + run] == values[i]:
            run += 1
        if run >= 3:
            out.append(run - 3 + 128)
            out.append(values[i])
            i += run
            continue
        start = i
        while i < n and i - start < 128:
            run = 1
            while i + run < n and run < 3 and values[i + run] == values[i]:
                run += 1
            if run >= 3:
                break
            i += 1
        out.append(i - start - 1)
        out.extend(values[start:i])
    return bytes(out)


def entry(tag, payload):
    return tag + struct.pack('>I', 8 + len(payload)) + payload


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    out = sys.argv[1]
    body = b''
    for path in sys.argv[2:]:
        w, h, px = read_png(path)
        if w != h or w not in SIZES:
            raise SystemExit('%s: %dx%d is not one of 16, 32, 48, 128' % (path, w, h))
        ctag, mtag = SIZES[w]
        rgb = b''.join(rle_channel(bytes(p[k] for p in px)) for k in range(3))
        if ctag == b'it32':
            rgb = b'\0\0\0\0' + rgb  # it32 carries four leading zero bytes
        body += entry(ctag, rgb)
        body += entry(mtag, bytes(p[3] for p in px))
    open(out, 'wb').write(b'icns' + struct.pack('>I', 8 + len(body)) + body)
    print('wrote %s (%d bytes, %d images)' % (out, 8 + len(body), len(sys.argv) - 2))


if __name__ == '__main__':
    main()
