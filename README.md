# About:
This project is a [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C++ with [Allegro 5 library](http://liballeg.org/).

##### Command to compile the project
g++ -Wall main.cpp graficos_Allegro.cpp chip8.cpp -o chip8 `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`

##### Command to run a game 
./chip8 "/games/game"

##### Keys schematic

1 - 2 - 3 - 4  
Q - W - E - R  
A - S - D - F  
Y - X - C - V  


