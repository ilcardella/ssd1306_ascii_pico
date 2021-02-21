cmake_minimum_required(VERSION 3.12)

# PICO_SDK_PATH env var must point to the sdk installation path
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(ssd1306_ascii_pico_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# initialize the Pico SDK
pico_sdk_init()

# Import ssd1306_ascii_pico library
find_package(ssd1306_ascii_pico CONFIG REQUIRED)

# Create the binary from sources
set(SOURCES example_main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
                        pico_stdlib
                        ssd1306_ascii_pico::ssd1306_ascii_pico)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
