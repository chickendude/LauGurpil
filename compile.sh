cd gfx || exit
clear
#grit titlescreen.png -gb -gB8
#grit cars.png -gt -gB4 -Mw 2 -Mh 2
#grit ground_tiles.png -gt -gB4 -Mw 2 -Mh 2
# grit track_tiles.png -gt -gB4 -Mw 2 -Mh 2
#grit icons.png -gt -gB4
cd ..
make
#sleep 1
open lau_gurpil.gba -a mgba