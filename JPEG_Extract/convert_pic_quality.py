"""
convert_pic_quality.py

Converts all .PIC files in the current directory to JPEG at multiple quality
levels, so you can compare them and find the closest match to the original.

Output filenames: <basename>_q<quality>.jpg
Example: 23_q50.jpg, 23_q75.jpg, 23_q95.jpg

Usage:
    python3 convert_pic_quality.py

Requires: Pillow
    pip install Pillow
"""

import struct
import glob
import io
from PIL import Image

# Quality levels to try
QUALITY_LEVELS = [50, 55, 60, 65, 70, 75, 80, 85, 90, 95]

# Standard JFIF APP0 replacement
APP0 = b'\xff\xe0\x00\x10JFIF\x00\x01\x01\x00\x00\x01\x00\x01\x00\x00'

# Standard IJG luminance quantization table (quality 50 baseline)
LUM_QT = bytes([
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,109,103, 77,
    24, 35, 55, 64, 81,104,113, 92,
    49, 64, 78, 87,103,121,120,101,
    72, 92, 95, 98,112,100,103, 99
])

# Standard IJG chrominance quantization table (quality 50 baseline)
CHR_QT = bytes([
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

DQT = make_dqt(0, LUM_QT) + make_dqt(1, CHR_QT)


def extract_jpeg(path):
    """Extract and repair the raw JPEG stream from a .PIC file."""
    with open(path, 'rb') as f:
        data = f.read()

    jpeg = data[0x0402:]

    # Skip the malformed APP0
    orig_app0_len = struct.unpack('>H', jpeg[4:6])[0]
    pos = 2 + 2 + orig_app0_len

    # Skip any null padding
    while pos < len(jpeg) and jpeg[pos] != 0xff:
        pos += 1

    # Walk segments, stripping the custom PIC APP1
    rest = b''
    while pos < len(jpeg) - 1:
        if jpeg[pos] != 0xff:
            rest += jpeg[pos:]
            break
        marker = jpeg[pos:pos+2]
        if marker == b'\xff\xda':
            rest += jpeg[pos:]
            break
        seg_len = struct.unpack('>H', jpeg[pos+2:pos+4])[0]
        seg_end = pos + 2 + seg_len
        if marker != b'\xff\xe1':
            rest += jpeg[pos:seg_end]
        pos = seg_end

    return b'\xff\xd8' + APP0 + DQT + rest


def decode_pic(path):
    """Extract the JPEG from a .PIC file and decode it into a PIL Image."""
    jpeg_bytes = extract_jpeg(path)
    return Image.open(io.BytesIO(jpeg_bytes))


def convert_all():
    pics = glob.glob('*.PIC')
    if not pics:
        print("No .PIC files found in the current directory.")
        return

    for pic in pics:
        basename = pic.replace('.PIC', '')
        print(f"\nProcessing {pic}...")

        try:
            img = decode_pic(pic)
        except Exception as e:
            print(f"  ERROR decoding {pic}: {e}")
            continue

        for quality in QUALITY_LEVELS:
            out = f"{basename}_q{quality}.jpg"
            try:
                img.save(out, 'JPEG', quality=quality, subsampling=0)
                print(f"  Saved {out}")
            except Exception as e:
                print(f"  ERROR saving {out}: {e}")

    print("\nDone. Compare the _q## files to find the best match.")


if __name__ == '__main__':
    convert_all()
