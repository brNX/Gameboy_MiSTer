
#include "SDL.h"
#include <stdint.h>

#include "gb-draw-utils.h"



SDL_Color getColor(int number,int mode, Vgb_video* video){
    SDL_Color white = {255,255,255,255};
    SDL_Color light_gray = {190,190,190,255};
    SDL_Color dark_gray = {90,90,90,255};
    SDL_Color black = {0,0,0,255};

    int colorindex;
    uint8_t palette;
    SDL_Color * color = &white;

    int hi,lo;

    switch(number)
    {
        case 0: hi = 1 ; lo = 0 ;break ;
        case 1: hi = 3 ; lo = 2 ;break ;
        case 2: hi = 5 ; lo = 4 ;break ;
        case 3: hi = 7 ; lo = 6 ;break ;
    }

    switch(mode)
    {
        case 0: palette=video->bgp ;  break;
        case 1: palette=video->obp0;  break;
        case 2: palette=video->obp1;  break;
    }

    colorindex = (palette & (1<<hi))?0x2:0;
    colorindex |= (palette & (1<<lo))?0x1:0;

    switch(colorindex){
        //white
        case 0: break ;

        //light gray
        case 1: color=&light_gray;break;

        //dark gray
        case 2: color=&dark_gray;break;

        //black
        case 3: color=&black;break;
    }

    return *color;

}


void loadvram(char * fileName, CData * array) {
    FILE *fileptr;
    fileptr = fopen(fileName, "rb");  // Open the file in binary mode
    fread(array, 8192, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file
}