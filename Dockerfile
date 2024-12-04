# docker build -t rp2040_env .
# docker run -it --rm -v ./src:/home/ubuntu/rp2shell/src --name rp2040_env rp2040_env

# 使用最新ubuntu
FROM ubuntu:latest
WORKDIR /home/ubuntu/rp2shell/src

# 更新 apt 並安裝編譯依賴套件
RUN apt update && apt install -y python3 git tar cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential g++ libstdc++-arm-none-eabi-newlib

RUN cd .. \
    && git clone https://github.com/raspberrypi/pico-sdk.git \
    && cd pico-sdk \
    && git submodule update --init \
    && cd .. \
    && git clone https://github.com/raspberrypi/picotool.git \
    && cd picotool \
    && mkdir build && cd build \
    && export PICO_SDK_PATH=../../pico-sdk/ \
    && cmake .. \
    && make && make install \
    && cd .. \
    && git config --global safe.directory '*' \