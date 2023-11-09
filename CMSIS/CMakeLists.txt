cmake_minimum_required(VERSION 3.20)

string(TOLOWER ${ST_DEVICE} ST_DEVICE_LOWERCASE)
string(TOUPPER ${ST_DEVICE} ST_DEVICE_UPPERCASE)

set(DEVICE_LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/linker/${ST_DEVICE_UPPERCASE}_FLASH.ld CACHE STRING "")

file(GLOB CMSIS_SRC
    Device/ST/STM32F1xx/Source/Templates/*.c
    Device/ST/STM32F1xx/Source/Templates/gcc/startup_${ST_DEVICE_LOWERCASE}.s
)

add_library(CMSIS_LIBRARY STATIC ${CMSIS_SRC})

target_link_options(CMSIS_LIBRARY PRIVATE -nostdlib)

target_include_directories(CMSIS_LIBRARY PUBLIC Include/ Device/ST/STM32F1xx/Include/)

target_compile_definitions(CMSIS_LIBRARY PUBLIC -D${ST_DEVICE})