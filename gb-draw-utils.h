#ifndef GBDRAWUTILS_H
#define GBDRAWUTILS_H

#include "SDL.h"
#include "Vgb.h"
#include "Vgb_gb.h"
#include "Vgb_video.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

SDL_Color getColor(int number,int mode, Vgb_video* video);
void loadvram(char * fileName, CData * array) ;

#endif // GBDRAWUTILS_H