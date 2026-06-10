#!/bin/bash

# IV is fixed at 16 characters

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# dynamic encrypt

function dencrypt() {

    function bundle() {

		[ $# -ne 2 ] && echo "bundle <inputfile> <outputbundle>" && exit 1
		[ ! -e $1 ] && echo "No input file: $1" && exit 1

		if=$1
		of=$2
		local TMPDIR=$(mktemp -d)

		_BUNDLE_KEY=$( echo $(sha512sum "/usr/include/bits/semaphore.h")"jumble" | sha256sum | cut -d" " -f 1)
		iv=$(openssl rand 8 | xxd -ps)
		echo -n $iv >  ${TMPDIR}/iv

		openssl enc -aes-256-cbc -in $if -out  ${TMPDIR}/_input.enc -K ${_BUNDLE_KEY} -iv $iv

		cat ${TMPDIR}/iv ${TMPDIR}/_input.enc > $of
		shred -fuz  ${TMPDIR}/*
		rm -rf $TMPDIR

		return 0
    }

    [ $# -ne 2 ] && echo "$0 <inputfile> <outputbundle>" && exit 1
    bundle $1 $2
}


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# dynamic decrypt

function ddecrypt() {

    function unbundle() {

		if=$1
		of=$2

		local IVLEN=16
		local TMPDIR=$(mktemp -d)

		_BUNDLE_KEY=$( echo $(sha512sum "/usr/include/bits/semaphore.h")"jumble" | sha256sum | cut -d" " -f 1)
		dd if=${if} bs=1 count=$IVLEN > ${TMPDIR}/iv 2>/dev/null
		dd if=${if} bs=1 skip=$IVLEN >  ${TMPDIR}/__temp.enc 2>/dev/null
		iv=$(cat  ${TMPDIR}/iv)

		openssl aes-256-cbc -d -in  ${TMPDIR}/__temp.enc -out $of -K ${_BUNDLE_KEY} -iv $iv

		# delete unneeded files
		shred -fuz  ${TMPDIR}/*
		rm -rf $TMPDIR

		return 0
    }

    [ $# -ne 2 ] && echo "$0 <encryptedfile> <outputfile>" && exit 1;
    unbundle $1 $2
}


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#


# test variable
DIDITWORK=No

# encrypt inputfile and store as new file (BUNDLE)
SRCFILE=./inputfile
BUNDLE=$(mktemp)
dencrypt $SRCFILE $BUNDLE

# decrypt BUNDLE and source it to test success
DDFILE=$(mktemp)
ddecrypt $BUNDLE $DDFILE

# only for testing if decryption worked
source $DDFILE

# clean up
shred -fuz $DDFILE $BUNDLE

# should be "YES"
echo "Did it work: $DIDITWORK"

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
