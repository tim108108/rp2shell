#!/bin/bash

case "$1" in
    build) docker build -t rp2040_env . ;;
    run) docker run -it --rm -v ./src:/home/ubuntu/rp2shell/src --name rp2040_env rp2040_env;;
esac

echo Script Done!!