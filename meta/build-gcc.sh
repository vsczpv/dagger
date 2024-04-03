#!/bin/bash

#
# Copyright © 2024 Vinícius Schütz Piva
#
# This file is part of dagger
#
# dagger is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>
#

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

../configure --target="$TARGET" --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers && \

make -j$(nproc) all-gcc           && \
make -j$(nproc) all-target-libgcc && \
make install-gcc                  && \
make install-target-libgcc
