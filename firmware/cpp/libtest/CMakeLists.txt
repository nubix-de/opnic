
add_executable(libtest
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(libtest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/
)

target_link_libraries(libtest PRIVATE
    grapix
)

opnic_setup(libtest "1" "Library Test" ON)
