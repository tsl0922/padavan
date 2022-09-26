#!/bin/sh

DIR="toolchain-4.4.x"
DL_URL="https://github.com/tsl0922/padavan/releases/download/toolchain"

dl() {
	[ -z "$1" ] && return

	echo "Download toolchain: $1"
	mkdir -p $DIR
	(curl -fSsLo- "${DL_URL}/$1" | tar Jx -C $DIR) || rm -rf $DIR
}

if [ -d $DIR ]; then
	echo "$DIR already exists!"
	exit
fi

ARCH="$(uname -m)"

case $ARCH in
	aarch64)
		dl "aarch64_mipsel-linux-uclibc.tar.xz"
		;;
	x86_64)
		dl "mipsel-linux-uclibc.tar.xz"
		;;
	*)
		echo "Unknown ARCH: $ARCH"
		exit 1
esac
