cd gfx || exit
clear
#grit lap_numbers.png -gt -gB4 -Mw 2 -Mh 2
#grit titlescreen.png -gb -gB8
#grit cars.png -gt -gB4 -Mw 2 -Mh 2
#grit ground_tiles.png -gt -gB4 -Mw 2 -Mh 2
#grit track_tiles.png -gt -gB4 -Mw 2 -Mh 2
#grit icons.png -gt -gB4
cd ..

# Extract checkpoints from tmx files
tools/checkpoints/checkpoint_extractor gfx/track1.tmx track_data
tools/checkpoints/checkpoint_extractor gfx/track2.tmx track_data

make

#sleep 1
open lau_gurpil.gba -a mgba