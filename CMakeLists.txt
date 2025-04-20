# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSB_VID=0x2E8A")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSB_PID=0x0001")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(model3_isospi C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(model3_isospi)

pico_enable_stdio_usb(model3_isospi 1)
pico_enable_stdio_uart(model3_isospi 0)

pico_generate_pio_header(model3_isospi ${CMAKE_CURRENT_LIST_DIR}/isospi.pio)

target_sources(model3_isospi PRIVATE
        model3_isospi.c
        pio_isospi.c
        pio_isospi.h
        )

target_link_libraries(model3_isospi PRIVATE pico_stdlib hardware_pio)
pico_add_extra_outputs(model3_isospi)

