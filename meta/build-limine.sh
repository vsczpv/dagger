#!/bin/sh

git submodule init
git submodule update

cd boot
make -C limine
