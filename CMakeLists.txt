cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Da_Vinci_controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include_directories(include)

add_executable(Da_Vinci_controller
    src/main.cpp
    src/servo.cpp
    src/pot.cpp
)

target_link_libraries(Da_Vinci_controller
    pico_stdlib
    hardware_pwm
    hardware_adc
)

pico_enable_stdio_usb(Da_Vinci_controller 1)
pico_enable_stdio_uart(Da_Vinci_controller 0)

pico_add_extra_outputs(Da_Vinci_controller)

