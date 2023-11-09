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
set(VERSION_INFO "#define VERSION \"${PROJECT_VERSION}\"" )
set(DATE_INFO "#define DATE \"${DATE}\"" )
message("Build version = ${PROJECT_VERSION}\r\nBuild date = ${DATE}")

file(WRITE ${SOURCE_DIR}Version.h ${VERSION_INFO} "\r\n" ${DATE_INFO})

#############################################################
#                          CMSIS                            #
#############################################################

set(ST_DEVICE STM32F103x6)

add_subdirectory(CMSIS)

target_link_options(CMSIS_LIBRARY PUBLIC 
                    -Wl,--defsym,_Stack_Size=0x400
                    -Wl,--defsym,_Heap_Size=0x200)


#############################################################
#                        AntMonProj                         #
#############################################################

set(TARGET ${PROJECT_NAME}.elf)

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET} CMSIS_LIBRARY)

message("${DEVICE_LINKER_SCRIPT}")

target_link_options(${TARGET} PUBLIC -T ${DEVICE_LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map)

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${TARGET} ${PROJECT_NAME}.bin)

install(FILES build/${PROJECT_NAME}.bin DESTINATION ./)