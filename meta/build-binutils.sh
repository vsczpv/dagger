#!/bin/sh

export PREFIX="$(pwd)/cross/dest"
export TARGET="x86_64-elf"
export PATH="$PREFIX/dest/bin:$PATH"

cd cross
tar -xf binutils-2.40.tar.xz

cd binutils-2.40
mkdir build
cd build

../configure --target="$TARGET" --prefix="$PREFIX" --with-sysroot --disable-nls

make -j$(nproc)
make install
