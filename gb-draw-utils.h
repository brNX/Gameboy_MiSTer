#ifndef GBDRAWUTILS_H
#define GBDRAWUTILS_H

#include "SDL.h"
#include "VGameboy.h"
#include "VGameboy_gb.h"
#include "VGameboy_video.h"
#include "VGameboy_lcd.h"
#include "VGameboy_sprite.h"
#include "VGameboy_Gameboy.h"

#include "VGameboy_generic_spram__Ad.h"
#include "VGameboy_generic_spram__A7.h"
#include "VGameboy_generic_spram__Af.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

SDL_Color getColor(int number,int mode, VGameboy_video* video);
void loadvram(char * fileName, CData * array) ;
void drawBackground(SDL_Texture* background, SDL_Renderer* renderer, VGameboy* top);
void drawTileMap(SDL_Texture* tilemap, SDL_Renderer* renderer, VGameboy* top, int number);
void drawSprite(SDL_Texture* sprite_texture, SDL_Renderer* renderer, VGameboy_sprite* sprite,VGameboy* top, bool isGBC);
void drawLCD(SDL_Texture* tilemap, SDL_Renderer* renderer, VGameboy* top, bool isGBC);


#endif // GBDRAWUTILS_H