#!/bin/bash

# cleanup
#rm -rf obj_dir
#rm -f  gb.vcd


# run Verilator to translate Verilog into C++, include C++ testbench
# verilator -LDFLAGS  "-lgdi32 -lopengl32 -limm32 \`pkg-config --static --libs sdl2\` -IC:/verilator/imgui -IC:/verilator/imgui/examples" -CFLAGS "-O2 \`pkg-config --cflags sdl2\` -IC:/verilator/imgui -IC:/verilator/imgui/examples -fpermissive" -Wall --cc --trace gb.v --exe gb_tb.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_sdl.cpp imgui/imgui_demo.cpp gb-draw-utils.cpp
# build C++ project
make -j -C obj_dir/ -f Vgb.mk Vgb
# run executable simulation
obj_dir/Vgb

# gtkwave gb.vcd gb.sav &
