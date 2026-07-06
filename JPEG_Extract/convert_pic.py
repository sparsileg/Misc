import struct, glob, os

lum_qt = bytes([
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,109,103, 77,
    24, 35, 55, 64, 81,104,113, 92,
    49, 64, 78, 87,103,121,120,101,
    72, 92, 95, 98,112,100,103, 99
])

chr_qt = bytes([
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
])

def make_dqt(table_id, qtable):
    payload = bytes([table_id]) + qtable
    return b'\xff\xdb' + struct.pack('>H', len(payload) + 2) + payload

dqt = make_dqt(0, lum_qt) + make_dqt(1, chr_qt)

# Standard JFIF APP0 segment (1:1 pixel ratio, no thumbnail)
app0 = b'\xff\xe0\x00\x10JFIF\x00\x01\x01\x00\x00\x01\x00\x01\x00\x00'

def convert(path):
    with open(path, 'rb') as f:
        data = f.read()

    jpeg = data[0x0402:]

    # Skip the malformed APP0 (has extra null bytes beyond its stated length)
    orig_app0_len = struct.unpack('>H', jpeg[4:6])[0]
    pos = 2 + 2 + orig_app0_len

    # Skip any null padding until we hit a real marker
    while pos < len(jpeg) and jpeg[pos] != 0xff:
        pos += 1

    # Walk remaining segments, stripping the custom PIC APP1 segment
    rest = b''
    while pos < len(jpeg) - 1:
        if jpeg[pos] != 0xff:
            rest += jpeg[pos:]
            break
        marker = jpeg[pos:pos+2]
        if marker == b'\xff\xda':
            # Start of scan — copy everything from here to end
            rest += jpeg[pos:]
            break
        seg_len = struct.unpack('>H', jpeg[pos+2:pos+4])[0]
        seg_end = pos + 2 + seg_len
        if marker != b'\xff\xe1':
            rest += jpeg[pos:seg_end]
        pos = seg_end

    return b'\xff\xd8' + app0 + dqt + rest

pics = glob.glob('*.PIC')
if not pics:
    print("No .PIC files found in current directory.")
else:
    for pic in pics:
        out = pic.replace('.PIC', '.jpg')
        data = convert(pic)
        with open(out, 'wb') as f:
            f.write(data)
        print(f"Converted {pic} -> {out} ({len(data)} bytes)")
    print("Done.")
