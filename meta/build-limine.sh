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


git submodule init
git submodule update --depth 1

cd boot
cd limine
git fetch origin tag v7.1.0-binary --no-tags
git checkout v7.1.0-binary
cd ..

make -C limine
cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/

# This header is 0BSD Licensed, which is GPL compatible.
cd ..
echo '/* Taken from Limine */' >  include/arch/amd64/limine.h
cat boot/limine/limine.h       >> include/arch/amd64/limine.h
