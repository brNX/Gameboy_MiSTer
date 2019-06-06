#ifndef GBDRAWUTILS_H
#define GBDRAWUTILS_H

#include "SDL.h"
#include "Vgb.h"
#include "Vgb_gb.h"
#include "Vgb_video.h"
#include "Vgb_lcd.h"
#include "Vgb_sprite.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

SDL_Color getColor(int number,int mode, Vgb_video* video);
void loadvram(char * fileName, CData * array) ;
void drawBackground(SDL_Texture* background, SDL_Renderer* renderer, Vgb* top);
void drawTileMap(SDL_Texture* tilemap, SDL_Renderer* renderer, Vgb* top);
void drawSprite(SDL_Texture* sprite_texture, SDL_Renderer* renderer, Vgb_sprite* sprite,Vgb* top, bool isGBC);
void drawLCD(SDL_Texture* tilemap, SDL_Renderer* renderer, Vgb* top, bool isGBC);


#endif // GBDRAWUTILS_H