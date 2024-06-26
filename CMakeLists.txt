# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

# set local pico-sdk path
# set(PICO_SDK_PATH ./pico-sdk/)

# Include build functions from Pico SDK
include(pico-sdk/pico_sdk_init.cmake)


# Set name of project (as PROJECT_NAME) and C/C++ standards
project(rp2cli C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# add libraries
# add_subdirectory(freertos)
add_subdirectory(lib)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
    )

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    lib pico_stdlib hardware_pio hardware_adc hardware_i2c hardware_flash hardware_sync
    
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})