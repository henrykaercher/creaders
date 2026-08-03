#!/bin/sh

PREFIX="${PREFIX:-/usr/local}"

mkdir -p "$PREFIX/include/creaders"

cp src/*.h "$PREFIX/include/creaders/"
