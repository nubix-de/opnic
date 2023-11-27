
add_executable(snake
    src/main.cpp
    src/DynamicImageLoader.cpp
    src/Booster.cpp
    src/Bumper.cpp
    src/BlackHole.cpp
    src/Coin.cpp
    src/Text.cpp
    src/Headline.cpp
    src/Physics.cpp
    src/Snake.cpp
    src/Viewport.cpp
    src/World.cpp
    src/Game.cpp
    src/Object.cpp
    src/FrameView.cpp
)

target_include_directories(snake PRIVATE
    inc/
    sprites/
)

target_link_libraries(snake PRIVATE
    grapix
)

opnic_setup(snake "1" "nubix snake game" OFF)

format_code(snake Google)
