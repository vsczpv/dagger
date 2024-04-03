#!/bin/sh

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
tar -xf binutils-2.40.tar.xz

cd binutils-2.40
mkdir build
cd build

../configure --target="$TARGET" --prefix="$PREFIX" --with-sysroot --disable-nls

make -j$(nproc)
make install
