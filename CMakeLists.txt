cmake_minimum_required(VERSION 3.20)
project(AntMon LANGUAGES C CXX ASM)

#======================================================#
#                   Version.h                          #
#======================================================#
if(NOT DEFINED GITHUB_RUN_NUMBER)
    set(GITHUB_RUN_NUMBER 0)
endif()

set(PROJECT_VERSION 0.0.1.${GITHUB_RUN_NUMBER})

string(TIMESTAMP DATE "%d.%m.%y")
message("Build version = ${PROJECT_VERSION}\r\nBuild date = ${DATE}")

set(ST_DEVICE STM32F100x8)

set(HSE_VALUE 8000000)
set(LSE_VALUE 32768)

#############################################################
#                          CMSIS                            #
#############################################################

if(ST_DEVICE STREQUAL STM32F100x4 OR ST_DEVICE STREQUAL STM32F100x8)
set(ST_DEVICE_CMSIS STM32F100xB)
else()
set(ST_DEVICE_CMSIS ST_DEVICE)
endif()

string(TOLOWER ${ST_DEVICE} ST_DEVICE_LOWERCASE)
string(TOUPPER ${ST_DEVICE} ST_DEVICE_UPPERCASE)

set(DEVICE_LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/linker/${ST_DEVICE_UPPERCASE}_FLASH.ld CACHE STRING "")

file(GLOB CMSIS_SRC
    CMSIS/Device/ST/STM32F1xx/Source/Templates/*.c
    CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_${ST_DEVICE_LOWERCASE}.s
)

add_library(CMSIS_LIBRARY STATIC ${CMSIS_SRC})

target_include_directories(CMSIS_LIBRARY PUBLIC CMSIS/Include/ CMSIS/Device/ST/STM32F1xx/Include/)

target_compile_definitions(CMSIS_LIBRARY PUBLIC 
                            -D${ST_DEVICE_CMSIS}
                            -DHSE_VALUE=${HSE_VALUE}
                            -DLSE_VALUE=${LSE_VALUE})

target_link_options(CMSIS_LIBRARY PRIVATE -nostdlib)

get_target_property(CMSIS_LIBRARY_INCLUDE_DIR CMSIS_LIBRARY INCLUDE_DIRECTORIES)


#############################################################
#                          STM_HAL                          #
#############################################################

if(ST_DEVICE STREQUAL STM32F100x4 OR ST_DEVICE STREQUAL STM32F100x8)
set(ST_DEVICE_HAL STM32F100xB)
else()
set(ST_DEVICE_HAL ST_DEVICE)
endif()

file(GLOB STM_HAL_SRC
    STM_HAL/Src/*.c
)

add_library(STM_HAL_LIBRARY STATIC ${STM_HAL_SRC})

target_include_directories(STM_HAL_LIBRARY PUBLIC 
                            STM_HAL/Inc/ 
                            include/ 
                            ${CMSIS_LIBRARY_INCLUDE_DIR})

target_compile_definitions(STM_HAL_LIBRARY PUBLIC 
                            -D${ST_DEVICE_HAL}
                            -DHSE_VALUE=${HSE_VALUE}
                            -DLSE_VALUE=${LSE_VALUE})

target_link_libraries(STM_HAL_LIBRARY CMSIS_LIBRARY)

target_link_options(STM_HAL_LIBRARY PRIVATE -nostdlib)

get_target_property(STM_HAL_LIBRARY_INCLUDE_DIR STM_HAL_LIBRARY INCLUDE_DIRECTORIES)

#############################################################
#                        AntMonProj                         #
#############################################################

file(GLOB SOURCES
    source/*.c
    source/*.cpp
    small-printf/*.c
)

set(TARGET ${PROJECT_NAME}.elf)

add_executable(${TARGET} main.cpp ${SOURCES})

target_compile_definitions(${TARGET} PUBLIC
                            -DVERSION="${PROJECT_VERSION}"
                            -DDATE="${DATE}")

target_include_directories(${TARGET} PUBLIC CMSIS_LIBRARY_INCLUDE_DIR STM_HAL_LIBRARY_INCLUDE_DIR include/ small-printf/)

target_link_libraries(${TARGET} CMSIS_LIBRARY STM_HAL_LIBRARY)

target_link_options(${TARGET} PUBLIC 
                        -T ${DEVICE_LINKER_SCRIPT} 
                        -Wl,-Map=${PROJECT_NAME}.map
                        -Wl,--defsym,_Stack_Size=0x500
                        -Wl,--defsym,_Heap_Size=0x200)

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${TARGET} ${PROJECT_NAME}.bin)

install(FILES build/${PROJECT_NAME}.bin DESTINATION ./ RENAME ${PROJECT_NAME}_${PROJECT_VERSION}.bin)