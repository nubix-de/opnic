# Auto format code
# Author Vinícius Gabriel Linden
# Date 2023-10-19
# Copyright nubix Software-Design GmbH 2023

find_program(FORMATTER
             NAMES clang-format
             DOC "Code formatter"
             )

function(format_code TARGET FORMAT_STYLE)
    if(FORMATTER)
        get_target_property(TARGET_SOURCES ${TARGET} SOURCES)

        add_custom_target(${TARGET}_format
            COMMAND ${FORMATTER} -style="${FORMAT_STYLE}" -i ${TARGET_SOURCES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Formatting ${TARGET} files in the ${FORMAT_STYLE} style"
        )

        add_dependencies(${TARGET} ${TARGET}_format)
    else()
        message(WARNING "Could not find the code formatter")
    endif()
endfunction()
