#!/bin/bash

case "$1" in
    build) mkdir -p build && cd build && cmake .. && make -j ;;
    clr) cd ..;;
esac

echo Script Done!!