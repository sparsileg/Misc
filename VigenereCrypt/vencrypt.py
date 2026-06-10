#!/usr/bin/python

"""This reads a plaintext file, ptext.txt, and uses the Vigenere
Cipher to encrypt and write out to the pyctext.txt file. Note that the
key is in hex, the plain text is in ascii, and the output cipher text
in hex. New lines are skipped.

"""

from binascii import hexlify

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

def is_valid_character( ptext ):
    """ possible filter on no numbers (48 thru 57)
    """
    # ascii printable characters and a newline
    if (ptext >= 32 and ptext <= 127) or ptext == 10 or ptext == 13:
        retval = True
    else:
        retval = False

    return retval


key=[0x20,0x7A,0xB4,0x1F,0x27,0xee,0x9c,0xd1]
KEY_LENGTH = len(key)

fpIn = open('bom.txt','r')
fpOut = open('ebom.txt','w')

i = 0
total_chars = 0
while True:
    p = fpIn.read(1)
    if not p: break
    total_chars += 1
#    if p != '\n':
    if not is_valid_character( ord(p) ):
        print "Character: {0} is invalid: {1}".format(p,ord(p))
    c = hexlify( chr( ord(p) ^ key[ i % KEY_LENGTH ])).upper()
    fpOut.write(c)
    i += 1

fpIn.close()
fpOut.close()

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
