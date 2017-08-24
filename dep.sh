#!/bin/sh

BASE_FETCH_URL="https://gist.githubusercontent.com/StefanoBelli/c12965643c86044719c75fcaaa7d0178/raw"
FILES="linux-keys.c linux-keys.h"
GET_RC="curl -o"

for file in $FILES; do
	echo " --> Fetching: $file"
	if [ -f $file ]; then
		echo "  $file: already exists"
		continue
	fi

	if ! $GET_RC $file ${BASE_FETCH_URL}/${file} 2>/dev/null >>/dev/null; then
		exit 2
	fi
done

