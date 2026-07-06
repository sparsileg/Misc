# Legacy `.PIC` File Format — Structure & Recovery Notes

## Background

These `.PIC` files were produced by an old Windows application (name unknown). Despite the `.PIC` extension, each file is a hybrid format: a BMP container with an embedded JPEG payload, missing its quantization tables.

---

## File Structure

| Offset   | Length     | Content                                                                                                                        |
| -------- | ---------- | ------------------------------------------------------------------------------------------------------------------------------ |
| `0x0000` | 2 bytes    | BMP signature: `42 4D` ("BM")                                                                                                  |
| `0x0002` | 12 bytes   | BMP header fields (file size, reserved, pixel data offset)                                                                     |
| `0x000E` | 54 bytes   | BMP DIB header (dimensions, color planes, bit depth, compression)                                                              |
| `0x0012` | 4 bytes    | ASCII string `"JPEG"` embedded in the DIB header                                                                               |
| `0x0050` | ~944 bytes | BGR color palette data (256 entries × 3 bytes + 1 padding byte each)                                                           |
| `0x0402` | 2 bytes    | JPEG SOI marker: `FF D8`                                                                                                       |
| `0x0404` | 18 bytes   | Malformed JFIF APP0 segment (`FF E0`) — states length 16 but contains extra null bytes                                         |
| `0x0416` | 13 bytes   | Custom PIC APP1 segment (`FF E1`) — contains string `"PIC"` and 6 unknown bytes, possibly a back-reference into the BMP header |
| `0x0423` | —          | JPEG image data (SOF0, Huffman tables, SOS scan data)                                                                          |
| EOF      | 2 bytes    | JPEG EOI marker: `FF D9`                                                                                                       |

### Key quirks

- **No `FF DB` (DQT) quantization table markers anywhere in the file.** The original application hardcoded its quantization tables in the decoder and never wrote them into the file.
- The JFIF APP0 segment is non-standard: it declares a length of 16 bytes but contains extra null padding, causing conformant JPEG parsers to lose sync with the marker stream.
- The custom APP1 `"PIC"` segment confuses parsers that encounter it after resynchronizing from the malformed APP0.

---

## Why Standard Tools Failed

| Tool                     | Failure reason                                    |
| ------------------------ | ------------------------------------------------- |
| Rename to `.jpg`         | Missing quantization tables; parsers abort        |
| `jpegtran -copy none`    | Same — no DQT means no valid JPEG to re-encode    |
| `identify` (ImageMagick) | Reports "Quantization table 0x00 was not defined" |

---

## Fix Applied (`convert_pic.py`)

1. **Skip the BMP wrapper** — the JPEG stream begins at a fixed offset of `0x0402` in all files from this application.

2. **Replace the malformed APP0** — discard the original JFIF APP0 and substitute a clean, standards-compliant one:
   
   ```
   FF E0 00 10 4A 46 49 46 00 01 01 00 00 01 00 01 00 00
   ```

3. **Inject standard quantization tables** — insert two `FF DB` DQT segments immediately after the APP0, using the standard IJG/libjpeg quality-50 baseline tables:
   
   - Table 0: luminance (Y channel)
   - Table 1: chrominance (Cb/Cr channels)

4. **Strip the custom PIC APP1 segment** — remove the `FF E1 "PIC"` segment entirely, as it carries no image data and confuses parsers.

5. **Preserve all remaining JPEG data unchanged** — the SOF0, Huffman tables, and scan data are written out verbatim.

---

## Batch Conversion

Run `convert_pic.py` in any folder containing `.PIC` files:

```bash
python3 convert_pic.py
```

It will produce a `.jpg` alongside each `.PIC` file. No dependencies beyond the Python standard library.

---

## Notes & Caveats

- The injected quantization tables are the standard IJG quality-50 tables, not the originals (which were never stored). The resulting images appear visually correct, with no meaningful difference from what the original application would have displayed.
- All files from this application appear to share the same fixed JPEG offset (`0x0402`). If a file from a different version of the application fails, check the offset with `grep -boa $'\xff\xd8\xff' file.PIC`.
- The BGR palette data in the BMP header (`0x0050`–`0x03FF`) appears to be unused by the embedded JPEG and can be ignored.
