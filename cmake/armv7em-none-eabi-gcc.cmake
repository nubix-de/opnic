
# We use "Generic" so cmake can expect to build for `none` platform.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_EFFECTIVE_SYSTEM_NAME Generic)
# Our buildroot toolchain is for `arm`
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSS_COMPILING 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX OR APPLE)
    find_path(TOOLCHAIN_ROOT
        NAMES
            arm-none-eabi-gcc
        NO_CACHE
        PATHS
            /opt/arm-none-eabi
            /opt/gcc-arm-none-eabi
            /usr
            /usr/local
        PATH_SUFFIXES
            /bin
        NO_DEFAULT_PATH
        )
      message(STATUS "TOOLCHAIN_ROOT: ${TOOLCHAIN_ROOT}")
else()
    message(FATAL_ERROR "toolchain not supported on this OS")
endif(UNIX OR APPLE)

# Toolchain Prefix
set(TRIPLE arm-none-eabi)

set(CMAKE_C_COMPILER_TARGET ${TRIPLE})
set(CMAKE_CXX_COMPILER_TARGET ${TRIPLE})

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CROSS_COMPILING 1)

set(CMAKE_C_COMPILER ${TOOLCHAIN_ROOT}/${TRIPLE}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/${TRIPLE}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_ROOT}/${TRIPLE}-gcc)
set(CMAKE_AR ${TOOLCHAIN_ROOT}/${TRIPLE}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_ROOT}/${TRIPLE}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_ROOT}/${TRIPLE}-objdump)
set(CMAKE_STRIP ${TOOLCHAIN_ROOT}/${TRIPLE}-strip)
set(CMAKE_SIZE ${TOOLCHAIN_ROOT}/${TRIPLE}-size)
set(CMAKE_LINKER ${TOOLCHAIN_ROOT}/${TRIPLE}-ld)

if(APPLE)
    #needed to allow link on MacOS
    set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
endif(APPLE)

# Don't run the linker on compiler check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

add_compile_definitions(USE_URIPARSE)

if(CMAKE_SIZEOF_VOID_P GREATER 0)
  add_definitions(-DSIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P})
endif()
