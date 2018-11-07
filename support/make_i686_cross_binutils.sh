#!/bin/sh
export PRJSRC="$HOME/prj-g0s"

export PREFIX="$PRJSRC/cross-i686/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

SRCPATH="$PRJSRC/src/binutils-2.31"
mkdir -p "$SRCPATH/build"
cd "$SRCPATH/build"

../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j8 && make install
