cd gfx || exit
clear
grit titlescreen.png -gb -gB8
#grit cars.png -gt -gB4 -Mw 2 -Mh 2
#grit bullets.png -gt -gB4
#grit icons.png -gt -gB4
#grit winscreen.png -gb -gB8
cd ..
make
#sleep 1
open lau_gurpil.gba -a mgba