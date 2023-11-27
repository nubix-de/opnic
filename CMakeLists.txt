# Main CMakeList for all projects
# Author Andreas Petter
#        Vinícius Gabriel Linden
# Date 2022-12-17
# Copyright nubix Software-Design GmbH 2022
cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE Debug) # TODO remove
set(PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/firmware/board/)
set(PICO_PLATFORM "rp2040")
set(PICO_BOARD "nubix_opnic")
set(PICO_CXX_ENABLE_CXA_ATEXIT 0)
set(PICO_CXX_ENABLE_EXCEPTIONS 0)
set(PICO_CXX_ENABLE_RTTI 0)
# initialize pico-sdk from GIT
set(PICO_SDK_FETCH_FROM_GIT ON)
# put the sdk where C project may also access
set(PICO_SDK_FETCH_FROM_GIT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/firmware/pico_sdk/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
# pico_sdk_import.cmake is a single file copied from this SDK
include(pico_sdk_import)
include(armv7em-none-eabi-gcc)
include(doxygen)
include(formatter)

# build.sh takes care of this
project(OPNIC LANGUAGES ASM C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_compile_options(-Os) # flash space if very limited
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-D DEBUG=ON
                    -D PICO_DEOPTIMIZED_DEBUG=ON)
    set(PICO_DEOPTIMIZED_DEBUG ON)
    add_compile_options(-g3)
endif()
add_compile_definitions(
    PICO_HEAP_SIZE=0x10000 # 64kiB
    PICO_MALLOC_PANIC=1
    PICO_STACK_SIZE=0x800
)
add_compile_options(-Wall -Wextra -Wshadow -Wdouble-promotion -Wno-format
    -Wundef -fno-common -fstack-usage -fdata-sections -ffunction-sections
    -Wno-undef -Wno-scalar-storage-order -Wno-unused-function
    -Wno-implicit-fallthrough
)
add_link_options(-Wl,--start-group -Wl,--gc-sections -Wl,--print-memory-usage
    -Wl,--end-group
)

function(opnic_setup TARGET VERSION DESCRIPTION UART)
    if(UART)
        message(NOTICE "${TARGET} stdio: UART enabled, USB CDC disabled")
        pico_enable_stdio_usb(${TARGET} OFF)
        pico_enable_stdio_uart(${TARGET} ON)
    else()
        message(NOTICE "${TARGET} stdio: USB CDC enabled, UART disabled")
        pico_enable_stdio_usb(${TARGET} ON)
        pico_enable_stdio_uart(${TARGET} OFF)
    endif()

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_SIZE} -G -d ${TARGET}.elf
        COMMENT "Displaying size"
    )

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -h -S "$<TARGET_FILE:${TARGET}>" >
                ${TARGET}.list
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Dumping compiled symbols to list"
    )

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_CURRENT_LIST_DIR}/
        COMMENT "Copying compile_commands.json to root"
    )

    # picotool stuff
    pico_set_program_name(${TARGET} ${TARGET})
    pico_set_program_version(${TARGET} ${VERSION})
    pico_set_program_description(${TARGET} ${DESCRIPTION})
    pico_set_program_url(${TARGET} "https://nubix.de")

    # generate map/bin/hex/uf2 files in addition to elf
    pico_add_extra_outputs(${TARGET})

    install(TARGETS ${TARGET}
            DESTINATION deploy/${TARGET}/
            OPTIONAL
    )
    # TODO search for a better way
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.uf2
                  ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin
            DESTINATION deploy/${TARGET}/
            OPTIONAL
    )

    doxygen_add(${TARGET})
endfunction()

add_subdirectory(firmware/cpp/)

