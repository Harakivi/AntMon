cmake_minimum_required(VERSION 3.20)

file(GLOB STM_HAL_SRC
    Src/*.c
)

add_library(STM_HAL_LIBRARY STATIC ${STM_HAL_SRC})

target_link_options(STM_HAL_LIBRARY PRIVATE -nostdlib)

target_include_directories(STM_HAL_LIBRARY PUBLIC ./ Inc/ )

target_compile_definitions(STM_HAL_LIBRARY PUBLIC -D${ST_DEVICE})