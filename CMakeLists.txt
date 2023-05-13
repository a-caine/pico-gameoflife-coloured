cmake_minimum_required(VERSION 3.12)

set(NAME game-of-life-colour)

include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(${NAME}
    golcol.cpp
)

# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project - check pimoroni_pico_import.cmake for details
include(common/pimoroni_bus)
include(libraries/pico_display/pico_display)
include(libraries/pico_graphics/pico_graphics)
include(drivers/st7789/st7789)
include(drivers/button/button)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
    pico_stdlib
    hardware_spi
    hardware_pwm
    hardware_dma

    pico_display
    pico_graphics
    button
    st7789
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
