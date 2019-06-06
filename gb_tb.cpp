#include "Vgb.h"
#include "Vgb_gb.h"
#include "Vgb_video.h"
#include "Vgb_lcd.h"
#include "Vgb_sprites.h"
#include "Vgb_sprite.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <stdint.h>   
#include "SDL.h"

#include "imgui.h"
#include "imgui_sdl.h"

#include "gb-draw-utils.h"


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




void setSprite(Vgb_sprite* sprite, int8_t y_pos, int8_t x_pos, int8_t tile, int8_t flags ) {
    sprite->y_pos = y_pos;
    sprite->x_pos = x_pos;
    sprite->tile = tile;
    sprite->flags = flags;
}

void setGBCPalettes(Vgb* top){
    //bgpd
    top->gb->video->bgpd[0] = 0x6f;
    top->gb->video->bgpd[1] = 0xdd;
    top->gb->video->bgpd[2] = 0xf5;
    top->gb->video->bgpd[3] = 0x1a;
    top->gb->video->bgpd[4] = 0x2f;
    top->gb->video->bgpd[5] = 0x16;
    top->gb->video->bgpd[6] = 0x00;
    top->gb->video->bgpd[7] = 0x00;

    top->gb->video->bgpd[8] = 0x9b;
    top->gb->video->bgpd[9] = 0x2e;
    top->gb->video->bgpd[10] = 0xf5;
    top->gb->video->bgpd[11] = 0x1a;
    top->gb->video->bgpd[12] = 0xb4;
    top->gb->video->bgpd[13] = 0x11;
    top->gb->video->bgpd[14] = 0x00;
    top->gb->video->bgpd[15] = 0x00;

    top->gb->video->bgpd[16] = 0xff;
    top->gb->video->bgpd[17] = 0x7f;
    top->gb->video->bgpd[18] = 0xf5;
    top->gb->video->bgpd[19] = 0x1a;
    top->gb->video->bgpd[20] = 0x1f;
    top->gb->video->bgpd[21] = 0x21;
    top->gb->video->bgpd[22] = 0x00;
    top->gb->video->bgpd[23] = 0x00;

    top->gb->video->bgpd[24] = 0xbe;
    top->gb->video->bgpd[25] = 0x6f;
    top->gb->video->bgpd[26] = 0x9b;
    top->gb->video->bgpd[27] = 0x2e;
    top->gb->video->bgpd[28] = 0xb4;
    top->gb->video->bgpd[29] = 0x11;
    top->gb->video->bgpd[30] = 0x00;
    top->gb->video->bgpd[31] = 0x00;

    top->gb->video->bgpd[32] = 0xff;
    top->gb->video->bgpd[33] = 0x7f;
    top->gb->video->bgpd[34] = 0x08;
    top->gb->video->bgpd[35] = 0x7f;
    top->gb->video->bgpd[36] = 0xb4;
    top->gb->video->bgpd[37] = 0x11;
    top->gb->video->bgpd[38] = 0x00;
    top->gb->video->bgpd[39] = 0x00;

    top->gb->video->bgpd[40] = 0x08;
    top->gb->video->bgpd[41] = 0x7f;
    top->gb->video->bgpd[42] = 0xf5;
    top->gb->video->bgpd[43] = 0x1a;
    top->gb->video->bgpd[44] = 0x2f;
    top->gb->video->bgpd[45] = 0x16;
    top->gb->video->bgpd[46] = 0x00;
    top->gb->video->bgpd[47] = 0x00;

    top->gb->video->bgpd[48] = 0xff;
    top->gb->video->bgpd[49] = 0x7f;
    top->gb->video->bgpd[50] = 0x08;
    top->gb->video->bgpd[51] = 0x7f;
    top->gb->video->bgpd[52] = 0xfb;
    top->gb->video->bgpd[53] = 0x26;
    top->gb->video->bgpd[54] = 0x00;
    top->gb->video->bgpd[55] = 0x00;

    top->gb->video->bgpd[56] = 0x9b;
    top->gb->video->bgpd[57] = 0x2e;
    top->gb->video->bgpd[58] = 0x08;
    top->gb->video->bgpd[59] = 0x7f;
    top->gb->video->bgpd[60] = 0xb4;
    top->gb->video->bgpd[61] = 0x11;
    top->gb->video->bgpd[62] = 0x00;
    top->gb->video->bgpd[63] = 0x00;

    //obpd

    top->gb->video->obpd[0] = 0x77;
    top->gb->video->obpd[1] = 0x77;
    top->gb->video->obpd[2] = 0x7d;
    top->gb->video->obpd[3] = 0x3a;
    top->gb->video->obpd[4] = 0x7c;
    top->gb->video->obpd[5] = 0x10;
    top->gb->video->obpd[6] = 0x00;
    top->gb->video->obpd[7] = 0x00;

    top->gb->video->obpd[8] = 0x77;
    top->gb->video->obpd[9] = 0x77;
    top->gb->video->obpd[10] = 0xff;
    top->gb->video->obpd[11] = 0x7f;
    top->gb->video->obpd[12] = 0xef;
    top->gb->video->obpd[13] = 0x3d;
    top->gb->video->obpd[14] = 0x00;
    top->gb->video->obpd[15] = 0x00;

    top->gb->video->obpd[16] = 0x77;
    top->gb->video->obpd[17] = 0x77;
    top->gb->video->obpd[18] = 0x7d;
    top->gb->video->obpd[19] = 0x67;
    top->gb->video->obpd[20] = 0xb9;
    top->gb->video->obpd[21] = 0x32;
    top->gb->video->obpd[22] = 0x00;
    top->gb->video->obpd[23] = 0x00;

    top->gb->video->obpd[24] = 0x77;
    top->gb->video->obpd[25] = 0x77;
    top->gb->video->obpd[26] = 0x3a;
    top->gb->video->obpd[27] = 0x6f;
    top->gb->video->obpd[28] = 0xf2;
    top->gb->video->obpd[29] = 0x5d;
    top->gb->video->obpd[30] = 0x00;
    top->gb->video->obpd[31] = 0x00;

    top->gb->video->obpd[32] = 0x77;
    top->gb->video->obpd[33] = 0x77;
    top->gb->video->obpd[34] = 0xff;
    top->gb->video->obpd[35] = 0x7f;
    top->gb->video->obpd[36] = 0x5e;
    top->gb->video->obpd[37] = 0x0b;
    top->gb->video->obpd[38] = 0x00;
    top->gb->video->obpd[39] = 0x00;

    top->gb->video->obpd[40] = 0x77;
    top->gb->video->obpd[41] = 0x77;
    top->gb->video->obpd[42] = 0xff;
    top->gb->video->obpd[43] = 0x7f;
    top->gb->video->obpd[44] = 0x3f;
    top->gb->video->obpd[45] = 0x02;
    top->gb->video->obpd[46] = 0x00;
    top->gb->video->obpd[47] = 0x00;

    top->gb->video->obpd[48] = 0x77;
    top->gb->video->obpd[49] = 0x77;
    top->gb->video->obpd[50] = 0x5e;
    top->gb->video->obpd[51] = 0x0b;
    top->gb->video->obpd[52] = 0xaf;
    top->gb->video->obpd[53] = 0x01;
    top->gb->video->obpd[54] = 0x00;
    top->gb->video->obpd[55] = 0x00;

    top->gb->video->obpd[56] = 0x77;
    top->gb->video->obpd[57] = 0x77;
    top->gb->video->obpd[58] = 0xda;
    top->gb->video->obpd[59] = 0x46;
    top->gb->video->obpd[60] = 0x37;
    top->gb->video->obpd[61] = 0x01;
    top->gb->video->obpd[62] = 0x00;
    top->gb->video->obpd[63] = 0x00;
}

void getSpriteInfo(Vgb_sprite* sprite,char * spriteinfo){
    sprintf(spriteinfo,"X:%02X\nY:%02X\nT:%02X\nF:%02X",sprite->x_pos,sprite->y_pos,sprite->tile,sprite->flags);
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
    Vgb* top = new Vgb;
    
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("gb.vcd");


    Vgb_sprite * sprites_array[40];
    sprites_array[0]=top->gb->video->sprites->spr__BRA__0__KET____DOT__sprite;
    sprites_array[1]=top->gb->video->sprites->spr__BRA__1__KET____DOT__sprite;
    sprites_array[2]=top->gb->video->sprites->spr__BRA__2__KET____DOT__sprite;
    sprites_array[3]=top->gb->video->sprites->spr__BRA__3__KET____DOT__sprite;
    sprites_array[4]=top->gb->video->sprites->spr__BRA__4__KET____DOT__sprite;
    sprites_array[5]=top->gb->video->sprites->spr__BRA__5__KET____DOT__sprite;
    sprites_array[6]=top->gb->video->sprites->spr__BRA__6__KET____DOT__sprite;
    sprites_array[7]=top->gb->video->sprites->spr__BRA__7__KET____DOT__sprite;
    sprites_array[8]=top->gb->video->sprites->spr__BRA__8__KET____DOT__sprite;
    sprites_array[9]=top->gb->video->sprites->spr__BRA__9__KET____DOT__sprite;
    sprites_array[10]=top->gb->video->sprites->spr__BRA__10__KET____DOT__sprite;
    sprites_array[11]=top->gb->video->sprites->spr__BRA__11__KET____DOT__sprite;
    sprites_array[12]=top->gb->video->sprites->spr__BRA__12__KET____DOT__sprite;
    sprites_array[13]=top->gb->video->sprites->spr__BRA__13__KET____DOT__sprite;
    sprites_array[14]=top->gb->video->sprites->spr__BRA__14__KET____DOT__sprite;
    sprites_array[15]=top->gb->video->sprites->spr__BRA__15__KET____DOT__sprite;
    sprites_array[16]=top->gb->video->sprites->spr__BRA__16__KET____DOT__sprite;
    sprites_array[17]=top->gb->video->sprites->spr__BRA__17__KET____DOT__sprite;
    sprites_array[18]=top->gb->video->sprites->spr__BRA__18__KET____DOT__sprite;
    sprites_array[19]=top->gb->video->sprites->spr__BRA__19__KET____DOT__sprite;
    sprites_array[20]=top->gb->video->sprites->spr__BRA__20__KET____DOT__sprite;
    sprites_array[21]=top->gb->video->sprites->spr__BRA__21__KET____DOT__sprite;
    sprites_array[22]=top->gb->video->sprites->spr__BRA__22__KET____DOT__sprite;
    sprites_array[23]=top->gb->video->sprites->spr__BRA__23__KET____DOT__sprite;
    sprites_array[24]=top->gb->video->sprites->spr__BRA__24__KET____DOT__sprite;
    sprites_array[25]=top->gb->video->sprites->spr__BRA__25__KET____DOT__sprite;
    sprites_array[26]=top->gb->video->sprites->spr__BRA__26__KET____DOT__sprite;
    sprites_array[27]=top->gb->video->sprites->spr__BRA__27__KET____DOT__sprite;
    sprites_array[28]=top->gb->video->sprites->spr__BRA__28__KET____DOT__sprite;
    sprites_array[29]=top->gb->video->sprites->spr__BRA__29__KET____DOT__sprite;
    sprites_array[30]=top->gb->video->sprites->spr__BRA__30__KET____DOT__sprite;
    sprites_array[31]=top->gb->video->sprites->spr__BRA__31__KET____DOT__sprite;
    sprites_array[32]=top->gb->video->sprites->spr__BRA__32__KET____DOT__sprite;
    sprites_array[33]=top->gb->video->sprites->spr__BRA__33__KET____DOT__sprite;
    sprites_array[34]=top->gb->video->sprites->spr__BRA__34__KET____DOT__sprite;
    sprites_array[35]=top->gb->video->sprites->spr__BRA__35__KET____DOT__sprite;
    sprites_array[36]=top->gb->video->sprites->spr__BRA__36__KET____DOT__sprite;
    sprites_array[37]=top->gb->video->sprites->spr__BRA__37__KET____DOT__sprite;
    sprites_array[38]=top->gb->video->sprites->spr__BRA__38__KET____DOT__sprite;
    sprites_array[39]=top->gb->video->sprites->spr__BRA__39__KET____DOT__sprite;



    // initialize simulation inputs
    top->clk_sys = 1;
    top->reset = 1;
    top->ce = 1;
    top->ce_2x = 1;
    top->isGBC = 1;
    loadvram("mario2dx-dumps/vram0-overworld.bin",top->gb->vram0_array);
    loadvram("mario2dx-dumps/vram1-overworld.bin",top->gb->vram1_array);

    setSprite(sprites_array[0] ,0x4C,0x4B,0x0A,0x00);
    setSprite(sprites_array[1] ,0x4C,0x53,0x0A,0x20);
    setSprite(sprites_array[2] ,0x54,0x4b,0x0b,0x00);
    setSprite(sprites_array[3] ,0x54,0x53,0x0c,0x20);
    setSprite(sprites_array[4] ,0x4C,0x4f,0xf4,0x09);
    setSprite(sprites_array[5] ,0x98,0x58,0x7c,0x07);
    setSprite(sprites_array[6] ,0x98,0x60,0x6f,0x01);
    setSprite(sprites_array[7] ,0x98,0x68,0x70,0x01);
    setSprite(sprites_array[8] ,0x98,0x70,0x78,0x01);
    setSprite(sprites_array[9] ,0x98,0x80,0x7d,0x06);
    setSprite(sprites_array[10] ,0x98,0x88,0x6f,0x01);
    setSprite(sprites_array[11] ,0x98,0x90,0x70,0x01);
    setSprite(sprites_array[12] ,0x98,0x98,0x71,0x01);
    setSprite(sprites_array[13] ,0x98,0xa0,0x74,0x01);
    setSprite(sprites_array[14] ,0x1E,0x2a,0x20,0x01);
    setSprite(sprites_array[15] ,0x1e,0x32,0x21,0x01);
    setSprite(sprites_array[16] ,0x1e,0x3a,0x22,0x01);
    setSprite(sprites_array[17] ,0x1e,0x42,0x23,0x01);
    setSprite(sprites_array[18] ,0x26,0x2a,0x24,0x01);
    setSprite(sprites_array[19] ,0x26,0x32,0x25,0x01);
    setSprite(sprites_array[20] ,0x26,0x3a,0x26,0x01);
    setSprite(sprites_array[21] ,0x26,0x42,0x27,0x01);


    top->eval ();  

    SDL_TimerID my_timer_id = SDL_AddTimer(2, timerTick, NULL);

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

    SDL_Texture* sprites[40];
    for (int i =0; i<40;i++){
        sprites[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 8,8);  
    }


    bool run = true;
    bool isGBC = true;
    bool runVerilator = false;
    int lcd_mode = -1;
    int lcd_mode_old = -2;
    int spriteinfo=0;
    char spriteinfo_buffer [100];
    
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
                    /* and now we can call the function we wanted to call in the timer but couldn't because of the multithreading problems */
                    if (runVerilator){
                        for (int z = 0; z<4*4;z++){
                            i++;
                            top->reset = (i < 2);
                            top->isGBC = isGBC;
                            top->gb->video->lcdc = (i > 2)?0xE3:0x00;
                            top->gb->video->bgp = 0xe4;
                            top->gb->video->obp0 = 0xd0;
                            top->gb->video->obp1 = 0x38;
                            top->gb->video->scy = 0x1A;
                            top->gb->video->scx = 0x81;
                            top->gb->video->wy = 0x88;
                            top->gb->video->wx = 0x50;

                            setGBCPalettes(top);

                            // dump variables into VCD file and toggle clock
                            for (clk=0; clk<2; clk++) {
                                tfp->dump (2*i+clk);
                                top->clk_sys = !top->clk_sys;
                                top->eval ();
                                lcd_mode_old = lcd_mode;
                                lcd_mode = top->lcd_mode;

                                if ((lcd_mode == 3) && (lcd_mode_old!=3)) { //draw things 1 time
                                    drawBackground(background,renderer,top);
                                    drawTileMap(tilemap,renderer,top,0);
                                    drawTileMap(tilemap2,renderer,top,1);
                                }
                                if ((lcd_mode == 0) && (lcd_mode_old!=0)) { //draw things 1 time
                                    drawLCD(lcd,renderer,top,isGBC);
                                    for (int i=0;i<40;i++){
                                        drawSprite(sprites[i],renderer,sprites_array[i],top,isGBC);
                                    }
                                }
                            }
                        }
                        if (Verilated::gotFinish()) run = false;
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

        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::Begin("Background");
        ImGui::Image(background, ImVec2(256*2, 256*2));
        ImGui::End();

        ImGui::Begin("TileMap");
        ImGui::Image(tilemap, ImVec2(271, 407));
        ImGui::SameLine();
        ImGui::Image(tilemap2, ImVec2(271, 407));
        ImGui::End();

        ImGui::Begin("LCD");
        ImGui::Image(lcd, ImVec2(160*3, 144*3));
        ImGui::End();

        ImGui::Begin("Sprites");
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
        bool True = true;
        bool False = false;
        //details 
        Vgb_sprite * selected_sprite = sprites_array[spriteinfo];
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
        ImGui::End();


        ImGui::Begin("GB Config");
        ImGui::Checkbox("Enable Gameboy Color?",&isGBC);
        ImGui::Checkbox("Run?",&runVerilator);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("LCD Mode","%d",lcd_mode);
        ImGui::SetNextItemWidth(100);
        ImGui::LabelText("Cycles Elapsed","%d",i+2);
        ImGui::End();

        SDL_SetRenderDrawColor(renderer, 114, 144, 154, 255);
        SDL_RenderClear(renderer);

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());

        SDL_RenderPresent(renderer);
    }

    
    tfp->close();

    ImGuiSDL::Deinitialize();

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(tilemap);
    SDL_DestroyTexture(tilemap2);
    SDL_DestroyTexture(lcd);

    for (int i =0; i<40;i++){
        SDL_DestroyTexture(sprites[i]) ;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui::DestroyContext();

    return 0;  

}