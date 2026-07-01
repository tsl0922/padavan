#!/bin/sh

# check env
if [ -z "$ROMFSDIR" ]; then
	echo "ROMFSDIR is not set" >&2
	exit 1
fi

if [ -z "$ROOTDIR" ]; then
	echo "ROOTDIR is not set" >&2
	exit 1
fi

if [ ! -x "$(command -v $STRIPTOOL)" ] ; then
	echo "STRIPTOOL is not set" >&2
	exit 1
fi

if [ ! -x "$(command -v $OBJCOPY)" ] ; then
	echo "OBJCOPY is not set" >&2
	exit 1
fi

echo "\033[32m======================>>> STRIP ROMFS <<<======================\033[0m"
find ${ROMFSDIR} -type f -a -exec file {} \; | \
  sed -n -e 's/^\(.*\):.*ELF.*\(executable\|relocatable\|shared object\).*,.*/\1:\2/p' | \
(
  IFS=":"
  while read F S; do
    echo "$F: $S"
	${OBJCOPY} --strip-debug --strip-unneeded $F $F
	if [ "${S}" = "relocatable" ]; then
		${STRIPTOOL} -x \
			--strip-debug \
			--strip-unneeded \
			-R .comment \
			-R .pdr \
			-R .mdebug.abi32 \
			-R .gnu.attributes \
			-R .reginfo \
			-R .MIPS.abiflags \
			-R .note.GNU-stack \
			-R .note.gnu.build-id \
			$F
	else
		${STRIPTOOL} $F
		[ -x "${SSTRIP_TOOL}" ] && ${SSTRIP_TOOL} $F
	fi
  done
  true
)
sync
echo "\033[32m======================>>> ROMFS STRIP OK <<<======================\033[0m"
