cmake_minimum_required(VERSION 3.19)
project(test C)

set(CMAKE_C_STANDARD 99)

# Specify external includes here
include_directories($ENV{DEVKITARM}/include/)
include_directories($ENV{DEVKITPRO}/libgba/include/)
include_directories($ENV{DEVKITPRO}/libtonc/include/)
include_directories($ENV{DEVKITARM}/arm-none-eabi/include)
include_directories(include gfx)

add_executable(test source/main.c source/racecar.c include/racecar.h include/race.h source/race.c)