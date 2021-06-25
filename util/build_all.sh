#! /bin/sh

find ../keyboards -maxdepth 1 -mindepth 1 -type d -printf '%f\n' | xargs -L 1 make -C ../