#!/usr/bin/python

"""This reads a ciphertext file, pyctext.txt, and uses the Vigenere
Cipher to decrypt and write plaintext to the pyptext.txt file. Note
that the key is in hex, the ciphertext is in hex, and the output plain
text in ascii.
"""

import sys

#     59, 174,  66,  31, 114, 254,  80,  29
key=[0x3B,0xAE,0x42,0x1F,0x72,0xfe,0x50,0x1d]
key=[0xba,0x1f,0x91,0xb2,0x53,0xcd,0x3e]
KEY_LENGTH = len(key)

fpIn = open('ciphertext.txt','r')
ki = 0
while True:
    c = fpIn.read(2)
    if not c: break
    p = chr( int( c, 16 ) ^ key[ ki % KEY_LENGTH ])
    sys.stdout.write(p)
    ki += 1

sys.stdout.write('\n')
fpIn.close()

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
