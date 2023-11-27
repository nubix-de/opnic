#Snake Game

## Overview

The Snake game is a fun-to-play game based on the original Snake Game back in
90's.

Our approach here was to refer to the original game and give it a more fun
aspect running on a modern microcontroller, the RP2040 on a PCB developed by us,
at a very low cost of 6€.

Thus, the game presents in a simple but nice way our company, what we do, how we
work, and useful information for you to get to know us better.

## Snake Game Development

For this, the decision was to use the pico SDK and the C++ language to meet the
fact that C++ has better performance than Python, and we have a small flash
memory, so C++ has a smaller footprint than python.

The game itself is developed using both cores of the RP2040 where core-0 runs
the [physics calculation of the game](#game-physics) while core-1 is in
charge of [drawing the game images](#world-class).

### Snake

The base element of the snake is the `particle` class that represents the
physical properties of the snake, such as position and velocity. It is worth
noting that in this implementation the snake moves, the world stands still. This
is accomplished via the `Viewport` class.

### Loading Sprites

The game contains some PNG images that are embedded into the firmware. And the
images are decompressed at runtime using the PngImage library. Because of that,
we need to have some temporary buffers at runtime to render the images and be
able to draw them on the screen. However, because we have a limited SRAM
capacity, all the necessary images cannot be loaded at once, but instead the are
loaded and released on demand. Thus, the `DynamicImageLoader` class is
responsible for managing, loading and releasing the images (sprites) used by the
game.

For example, suppose we have an image that represents a Coin. And we have in
total 200 Coins spread over the game's world. Of course just a few of them will
be on the viewport's view at any time (then they should be displayed), and the
others will be off the viewport's view (thus, not displayed). Because of this,
it can be released if and only if we are sure that none of the Coins are in the
viewsport's view, thus we can release it from memory. If that is not true, then
we should keep the Coin's image for as long as it is visible in the viewport.
And, to make it clear, only a single Coin image is loaded for those 200 Coins.
The 200 Coins are just the reference where the Coin image should be drawn.

### Object Class

The game is composed for different so called `Object`, so, there is a base
class `Object` that provides the basic implementation of an object of the game.

The `Object` class is statically allocated, using a singleton instantiation. So,
only one `Object` class will exist in any time. This decision was made because
of the SRAM limitation that the board has, so we keep only a single object
instance. The mutation happens at runtime, that is, the `Object` base class
mutates according to the object it is going to represent at the iteration.

There are several objects on the game: `Coin`, `BlackHole`, for example, and
they all use the same base class `Object`. However, these objects also have a
specialized class that represents them, and they may overload some methods of
the base class `Object`.

That is the case of the `BlackHole->Draw()`, which overloads the base class for
drawing itself in a specialized way, this `Object` can be viewed as an interface
class. The cited specialized classes keep the singleton behavior of the base
class, as to avoid the unnecessary SRAM usage.

At the end, an `Object` is represented by its coordinate `x` and `y`, its
`width` and `height`, and its object `type`. So, the different objects are
generalized by these information. So, to mutate between objects, we simply
mutate the base `Object` by the specialized class at runtime. So, the objects
coordinates, widths and heights are kept as a simple array, and the game iterate
over these arrays, mutating the `Object` class based on the object `type`.

This manner keeps the SRAM usage low, that is, one does not have to instance per
object. If one would do so, there would have thousands of objects instantiated,
which is impossible for the amount of SRAM the RP2040 microcontroller has.

## Fading Texts

There are some fading texts throughout the game. To have this effect, the
implementation was done by creating a specialized class `Text` that should
represent text on the screen that has the fading effect.

This class is limited by its concepts on purpose, that is, the SRAM limitation
of RP2040 imposes some limits in how classes are instantiated. Due to this, the
`Text` class is also statically allocated and is a singleton class. Nonetheless,
the class can represent only three lines of text, at maximum text length of 20
bytes - including the `NULL` terminator.

The fading effect is not managed by the class itself, but by the `Game` class.
This decision was made to make the `Text` class simpler, and lighter. So, the
`Game` class when drawing the `Text` it should calculate the `decay` value (that
at the end is translated into the alpha channel) of the text to be drawn on the
screen.

## Game Physics

The game in the current implementation has some basic physics concepts for the
snake's movement, for example, or for the gravity force when the snakes passes
near by a `BlackHole`. So, the `Physics` file implements the `Particle` class,
as well as the `Vector` class.

The `Vector` class represents a 2-axis unit vector, where it has the
x-coordinate, y-axis coordinate, and the angle formed by the vector x and y.
This vector is the base for calculating the physics behind the objects that
require it.

Then, there is the `Particle` object, that represents a object that can move (as
the snake does), which it has the `Vector` position and the `Vector` velocity.
This `Particle` then drives the logic of particle motion, such as the position
updated based on the `Particle`s velocity `Vector`, also the gravity effect that
affects the velocity `Vector` of the particle, etc.

## World class

The `World` class is responsible for creating, drawing and updating the world of
the game. It is dependent on the `Coordinates.hpp` file ([explained
below](#coordinates-of-the-objects)) that are the arrays representing the
objects, their coordinates and etc.

This class has the two main methods: `WorldUpdate()` and `WordDraw()`. At the
current implementation, each method runs in different RP2040 cores, that is,
Core-0 runs `WorldUpdate()` while Core-1 runs the `WorldDraw()`.

### Coordinates of the objects

The objects have their properties defined at `Coordinates.hpp` file, which is
auto-generated via the `from_unity.py` file. The coordinates is a structure
`Coordinate_t` with the below attributes:

```cpp
typedef struct Coordinate_t {
  int x;             ///< x-coordinate
  int y;             ///< y-coordinate
  bool visible;      ///< is the object visible in viewport
  bool picked;       ///< was the object picked up (if is pickable)
  uint8_t width;     ///< width of the object
  uint8_t height;    ///< height of the object
  int32_t decay;     ///< alpha decay (if have)
  const char *text;  ///< Text of the object (if have)
} Coordinate_t;
```

So, for each type of object, there will be an array with the coordinates. The
script `from_unity.py` is the one responsible for auto-generating this file. But, to
make it work, you need to have the `abc.csv` file generated from the Unity game
version of the snake.

# Style

The snake code loosely uses the [Google C++ Style
Guide](https://google.github.io/styleguide/cppguide.html). To enforce
formatting, run:

```sh
find . -type f -iregex ".*\.\(c\|cpp\|h\|hpp\)$" \
    -exec clang-format -style=Google -i {} \;
```
