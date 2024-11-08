#!/bin/bash

case "$1" in
    build) mkdir -p build && cd build && cmake .. && make -j ;;
    clr) rm -rf build;;
esac

echo Script Done!!
