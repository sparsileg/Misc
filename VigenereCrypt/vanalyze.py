#!/usr/bin/python

import sys

class VigenereCipher(object):

    DEBUG = False
    MIN_KEY_LENGTH = 1
    MAX_KEY_LENGTH = 13

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def __init__( self, efn=None ):
        self.encrypted_filename = efn
        self.dkey_length = 0
        self.dkey = []
        self.elength = 0
        self.edata = self.read_encrypted_file()

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def read_encrypted_file( self ):
        fpIn = open( self.encrypted_filename, 'r' )
        self.edata = fpIn.read()
        self.elength = len( self.edata )
        fpIn.close()

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def generate_summation( self, key_length ):
        if self.edata is None: self.read_encrypted_file()
        total_chars = 0
        freq = [0 for i in xrange(256)]

        i=0
        while i < (self.elength-(2*key_length)):
            total_chars += 1
            c = self.edata[i:i+2]
            p = int( c, 16 )
            freq[p] += 1
            i += int( 2 * key_length )

        freq = [float(freq[i])/float(total_chars) for i in xrange(256)]
        freq = [ freq[i] * freq[i] for i in xrange(256)]

        sum = 0
        for i in xrange(256): sum += freq[i]
        if self.DEBUG: print "key_length:" + str(key_length) + "; total_chars:" + str(total_chars) + "; sum:" + str(sum)
        return sum

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def guess_key_length( self ):
        max_sum = -1
        i = self.MIN_KEY_LENGTH
        while i <= self.MAX_KEY_LENGTH:
            sum = self.generate_summation( i )
            if sum > max_sum:
                max_sum = sum
                the_key_length = i
                if self.DEBUG: print "New max: " + str(i) + ": " + str(max_sum)
            i += 1

        self.dkey_length = the_key_length

        if self.DEBUG:
            print "Suspected key length: {0}".format(the_key_length)
            print "Remember that multiples may mask the true key length\n"


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def generate_encrypted_lc_frequencies( self ):
        """ generate frequencies of lower case letters """

        freq = [0 for i in xrange(256)]
        lca = ord('a')
        lcz = ord('z')
        total_chars = 0
        i = 0
        while i < self.elength:
            c = self.edata[i:i+2]
            numc = int(c,16)
            if lca <= numc and numc <= lcz:
                total_chars += 1
                freq[numc] += 1
            i += 2

        if total_chars > 0:
            freq = [float(freq[i])/float(total_chars) for i in xrange(256)]
        else:
            freq = [ 0 for i in xrange(256) ]

        return freq


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def generate_plaintext_lc_frequencies( self, stream ):
        """ generate frequencies of lower case letters """

        freq = [0 for i in xrange(256)]
        lca = ord('a')
        lcz = ord('z')
        total_chars = 0
        i = 0
        while i < len(stream):
            c = stream[i]
            numc = ord( c )
            if lca <= numc and numc <= lcz:
                total_chars += 1
                freq[numc] += 1

            i += 1

        if total_chars > 0:
            freq = [float(freq[i])/float(total_chars) for i in xrange(256)]
        else:
            freq = [ 0 for i in xrange(256) ]

        return freq


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def is_valid_character( self, ptext ):
        """ possible filter on no numbers (48 thru 57)
        """
        if (ptext >= 32 and ptext <= 127) or ptext == 10 or ptext == 13:
            retval = True
        else:
            retval = False

        return retval

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def determine_ith_byte( self, ith_byte ):
        qi = self.generate_encrypted_lc_frequencies()
        streams = {}
        for keyint in xrange(0,256):
            j = ith_byte*2
            pstream = []
            append = True
            while j < (self.elength-(2*self.dkey_length)):
                c = self.edata[j:j+2]
                p = int( c, 16 ) ^ keyint
                if self.is_valid_character( p ):
                    pstream.append( chr(p) )
                else:
                    append = False
                    break
                j += int( 2 * self.dkey_length )

            # if there are no spaces on an appendable stream, we can
            # throw it out. this possibly may remove a solution, but
            # the odds are in our favor.
            if append and ' ' in pstream:
                streams[keyint] = pstream

        # for all the streams that passed, figure out the correlation
        keybyte = None
        themin = 1.0
        for s in streams:
            pi = self.generate_plaintext_lc_frequencies( streams[s] )
            dotproduct2 = sum( [ pi[x]*pi[x] for x in xrange(256) ] )
            #if self.DEBUG: print str(s) + "--::" + streams[s]
            if self.DEBUG: print str(s) + "--::"
            if self.DEBUG: print "Dot Product2: " + str(dotproduct2) + "\n"
            if abs(dotproduct2 - 0.065) < themin:
                themin = abs(dotproduct2 - 0.065)
                keybyte = hex(s)
                keybyte = s

        if keybyte is None: print dotproduct2, themin
        if self.DEBUG:
            if keybyte is not None:print "ith: " + str(ith_byte) + " is " + str(keybyte) + "\n"

        return keybyte

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def append_to_dkey( self, value ):
        self.dkey.append(value)

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def build_decryption_key( self ):
        for ith_byte in xrange(self.dkey_length):
            keybyte = self.determine_ith_byte( ith_byte )
            self.append_to_dkey(keybyte)

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def decrypt( self ):
        ki = 0
        i = 0
        while i < self.elength:
            c = self.edata[i:i+2]
            if not c: break
            p = chr( int( c, 16 ) ^ self.dkey[ ki % self.dkey_length ])
            sys.stdout.write(p)
            ki += 1
            i += 2

        sys.stdout.write('\n')

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

    def output_encrypted_data( self ):
        if self.edata is None: self.read_encrypted_file()
        print self.edata
        print "\n\n"

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

if __name__ == "__main__":
#    va = VigenereCipher( efn='ciphertext.txt' )
    va = VigenereCipher( efn='e2nephi.txt' )
#    va = VigenereCipher( efn='ebom.txt' )
    va.guess_key_length()
    va.build_decryption_key()
    va.decrypt()

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
