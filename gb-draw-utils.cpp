
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

void drawBackground(SDL_Texture* background, SDL_Renderer* renderer, Vgb* top) {

    SDL_SetRenderTarget(renderer, background);
    {

        uint16_t backgroundAddress;
        uint16_t tileAddress;

        //Window Tile Map Display Select
        if(top->gb->video->lcdc & 0x8)
            backgroundAddress=0x1C00;
        else
            backgroundAddress=0x1800;

        for (int line=0,y=0;line<32;line++,y+=8){
            for (int row=0,x=0;row<32;row++,x+=8){

                    if (top->gb->video->lcdc & 0x10){
                        uint8_t tilenumber;
                        tileAddress = 0x0;
                        tilenumber = top->gb->vram0_array[backgroundAddress];
                        tileAddress+=(tilenumber*16);
                    }
                    else{
                        int8_t tilenumber;
                        tileAddress = 0x800;
                        tilenumber = top->gb->vram0_array[backgroundAddress];
                        tileAddress+=((tilenumber+128)*16);
                    }

                    //8 2bytes pairs
                    for (int i=0,pixely=0;i<8;i++,pixely++){
                        uint8_t data1=top->gb->vram0_array[tileAddress+i*2];
                        uint8_t data2=top->gb->vram0_array[tileAddress+i*2+1];

                        //8 pixels per line
                        for (int j=7,pixelx=0;j>-1;j--,pixelx++){

                            int colorNumber = (data2 & (1<<j))?0x2:0;
                            colorNumber |= (data1 & (1<<j))?1:0;
                            SDL_Color value = getColor(colorNumber,0,top->gb->video);
                            SDL_SetRenderDrawColor(renderer, value.r, value.g, value.b,SDL_ALPHA_OPAQUE);
                            SDL_RenderDrawPoint(renderer,x+pixelx,y+pixely);
                        }

                    }
                    backgroundAddress++;
            }
        }
        SDL_SetRenderTarget(renderer, nullptr);
    }


}

void drawTileMap(SDL_Texture* tilemap, SDL_Renderer* renderer, Vgb* top) {

    SDL_SetRenderTarget(renderer, tilemap);
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 50, 50, 50,SDL_ALPHA_OPAQUE);

        int x=16,y=16;
        for (int i=0 ; i<15;i++ ){
            for (int j=0;j<407;j++){
                SDL_RenderDrawPoint(renderer,x,j);
            }
            x+=17;
        }

        for (int i=0 ; i<23;i++ ){
            for (int j=0;j<271;j++){
                SDL_RenderDrawPoint(renderer,j,y);
            }
            y+=17;
        } 

        x=0;
        y=0;
        int address = 0;
        //16 bytes(0x10) per tile
        for(int line=0;line <24;line++){
            y=line*16+line;
            for (int row = 0 ; row <16;row++){
                x=row*16+row;


                //8 2bytes pairs
                for (int i=0,pixely=0;i<8;i++,pixely+=2){
                    uint8_t data1=top->gb->vram0_array[address+i*2];
                    uint8_t data2=top->gb->vram0_array[address+i*2+1];

                    //8 pixels per line
                    for (int j=7,pixelx=0;j>-1;j--,pixelx+=2){

                        int colorNumber = (data2 & (1<<j))?0x2:0;
                        colorNumber |= (data1 & (1<<j))?1:0;

                        SDL_Color value = getColor(colorNumber,0,top->gb->video);
                        SDL_SetRenderDrawColor(renderer, value.r, value.g, value.b,SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer,x+pixelx,y+pixely);
                        SDL_RenderDrawPoint(renderer,x+pixelx+1,y+pixely);
                        SDL_RenderDrawPoint(renderer,x+pixelx,y+pixely+1);
                        SDL_RenderDrawPoint(renderer,x+pixelx+1,y+pixely+1);
                    }
                }
                address+=16;
            }
        }

        SDL_SetRenderTarget(renderer, nullptr);
    }

}

void drawSprite(SDL_Texture* sprite_texture, SDL_Renderer* renderer, Vgb_sprite* sprite,Vgb* top, bool isGBC) {
    
    SDL_SetRenderTarget(renderer, sprite_texture);
    {

        SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        //vram (0x8000 +(tileLocation*16))+line
        int tilelocation = sprite->tile*16;
        int vrambank=0;
        SDL_Color palette[4];

        if (isGBC)
            vrambank = (sprite->flags & 0x8)>>3;

        if (isGBC) {
            int palette_index = (sprite->flags & 0x7)<<3;
            for (int i=0;i<4;i++){
                int index = palette_index+i*2;

                
                int r5 = top->gb->video->obpd[index]&0x1F;
                int g5 = ((top->gb->video->obpd[index+1]&0x3)<<3) | ((top->gb->video->obpd[index]&0xE0)>>5);
                int b5 = (top->gb->video->obpd[index+1]&0x7C)>>2;

                int r10 = (r5 * 13) + (g5 * 2) +b5;
                int g10 = (g5 * 3) + b5;
                int b10 = (r5 * 3) + (g5 * 2) + (b5 * 11);
                
                palette[i].r = (r10&0x1FE)>>1;
                palette[i].g = (g10&0x7F)<<1;
                palette[i].b = (b10&0x1FE)>>1;

                palette[i].a = SDL_ALPHA_OPAQUE;
            }
        }else {
            int palettenumber = (sprite->flags & 0x10)>>4;
            for (int i=0;i<4;i++){
                palette[i]= getColor(i,palettenumber?2:1,top->gb->video);
            } 
        }
        
        //8 2bytes pairs
        for (int i=0,y=0;i<8;i++,y++){
            uint8_t data1,data2;
            if (vrambank) {
                data1=top->gb->vram1_array[tilelocation+i*2];
                data2=top->gb->vram1_array[tilelocation+i*2+1];
            }else{
                data1=top->gb->vram0_array[tilelocation+i*2];
                data2=top->gb->vram0_array[tilelocation+i*2+1];
            }

            //8 pixels per line
            for (int j=7,x=0;j>-1;j--,x++){

                int colorNumber = (data2 & (1<<j))?0x2:0;
                colorNumber |= (data1 & (1<<j))?1:0;
                SDL_SetRenderDrawColor(renderer, palette[colorNumber].r, palette[colorNumber].g, palette[colorNumber].b,SDL_ALPHA_OPAQUE);
                
                int pixelx= sprite->flags&0x20?7-x:x;
                int pixely= sprite->flags&0x40?7-y:y;

               
                SDL_RenderDrawPoint(renderer,pixelx,pixely);
            }
        }



        SDL_SetRenderTarget(renderer, nullptr);
    }
   

}

void drawLCD(SDL_Texture* tilemap, SDL_Renderer* renderer, Vgb* top, bool isGBC) {
    SDL_SetRenderTarget(renderer, tilemap);
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        
        int i=0;
        for (int y=0; y<144;y++){
            for (int x=0;x<160;x++) {

                if (isGBC) {
                    int r5 = top->gb->lcd->lcd_buffer[i]&0x1F;
                    int g5 = (top->gb->lcd->lcd_buffer[i]>>5)&0x1F;
                    int b5 = (top->gb->lcd->lcd_buffer[i]>>10)&0x1F;

                    int r10 = (r5 * 13) + (g5 * 2) +b5;
                    int g10 = (g5 * 3) + b5;
                    int b10 = (r5 * 3) + (g5 * 2) + (b5 * 11);

                    /*Bit 0-4   Red Intensity   (00-1F)
                    Bit 5-9   Green Intensity (00-1F)
                    Bit 10-14 Blue Intensity  (00-1F) */

                    SDL_SetRenderDrawColor(renderer, (r10&0x1FE)>>1, (g10&0x7F)<<1,(b10&0x1FE)>>1,SDL_ALPHA_OPAQUE);
                }else
                {
                    int pixel = top->gb->lcd->lcd_buffer[i];
                    int  grey = (pixel==0)?252:(pixel==1)?168:(pixel==2)?96:0;
                    SDL_SetRenderDrawColor(renderer, grey, grey, grey,SDL_ALPHA_OPAQUE);
                }
                
                SDL_RenderDrawPoint(renderer,x,y);
                i++;
            }
        }
        SDL_SetRenderTarget(renderer, nullptr);
    }
}