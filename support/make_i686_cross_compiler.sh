#!/bin/sh
export PRJSRC="$HOME/prj-g0s"

export PREFIX="$PRJSRC/cross-i686/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

SRCPATH="$PRJSRC/src/gcc-8.2.0"
mkdir -p "$SRCPATH/build"
cd "$SRCPATH/build"

../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
make all-gcc -j8 
make all-target-libgcc -j8
make install-gcc
make install-target-libgcc
