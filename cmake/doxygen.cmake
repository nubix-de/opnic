# Include doxygen target
# Author Vinícius Gabriel Linden
# Date 2023-10-16
# Copyright nubix Software-Design GmbH 2022

find_package(Doxygen OPTIONAL_COMPONENTS dot)
function(doxygen_add TARGET)
    if(DOXYGEN_FOUND)
        get_target_property(TARGET_INCLUDES ${TARGET} INCLUDE_DIRECTORIES)
        set(DOXYGEN_FILE_PATTERNS
            "*.c"
            "*.h"
            "*.cpp"
            "*.hpp"
            "*.py"
            )
        set(DOXYGEN_PROJECT_NAME "${TARGET}")
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_LIST_DIR}/README.md")
        set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/doc/logo.svg")
        set(DOXYGEN_RECURSIVE YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_SOURCE_BROWSER YES)
        set(DOXYGEN_INLINE_SOURCES YES)
        set(DOXYGEN_ENABLE_PREPROCESSING YES)
        set(DOXYGEN_MACRO_EXPANSION YES)
        set(DOXYGEN_PREDEFINED "__attribute__((x))=/**/")
        set(DOXYGEN_HTML_OUTPUT "doxygen")
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_DISABLE_INDEX NO)
        set(DOXYGEN_FULL_SIDEBAR NO)
        doxygen_add_docs("${TARGET}_doc"
            "${DOXYGEN_USE_MDFILE_AS_MAINPAGE}"
            "${TARGET_INCLUDES}"
            COMMENT "Generating doxygen documentation"
            )
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_HTML_OUTPUT}
                DESTINATION deploy/${TARGET}/
                OPTIONAL)
    endif()
endfunction()
