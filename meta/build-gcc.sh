#!/bin/sh

export PREFIX="$(pwd)/cross/dest"
export TARGET="x86_64-elf"
export PATH="$PREFIX/dest/bin:$PATH"

cd cross
tar -xf gcc-12.2.0.tar.xz

cd gcc-12.2.0

echo -e 'MULTILIB_OPTIONS += mno-red-zone\nMULTILIB_DIRNAMES += no-red-zone' > gcc/config/i386/t-x86_64-elf
sed '1902i tmake_file="${tmake_file} i386/t-x86_64-elf"' -i gcc/config.gcc

mkdir build
cd build

../configure --target="$TARGET" --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers

make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
