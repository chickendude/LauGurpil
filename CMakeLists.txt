cmake_minimum_required(VERSION 3.19)
project(LauGurpil C)

set(CMAKE_C_STANDARD 99)

# Specify external includes here
include_directories($ENV{DEVKITARM}/include/)
include_directories($ENV{DEVKITPRO}/libgba/include/)
include_directories($ENV{DEVKITPRO}/libtonc/include/)
include_directories($ENV{DEVKITARM}/arm-none-eabi/include)
include_directories(include gfx track_data build)

add_executable(test source/main.c source/racecar.c include/racecar.h include/race.h source/race.c include/types.h include/title.h include/state.h source/title.c source/state.c include/track.h source/track.c include/constants.h include/camera.h source/camera.c include/race_stats.h source/race_stats.c include/text.h source/text.c include/racecar_select.h include/race_single.h include/race_tournament.h source/race_single.c source/racecar_select.c include/track_select.h source/track_select.c source/race_tournament.c source/tournament.c include/tournament.h source/racecar_ai.c include/racecar_ai.h source/checkpoint.c include/checkpoint.h track_data/track1_progress.h track_data/track1_progress.c track_data/track_markers.h track_data/track_ai_checkpoints.h)