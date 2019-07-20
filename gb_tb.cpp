#include "VGameboy.h"
#include "VGameboy_gb.h"
#include "VGameboy_video.h"
#include "VGameboy_lcd.h"
#include "VGameboy_sprites.h"
#include "VGameboy_sprite.h"
#include "VGameboy_Gameboy.h"
#include "VGameboy_cart.h"
#include "VGameboy_tv80_core.h"
#include "VGameboy_tv80s.h"

#include "VGameboy_generic_spram__Ad.h"
#include "VGameboy_generic_spram__A7.h"
#include "VGameboy_generic_spram__Af.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#include <stdint.h>   
#include "SDL.h"

#include "imgui.h"
#include "imgui_sdl.h"

#include "gb-draw-utils.h"

#define GB_INTERNAL 1

#include <Core/gb.h>
#include <Core/gb_wrapper.h>

struct gbwrapper gb_sameboy;

//#define DISABLE_TRACE

const uint32_t verilator_cycles= 6*(2048*3);

SDL_Texture* obpd[32];
SDL_Texture* bgpd[32];


uint32_t bitmap[160*144];


Uint32 timerTick(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}


void setSprite(VGameboy_sprite* sprite, int8_t y_pos, int8_t x_pos, int8_t tile, int8_t flags ) {
    sprite->y_pos = y_pos;
    sprite->x_pos = x_pos;
    sprite->tile = tile;
    sprite->flags = flags;
}


void loadRom(char * fileName, VGameboy* top) {
    FILE *fileptr;
    long filelen;
    fileptr = fopen(fileName, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file 
    fread(top->Gameboy->game_cart->rom_data, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file



    top->Gameboy->cart_mbc_type =  top->Gameboy->game_cart->rom_data[0x147];
    top->Gameboy->cart_rom_size =  top->Gameboy->game_cart->rom_data[0x148];
    top->Gameboy->cart_ram_size =  top->Gameboy->game_cart->rom_data[0x149];
    top->Gameboy->cart_cgb_flag =  top->Gameboy->game_cart->rom_data[0x143];
    top->Gameboy->mbc1 = 1;

    top->eval();

    printf("mbc1 %d mbc2  %d mbc3 %d mbc5 %d\n",top->Gameboy->mbc1,top->Gameboy->mbc2,top->Gameboy->mbc3,top->Gameboy->mbc5);


}

#define COMPARE(x,y)   if ((x)!=(y)) { printf(#x"!="#y"\n"); return false; }
bool compareRegisters ( VGameboy* top) {
    printf("PC: %X\n",top->Gameboy->gb->cpu->i_tv80_core->PC);
    printf("sameboy pc: %X\n",gb_sameboy.pc);
    printf("sameboy sp: %X\n",gb_sameboy.sp);
    COMPARE(top->Gameboy->gb->cpu->i_tv80_core->PC,gb_sameboy.pc);
    COMPARE(top->Gameboy->gb->cpu->i_tv80_core->ACC,gb_sameboy.a);
    COMPARE(top->Gameboy->gb->cpu->i_tv80_core->F,gb_sameboy.f);
    COMPARE(top->Gameboy->gb->cpu->i_tv80_core->SP,gb_sameboy.sp);
    COMPARE(!top->Gameboy->gb->cpu->i_tv80_core->halt_n,gb_sameboy.halted);
    return true;
}

/*void setGBCPalettes(VGameboy* top){
    //bgpd
    top->Gameboy->gb->video->bgpd[0] = 0x6f;
    top->Gameboy->gb->video->bgpd[1] = 0xdd;
    top->Gameboy->gb->video->bgpd[2] = 0xf5;
    top->Gameboy->gb->video->bgpd[3] = 0x1a;
    top->Gameboy->gb->video->bgpd[4] = 0x2f;
    top->Gameboy->gb->video->bgpd[5] = 0x16;
    top->Gameboy->gb->video->bgpd[6] = 0x00;
    top->Gameboy->gb->video->bgpd[7] = 0x00;

    top->Gameboy->gb->video->bgpd[8] = 0x9b;
    top->Gameboy->gb->video->bgpd[9] = 0x2e;
    top->Gameboy->gb->video->bgpd[10] = 0xf5;
    top->Gameboy->gb->video->bgpd[11] = 0x1a;
    top->Gameboy->gb->video->bgpd[12] = 0xb4;
    top->Gameboy->gb->video->bgpd[13] = 0x11;
    top->Gameboy->gb->video->bgpd[14] = 0x00;
    top->Gameboy->gb->video->bgpd[15] = 0x00;

    top->Gameboy->gb->video->bgpd[16] = 0xff;
    top->Gameboy->gb->video->bgpd[17] = 0x7f;
    top->Gameboy->gb->video->bgpd[18] = 0xf5;
    top->Gameboy->gb->video->bgpd[19] = 0x1a;
    top->Gameboy->gb->video->bgpd[20] = 0x1f;
    top->Gameboy->gb->video->bgpd[21] = 0x21;
    top->Gameboy->gb->video->bgpd[22] = 0x00;
    top->Gameboy->gb->video->bgpd[23] = 0x00;

    top->Gameboy->gb->video->bgpd[24] = 0xbe;
    top->Gameboy->gb->video->bgpd[25] = 0x6f;
    top->Gameboy->gb->video->bgpd[26] = 0x9b;
    top->Gameboy->gb->video->bgpd[27] = 0x2e;
    top->Gameboy->gb->video->bgpd[28] = 0xb4;
    top->Gameboy->gb->video->bgpd[29] = 0x11;
    top->Gameboy->gb->video->bgpd[30] = 0x00;
    top->Gameboy->gb->video->bgpd[31] = 0x00;

    top->Gameboy->gb->video->bgpd[32] = 0xff;
    top->Gameboy->gb->video->bgpd[33] = 0x7f;
    top->Gameboy->gb->video->bgpd[34] = 0x08;
    top->Gameboy->gb->video->bgpd[35] = 0x7f;
    top->Gameboy->gb->video->bgpd[36] = 0xb4;
    top->Gameboy->gb->video->bgpd[37] = 0x11;
    top->Gameboy->gb->video->bgpd[38] = 0x00;
    top->Gameboy->gb->video->bgpd[39] = 0x00;

    top->Gameboy->gb->video->bgpd[40] = 0x08;
    top->Gameboy->gb->video->bgpd[41] = 0x7f;
    top->Gameboy->gb->video->bgpd[42] = 0xf5;
    top->Gameboy->gb->video->bgpd[43] = 0x1a;
    top->Gameboy->gb->video->bgpd[44] = 0x2f;
    top->Gameboy->gb->video->bgpd[45] = 0x16;
    top->Gameboy->gb->video->bgpd[46] = 0x00;
    top->Gameboy->gb->video->bgpd[47] = 0x00;

    top->Gameboy->gb->video->bgpd[48] = 0xff;
    top->Gameboy->gb->video->bgpd[49] = 0x7f;
    top->Gameboy->gb->video->bgpd[50] = 0x08;
    top->Gameboy->gb->video->bgpd[51] = 0x7f;
    top->Gameboy->gb->video->bgpd[52] = 0xfb;
    top->Gameboy->gb->video->bgpd[53] = 0x26;
    top->Gameboy->gb->video->bgpd[54] = 0x00;
    top->Gameboy->gb->video->bgpd[55] = 0x00;

    top->Gameboy->gb->video->bgpd[56] = 0x9b;
    top->Gameboy->gb->video->bgpd[57] = 0x2e;
    top->Gameboy->gb->video->bgpd[58] = 0x08;
    top->Gameboy->gb->video->bgpd[59] = 0x7f;
    top->Gameboy->gb->video->bgpd[60] = 0xb4;
    top->Gameboy->gb->video->bgpd[61] = 0x11;
    top->Gameboy->gb->video->bgpd[62] = 0x00;
    top->Gameboy->gb->video->bgpd[63] = 0x00;

    //obpd

    top->Gameboy->gb->video->obpd[0] = 0x77;
    top->Gameboy->gb->video->obpd[1] = 0x77;
    top->Gameboy->gb->video->obpd[2] = 0x7d;
    top->Gameboy->gb->video->obpd[3] = 0x3a;
    top->Gameboy->gb->video->obpd[4] = 0x7c;
    top->Gameboy->gb->video->obpd[5] = 0x10;
    top->Gameboy->gb->video->obpd[6] = 0x00;
    top->Gameboy->gb->video->obpd[7] = 0x00;

    top->Gameboy->gb->video->obpd[8] = 0x77;
    top->Gameboy->gb->video->obpd[9] = 0x77;
    top->Gameboy->gb->video->obpd[10] = 0xff;
    top->Gameboy->gb->video->obpd[11] = 0x7f;
    top->Gameboy->gb->video->obpd[12] = 0xef;
    top->Gameboy->gb->video->obpd[13] = 0x3d;
    top->Gameboy->gb->video->obpd[14] = 0x00;
    top->Gameboy->gb->video->obpd[15] = 0x00;

    top->Gameboy->gb->video->obpd[16] = 0x77;
    top->Gameboy->gb->video->obpd[17] = 0x77;
    top->Gameboy->gb->video->obpd[18] = 0x7d;
    top->Gameboy->gb->video->obpd[19] = 0x67;
    top->Gameboy->gb->video->obpd[20] = 0xb9;
    top->Gameboy->gb->video->obpd[21] = 0x32;
    top->Gameboy->gb->video->obpd[22] = 0x00;
    top->Gameboy->gb->video->obpd[23] = 0x00;

    top->Gameboy->gb->video->obpd[24] = 0x77;
    top->Gameboy->gb->video->obpd[25] = 0x77;
    top->Gameboy->gb->video->obpd[26] = 0x3a;
    top->Gameboy->gb->video->obpd[27] = 0x6f;
    top->Gameboy->gb->video->obpd[28] = 0xf2;
    top->Gameboy->gb->video->obpd[29] = 0x5d;
    top->Gameboy->gb->video->obpd[30] = 0x00;
    top->Gameboy->gb->video->obpd[31] = 0x00;

    top->Gameboy->gb->video->obpd[32] = 0x77;
    top->Gameboy->gb->video->obpd[33] = 0x77;
    top->Gameboy->gb->video->obpd[34] = 0xff;
    top->Gameboy->gb->video->obpd[35] = 0x7f;
    top->Gameboy->gb->video->obpd[36] = 0x5e;
    top->Gameboy->gb->video->obpd[37] = 0x0b;
    top->Gameboy->gb->video->obpd[38] = 0x00;
    top->Gameboy->gb->video->obpd[39] = 0x00;

    top->Gameboy->gb->video->obpd[40] = 0x77;
    top->Gameboy->gb->video->obpd[41] = 0x77;
    top->Gameboy->gb->video->obpd[42] = 0xff;
    top->Gameboy->gb->video->obpd[43] = 0x7f;
    top->Gameboy->gb->video->obpd[44] = 0x3f;
    top->Gameboy->gb->video->obpd[45] = 0x02;
    top->Gameboy->gb->video->obpd[46] = 0x00;
    top->Gameboy->gb->video->obpd[47] = 0x00;

    top->Gameboy->gb->video->obpd[48] = 0x77;
    top->Gameboy->gb->video->obpd[49] = 0x77;
    top->Gameboy->gb->video->obpd[50] = 0x5e;
    top->Gameboy->gb->video->obpd[51] = 0x0b;
    top->Gameboy->gb->video->obpd[52] = 0xaf;
    top->Gameboy->gb->video->obpd[53] = 0x01;
    top->Gameboy->gb->video->obpd[54] = 0x00;
    top->Gameboy->gb->video->obpd[55] = 0x00;

    top->Gameboy->gb->video->obpd[56] = 0x77;
    top->Gameboy->gb->video->obpd[57] = 0x77;
    top->Gameboy->gb->video->obpd[58] = 0xda;
    top->Gameboy->gb->video->obpd[59] = 0x46;
    top->Gameboy->gb->video->obpd[60] = 0x37;
    top->Gameboy->gb->video->obpd[61] = 0x01;
    top->Gameboy->gb->video->obpd[62] = 0x00;
    top->Gameboy->gb->video->obpd[63] = 0x00;
}*/

void getSpriteInfo(VGameboy_sprite* sprite,char * spriteinfo){
    sprintf(spriteinfo,"X:%02X\nY:%02X\nT:%02X\nF:%02X",sprite->x_pos,sprite->y_pos,sprite->tile,sprite->flags);
}

void drawpalettes(SDL_Renderer* renderer, VGameboy* top) {
    int currentindex=0;

    for (int palette_index = 0; palette_index<8;palette_index++) {
        
        //bgpd
        for (int i= 0; i<4;i++) {
            SDL_SetRenderTarget(renderer, bgpd[palette_index*4+i]);
            {

                SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                int index = (palette_index<<3)+i*2;
                
                int r5 = top->Gameboy->gb->video->bgpd[index]&0x1F;
                int g5 = ((top->Gameboy->gb->video->bgpd[index+1]&0x3)<<3) | ((top->Gameboy->gb->video->bgpd[index]&0xE0)>>5);
                int b5 = (top->Gameboy->gb->video->bgpd[index+1]&0x7C)>>2;

                int r10 = (r5 * 13) + (g5 * 2) +b5;
                int g10 = (g5 * 3) + b5;
                int b10 = (r5 * 3) + (g5 * 2) + (b5 * 11);
                
                SDL_Color palette;
                
                palette.r = (r10&0x1FE)>>1;
                palette.g = (g10&0x7F)<<1;
                palette.b = (b10&0x1FE)>>1;

                palette.a = SDL_ALPHA_OPAQUE;
                
                //8 2bytes pairs
                for (int y=0;y<8;y++){
                    //8 pixels per line
                    for (int x=0;x<8;x++){
                        SDL_SetRenderDrawColor(renderer, palette.r, palette.g, palette.b,SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer,x,y);
                    }
                }

                SDL_SetRenderTarget(renderer, nullptr);
            }
            currentindex++;
        }
    }

    for (int palette_index = 0; palette_index<8;palette_index++) {
        //obpd
        for (int i= 0; i<4;i++) {
            SDL_SetRenderTarget(renderer, obpd[palette_index*4+i]);
            {

                SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                int index = (palette_index<<3)+i*2;
                
                int r5 = top->Gameboy->gb->video->obpd[index]&0x1F;
                int g5 = ((top->Gameboy->gb->video->obpd[index+1]&0x3)<<3) | ((top->Gameboy->gb->video->obpd[index]&0xE0)>>5);
                int b5 = (top->Gameboy->gb->video->obpd[index+1]&0x7C)>>2;

                int r10 = (r5 * 13) + (g5 * 2) +b5;
                int g10 = (g5 * 3) + b5;
                int b10 = (r5 * 3) + (g5 * 2) + (b5 * 11);
                
                SDL_Color palette;
                
                palette.r = (r10&0x1FE)>>1;
                palette.g = (g10&0x7F)<<1;
                palette.b = (b10&0x1FE)>>1;

                palette.a = SDL_ALPHA_OPAQUE;
                
                //8 2bytes pairs
                for (int y=0;y<8;y++){
                    //8 pixels per line
                    for (int x=0;x<8;x++){
                        SDL_SetRenderDrawColor(renderer, palette.r, palette.g, palette.b,SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer,x,y);
                    }
                }


                SDL_SetRenderTarget(renderer, nullptr);
            }
            currentindex++;
        }
    }
}

bool drawSameboy = false;

static uint32_t rgb_encode(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8);
}

static void vblank(GB_gameboy_t *gb)
{
    printf("vblank\n");
    drawSameboy = true;
}

int main(int argc, char **argv) {

    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    int i=0;
    int clk;
    Verilated::commandArgs(argc, argv);
    // init top verilog instance
    VGameboy* top = new VGameboy;
    
    // init trace dump
    #ifndef DISABLE_TRACE
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("gb.vcd");
    #endif

    VGameboy_sprite * sprites_array[40];
    sprites_array[0]=top->Gameboy->gb->video->sprites->spr__BRA__0__KET____DOT__sprite;
    sprites_array[1]=top->Gameboy->gb->video->sprites->spr__BRA__1__KET____DOT__sprite;
    sprites_array[2]=top->Gameboy->gb->video->sprites->spr__BRA__2__KET____DOT__sprite;
    sprites_array[3]=top->Gameboy->gb->video->sprites->spr__BRA__3__KET____DOT__sprite;
    sprites_array[4]=top->Gameboy->gb->video->sprites->spr__BRA__4__KET____DOT__sprite;
    sprites_array[5]=top->Gameboy->gb->video->sprites->spr__BRA__5__KET____DOT__sprite;
    sprites_array[6]=top->Gameboy->gb->video->sprites->spr__BRA__6__KET____DOT__sprite;
    sprites_array[7]=top->Gameboy->gb->video->sprites->spr__BRA__7__KET____DOT__sprite;
    sprites_array[8]=top->Gameboy->gb->video->sprites->spr__BRA__8__KET____DOT__sprite;
    sprites_array[9]=top->Gameboy->gb->video->sprites->spr__BRA__9__KET____DOT__sprite;
    sprites_array[10]=top->Gameboy->gb->video->sprites->spr__BRA__10__KET____DOT__sprite;
    sprites_array[11]=top->Gameboy->gb->video->sprites->spr__BRA__11__KET____DOT__sprite;
    sprites_array[12]=top->Gameboy->gb->video->sprites->spr__BRA__12__KET____DOT__sprite;
    sprites_array[13]=top->Gameboy->gb->video->sprites->spr__BRA__13__KET____DOT__sprite;
    sprites_array[14]=top->Gameboy->gb->video->sprites->spr__BRA__14__KET____DOT__sprite;
    sprites_array[15]=top->Gameboy->gb->video->sprites->spr__BRA__15__KET____DOT__sprite;
    sprites_array[16]=top->Gameboy->gb->video->sprites->spr__BRA__16__KET____DOT__sprite;
    sprites_array[17]=top->Gameboy->gb->video->sprites->spr__BRA__17__KET____DOT__sprite;
    sprites_array[18]=top->Gameboy->gb->video->sprites->spr__BRA__18__KET____DOT__sprite;
    sprites_array[19]=top->Gameboy->gb->video->sprites->spr__BRA__19__KET____DOT__sprite;
    sprites_array[20]=top->Gameboy->gb->video->sprites->spr__BRA__20__KET____DOT__sprite;
    sprites_array[21]=top->Gameboy->gb->video->sprites->spr__BRA__21__KET____DOT__sprite;
    sprites_array[22]=top->Gameboy->gb->video->sprites->spr__BRA__22__KET____DOT__sprite;
    sprites_array[23]=top->Gameboy->gb->video->sprites->spr__BRA__23__KET____DOT__sprite;
    sprites_array[24]=top->Gameboy->gb->video->sprites->spr__BRA__24__KET____DOT__sprite;
    sprites_array[25]=top->Gameboy->gb->video->sprites->spr__BRA__25__KET____DOT__sprite;
    sprites_array[26]=top->Gameboy->gb->video->sprites->spr__BRA__26__KET____DOT__sprite;
    sprites_array[27]=top->Gameboy->gb->video->sprites->spr__BRA__27__KET____DOT__sprite;
    sprites_array[28]=top->Gameboy->gb->video->sprites->spr__BRA__28__KET____DOT__sprite;
    sprites_array[29]=top->Gameboy->gb->video->sprites->spr__BRA__29__KET____DOT__sprite;
    sprites_array[30]=top->Gameboy->gb->video->sprites->spr__BRA__30__KET____DOT__sprite;
    sprites_array[31]=top->Gameboy->gb->video->sprites->spr__BRA__31__KET____DOT__sprite;
    sprites_array[32]=top->Gameboy->gb->video->sprites->spr__BRA__32__KET____DOT__sprite;
    sprites_array[33]=top->Gameboy->gb->video->sprites->spr__BRA__33__KET____DOT__sprite;
    sprites_array[34]=top->Gameboy->gb->video->sprites->spr__BRA__34__KET____DOT__sprite;
    sprites_array[35]=top->Gameboy->gb->video->sprites->spr__BRA__35__KET____DOT__sprite;
    sprites_array[36]=top->Gameboy->gb->video->sprites->spr__BRA__36__KET____DOT__sprite;
    sprites_array[37]=top->Gameboy->gb->video->sprites->spr__BRA__37__KET____DOT__sprite;
    sprites_array[38]=top->Gameboy->gb->video->sprites->spr__BRA__38__KET____DOT__sprite;
    sprites_array[39]=top->Gameboy->gb->video->sprites->spr__BRA__39__KET____DOT__sprite;



    // initialize simulation inputs
    top->clk_sys = 1;
    top->reset = 1;

    char filename[] = "roms/sh.gb";
    

    loadRom(filename, top);
    // //loadRom("roms/Radar Mission (UE) [!].gb", top);
    // //loadRom("roms/gb240p.gbc", top);
    // //loadRom("roms/03-op sp,hl.gb", top);
    // //loadRom("roms/ldhlsp.gb", top);
    // //loadRom("roms/cpu_instrs.gb", top);
    // //loadRom("roms/Super_Mario_Land_2_DX_Hack_v1.12_toruzz.gbc", top);
    // //loadRom("roms/Legend_of_ZeldaDX.gbc", top);
    // //loadRom("roms/drmario.gb", top);
    // //loadRom("roms/Xenon 2 - Megablast (USA, Europe).gb", top);

    top->eval ();  

    const int window_x = 1980;
    const int window_y = 1080;

    SDL_Window* window = SDL_CreateWindow("SDL2 ImGui Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_x, window_y, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, window_x, window_y);
   
    SDL_Texture* background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 256, 256);
    SDL_Texture* tilemap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 271,407);
    SDL_Texture* tilemap2 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 271,407);
    SDL_Texture* lcd = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 160,144);
    SDL_Texture* lcd_sameboy = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 160,144);

    SDL_Texture* sprites[40];
    for (int i =0; i<40;i++){
        sprites[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 8,8);  
    }
    
    for (int i =0; i<32;i++){
        obpd[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 8,8); 
        bgpd[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 8,8); 
    }
    bool run = true;
    bool isGBC = false;
    bool runVerilator = false;
    int lcd_mode = -1;
    int lcd_mode_old = -2;
    int spriteinfo=0;
    char spriteinfo_buffer [100];

    uint32_t value = 0;

    GB_Init(filename,&bitmap[0]);

    SDL_TimerID my_timer_id = SDL_AddTimer(18, timerTick, NULL);
   
    
    while (run)
    {

        ImGuiIO& io = ImGui::GetIO();

        int wheel = 0;

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {

            switch(e.type)
            {            
                case SDL_QUIT:
                        run = false;
                        break;

                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        io.DisplaySize.x = static_cast<float>(e.window.data1);
                        io.DisplaySize.y = static_cast<float>(e.window.data2);
                    }
                    break;
                    
                case SDL_MOUSEWHEEL:
                    wheel = e.wheel.y;
                    break;
    
                case SDL_USEREVENT: {
                    bool render=false;
                    if (runVerilator){                   
                        unsigned long currentclock = top->Gameboy->gb->clockcounter;
                        unsigned long cycles_sameboy = runGB(&gb_sameboy);
                        printf("sameboy pc: %X\n",gb_sameboy.pc);

                        while (top->Gameboy->gb->clockcounter < ((cycles_sameboy-3)+currentclock-3)){
                            i++;
                            top->reset = (i < 2);
                            top->isGBC = isGBC;
                            for (clk=0; clk<2; clk++) {
                                
                                #ifndef DISABLE_TRACE
                                //if (top->Gameboy->gb->boot_rom_enabled == 0 && top->Gameboy->gb->video->ly<144 &&  (top->Gameboy->gb->video->h_cnt>78 && top->Gameboy->gb->video->h_cnt<270))
                                    tfp->dump (2*i+clk);
                                #endif
                                top->clk_sys = !top->clk_sys;
                                top->eval ();
                                lcd_mode_old = lcd_mode;
                                lcd_mode = top->mode;

                                if ((lcd_mode == 1) && (lcd_mode_old!=1)) {
                                    render=true;
                                }
                            }

                        }
                        
                        if (Verilated::gotFinish()) run = false;

                        if (!compareRegisters(top)) {
                            run = false;
                            printf("comparision failed at %d\n",top->Gameboy->gb->clockcounter);
                        }

                        if (render) { //draw things 1 time
                            drawLCD(lcd,renderer,top,isGBC);
                            drawTileMap(tilemap,renderer,top,0);
                            //drawTileMap(tilemap2,renderer,top,1);
                            drawBackground(background,renderer,top);
                            //drawpalettes(renderer,top);
                            //for (int i=0;i<40;i++){
                            //    drawSprite(sprites[i],renderer,sprites_array[i],top,isGBC);
                            //}
                        }

                        if (drawSameboy) { //draw things 1 time
                            drawSameboy=false;
                            //TODO: finish this
                        }
                    }

                    break;
                }
            }
        }

        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

        // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
        
        io.DeltaTime = 1.0f / 60.0f;
        io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        io.MouseWheel = static_cast<float>(wheel);

        bool True = true;
        bool False = false;

        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::Begin("Background");
        ImGui::Image(background, ImVec2(256*2, 256*2));
        ImGui::End();

        ImGui::Begin("TileMap");
        ImGui::Image(tilemap, ImVec2(271, 407));
        //ImGui::SameLine();
        //ImGui::Image(tilemap2, ImVec2(271, 407));
        ImGui::End();

        /*ImGui::Begin("Palettes");

        ImGui::BeginGroup();
        ImGui::Text("BGPD");
        int currentindex=0;
        for (int j=0;j<8;j++){
            ImGui::Text("%d:",j);
            ImGui::SameLine();
            for (int i=0;i<4;i++) {
                ImGui::Image(bgpd[currentindex], ImVec2(32, 32));
                if (i!=3)
                    ImGui::SameLine();
                currentindex++;
            }
        }
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(20.0f, 0.0f));
        ImGui::SameLine();
        
        ImGui::BeginGroup();
        ImGui::Text("OBPD");
        currentindex=0;
        for (int j=0;j<8;j++){
            ImGui::Text("%d:",j);
            ImGui::SameLine();
            for (int i=0;i<4;i++) {
                ImGui::Image(obpd[currentindex], ImVec2(32, 32));
                if (i!=3)
                    ImGui::SameLine();
                currentindex++;
            }
        }
        ImGui::EndGroup();
    
        ImGui::End();*/

        ImGui::Begin("LCD");
        ImGui::Image(lcd, ImVec2(160*3, 144*3));
        ImGui::End();

        ImGui::Begin("LCD Sameboy");
        ImGui::Image(lcd_sameboy, ImVec2(160*3, 144*3));
        ImGui::End();


        /* ImGui::Begin("Palettes Background Text");
        ImGui::Text("%04X",top->Gameboy->gb->video->bgpd[1]<<8|top->Gameboy->gb->video->bgpd[0]);
        ImGui::SameLine();
        ImGui::Text("%04X",top->Gameboy->gb->video->bgpd[3]<<8|top->Gameboy->gb->video->bgpd[2]);
        ImGui::SameLine();
        ImGui::Text("%04X",top->Gameboy->gb->video->bgpd[5]<<8|top->Gameboy->gb->video->bgpd[4]);
        ImGui::SameLine();
        ImGui::Text("%04X",top->Gameboy->gb->video->bgpd[7]<<8|top->Gameboy->gb->video->bgpd[6]);

        ImGui::End();*/

        /*ImGui::Begin("Sprites");
        ImGui::BeginGroup();
        int sprite=0;
        for (int row =0; row<5;row++){
            for (int i =0; i<7;i++){
 
                ImGui::BeginGroup();//sprite+text
                ImGui::Image(sprites[sprite], ImVec2(16*2, 16*2));
                if (ImGui::IsItemHovered()){
                    spriteinfo = sprite;
                }
                ImGui::SameLine();
                getSpriteInfo(sprites_array[sprite],spriteinfo_buffer);
                ImGui::Text(spriteinfo_buffer);
                ImGui::EndGroup();//sprite+text

                ImGui::SameLine();
                sprite++;
            }
            ImGui::BeginGroup();//sprite+text
            ImGui::Image(sprites[sprite], ImVec2(16*2, 16*2));
            if (ImGui::IsItemHovered()){
                    spriteinfo = sprite;
            }
            ImGui::SameLine();
            getSpriteInfo(sprites_array[sprite],spriteinfo_buffer);
            ImGui::Text(spriteinfo_buffer);
            ImGui::NewLine();
            ImGui::EndGroup();//sprite+text
            sprite++;
        }
        ImGui::EndGroup();
        
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(20.0f, 0.0f));
        
        ImGui::SameLine();
        //details 
        VGameboy_sprite * selected_sprite = sprites_array[spriteinfo];
        ImGui::BeginGroup();
        ImGui::SetNextItemWidth(20);
        ImGui::LabelText("X-loc","%02X",selected_sprite->x_pos);
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(10.0f, 0.0f));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(20);
        ImGui::LabelText("Tile No","%02X",selected_sprite->tile);
        ImGui::SetNextItemWidth(20);
        ImGui::LabelText("Y-loc","%02X",selected_sprite->y_pos);
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(10.0f, 0.0f));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(20);
        ImGui::LabelText("Flags","%02X",selected_sprite->flags);
        ImGui::NewLine();
        ImGui::SetNextItemWidth(70);
        ImGui::LabelText("OAM Addr","%04X",0xfe00+(4*spriteinfo));
        ImGui::SetNextItemWidth(70);
        ImGui::LabelText("Tile Addr","%d:%04X",isGBC?(selected_sprite->flags&0x8)>>3:0,0x8000+(selected_sprite->tile*16));
        ImGui::NewLine();
        ImGui::SetNextItemWidth(20);
        ImGui::Checkbox("X-Flip",(selected_sprite->flags&0x20)?&True:&False); //bit5
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        ImGui::LabelText("Palette","OBJ %d",isGBC?(selected_sprite->flags&0x7):(selected_sprite->flags&0x10>>3));  //bit 2-0 CGB mode , Bit4 in GB mode
        ImGui::SetNextItemWidth(20);
        ImGui::Checkbox("Y-Flip",(selected_sprite->flags&0x40)?&True:&False); //bit6
        ImGui::SameLine();
        ImGui::SetNextItemWidth(20);  
        ImGui::Checkbox("Priority",(selected_sprite->flags&0x80)?&True:&False); //bit7
        ImGui::NewLine();
        ImGui::LabelText("Sprite","%d",spriteinfo);
        ImGui::EndGroup();
        ImGui::End();*/

        ImGui::Begin("GB Config");
        ImGui::Checkbox("Enable Gameboy Color?",&isGBC);
        ImGui::Checkbox("Run?",&runVerilator);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("LCD Mode","%d",lcd_mode);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("Cycles Elapsed","%d",top->Gameboy->gb->clockcounter);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("LCDC","%02X",top->Gameboy->gb->video->lcdc);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("SCY","%02X",top->Gameboy->gb->video->scy );
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("SCX","%02X",top->Gameboy->gb->video->scx );
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("WY","%02X",top->Gameboy->gb->video->wy );
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("WX","%02X",top->Gameboy->gb->video->wx);
        ImGui::SetNextItemWidth(100);
        ImGui::Checkbox("Bootrom enabled?",top->Gameboy->gb->boot_rom_enabled?&True:&False);
        ImGui::SetNextItemWidth(100);
        ImGui::Checkbox("GBC game enabled?",top->Gameboy->isGBC_game?&True:&False);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("BGP","%02X",top->Gameboy->gb->video->bgp);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("OBP0","%02X",top->Gameboy->gb->video->obp0);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("OBP1","%02X",top->Gameboy->gb->video->obp1);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("Instructions per second","%d",value);
        ImGui::End();

        SDL_SetRenderDrawColor(renderer, 114, 144, 154, 255);
        SDL_RenderClear(renderer);

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());

        SDL_RenderPresent(renderer);
    }

    #ifndef DISABLE_TRACE
    tfp->close();
    #endif

    ImGuiSDL::Deinitialize();

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(tilemap);
    SDL_DestroyTexture(tilemap2);
    SDL_DestroyTexture(lcd);
    SDL_DestroyTexture(lcd_sameboy);
   
    for (int i =0; i<32;i++){
        SDL_DestroyTexture(obpd[i]); 
        SDL_DestroyTexture(bgpd[i]); 
    }

    for (int i =0; i<40;i++){
        SDL_DestroyTexture(sprites[i]) ;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui::DestroyContext();

    freeGB();

    return 0;  

}