#!/bin/sh

git submodule init
git submodule update --depth 1

cd boot
cd limine
git fetch origin tag v7.1.0-binary --no-tags
git checkout v7.1.0-binary
cd ..

make -C limine
cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/

cd ..
echo '/* Taken from Limine */' >  include/arch/amd64/limine.h
cat boot/limine/limine.h       >> include/arch/amd64/limine.h
