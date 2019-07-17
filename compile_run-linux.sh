#!/bin/bash

# cleanup
rm -rf obj_dir
rm -f  gb.vcd


# run Verilator to translate Verilog into C++, include C++ testbench
verilator -y tv80 -O3 --x-assign fast --x-initial fast --noassert -LDFLAGS "-L/mnt/C/verilator/sameboy -lsameboy -lm -lGL -lpthread -lSDL2_gpu -lOpenGL -lGLX -lGLU -Wl,-Bstatic -lSDL2main -Wl,-Bdynamic -lSDL2 -lpthread -ldl \`sdl2-config --libs\` -lSDL2_gpu -I/mnt/C/verilator/imgui/libs/gl3w -I/mnt/C/verilator/imgui -I/mnt/C/verilator/sameboy -I/mnt/C/verilator/imgui/examples -g" -CFLAGS "-lpthread \`sdl2-config --cflags\` -I/mnt/C/verilator/imgui/libs/gl3w -I/mnt/C/verilator/sameboy -I/mnt/C/verilator/imgui -I/mnt/C/verilator/imgui/examples -fpermissive " --cc Gameboy.sv --trace --exe gb_tb.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_sdl.cpp imgui/imgui_demo.cpp gb-draw-utils.cpp 
# build C++ project
make OPT_FAST="-O3 -fno-stack-protector" -j -C obj_dir/ -f VGameboy.mk VGameboy
# run executable simulation
obj_dir/VGameboy

# gtkwave gb.vcd gb.sav &
