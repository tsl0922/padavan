if command -v mipsel-linux-musl-gcc >/dev/null 2>&1; then
    export ARCH=mips
    export CROSS_COMPILE=mipsel-linux-musl-
    export CONFIG_CROSS_COMPILE=mipsel-linux-musl-
elif command -v mipsel-linux-uclibc-gcc >/dev/null 2>&1; then
    export ARCH=mips
    export CROSS_COMPILE=mipsel-linux-uclibc-
    export CONFIG_CROSS_COMPILE=mipsel-linux-uclibc-
else
    echo "Error: Neither mipsel-linux-musl nor mipsel-linux-uclibc toolchain found."
    exit 1
fi
