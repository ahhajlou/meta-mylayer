cmake_minimum_required(VERSION 3.16)
project(simple-sdl3-app)

# Simple approach - rely on Yocto's environment
add_executable(simple-sdl3-app main.c)

# Link SDL3 directly
target_link_libraries(simple-sdl3-app SDL3 SDL3_ttf)

# Install
install(TARGETS simple-sdl3-app DESTINATION bin)