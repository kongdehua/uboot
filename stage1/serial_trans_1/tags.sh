#!/bin/bash

ROOT=$PWD

echo ${ROOT}

rm cscope.*
rm tags

find ${ROOT} \
	-path "$ROOT/*/*"                                                 -o \
	-name "*.[.chsS]" -print > $ROOT/cscope.files

# generate the cscope file
cscope -bkq -i $ROOT/cscope.files

# generate the cppcomplete
ctags -n -f tags --fields=+ai -L $ROOT/cscope.files

#	-path "$UBOOT/board/100ask24x0/*" -prune -o\
