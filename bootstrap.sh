#!/bin/sh

# This script does all the necessary work to get from a clean checkout
# to being able to run configure as usual.

if [ ! -f include/Ample.h ]; then
  echo "This doesn't look right... I don't feel good about this"
  exit 1
fi

autoreconf -i && \
echo "Bootstrap successful, now run ./configure"

