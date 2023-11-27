
# How to add more projects

A binary project consists of a folder with a `CMakeLists.txt` inside. You don't
have to change anything about `./build.sh` or `./CMakeLists.txt`.

See `libtest` for a minimal example.

# Including pictures

See `libtest/src/main.cpp`. Path is relative to the build folder (`deploy/`) or
to the included folders (see `libtest/CMakeLists.txt`).
