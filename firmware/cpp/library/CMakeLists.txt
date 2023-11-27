
add_library(grapix STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/graphic/Effect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/graphic/Font.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/graphic/FrameBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/graphic/LT177ML35.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/graphic/PngImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pngle/src/miniz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pngle/src/pngle.c

    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/common/debug.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/i2cdev/I2Cdev/I2Cdev.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/i2cdev/MPU6050/MPU6050.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/sensor/Buttons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/src/sensor/GyroAccel.cpp
)
target_include_directories(grapix SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/i2cdev/I2Cdev/
    ${CMAKE_CURRENT_SOURCE_DIR}/i2cdev/MPU6050/
    ${CMAKE_CURRENT_SOURCE_DIR}/incbin/
    ${CMAKE_CURRENT_SOURCE_DIR}/nubix/inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/pngle/src/
)

target_compile_definitions(grapix PRIVATE
    PNGLE_NO_GAMMA_CORRECTION
)

target_link_libraries(grapix PUBLIC
    pico_multicore
    pico_stdlib
    hardware_dma
    hardware_pio
    hardware_i2c
    hardware_pwm
)
