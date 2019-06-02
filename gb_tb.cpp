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

                    SDL_SetRenderDrawColor(renderer, (r10&0x1FE)>>1, (g10&0x7F)<<1, (b10&0x1FE)>>1,SDL_ALPHA_OPAQUE);
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

void setSprite(Vgb_sprite* sprite, int8_t y_pos, int8_t x_pos, int8_t tile, int8_t flags ) {
    sprite->y_pos = y_pos;
    sprite->x_pos = x_pos;
    sprite->tile = tile;
    sprite->flags = flags;
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
    /*Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("gb.vcd");*/

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

    SDL_TimerID my_timer_id = SDL_AddTimer(1, timerTick, NULL);

    const int window_x = 1980;
    const int window_y = 1080;

    SDL_Window* window = SDL_CreateWindow("SDL2 ImGui Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_x, window_y, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, window_x, window_y);
   
    SDL_Texture* background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 256, 256);
    SDL_Texture* tilemap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 271,407);
    SDL_Texture* lcd = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 160,144);

    SDL_Texture* sprites[40];
    for (int i =0; i<40;i++){
        sprites[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 16,16);  
    }


    bool run = true;
    bool isGBC = false;
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
                            top->gb->isGBC = isGBC;
                            top->gb->video->lcdc = (i > 2)?0xE3:0x00;
                            top->gb->video->bgp = 0xe4;
                            top->gb->video->obp0 = 0xd0;
                            top->gb->video->obp1 = 0x38;
                            top->gb->video->scy = 0x1A;
                            top->gb->video->scx = 0x81;
                            top->gb->video->wy = 0x88;
                            top->gb->video->wx = 0x50;

                            // dump variables into VCD file and toggle clock
                            for (clk=0; clk<2; clk++) {
                                //tfp->dump (2*i+clk);
                                top->clk_sys = !top->clk_sys;
                                top->eval ();
                                lcd_mode_old = lcd_mode;
                                lcd_mode = top->lcd_mode;

                                if ((lcd_mode == 3) && (lcd_mode_old!=3)) { //draw things 1 time
                                    drawBackground(background,renderer,top);
                                    drawTileMap(tilemap,renderer,top);
                                }
                                if ((lcd_mode == 0) && (lcd_mode_old!=0)) { //draw things 1 time
                                    drawLCD(lcd,renderer,top,0);
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

        ImGui::ShowDemoWindow();

        ImGui::Begin("Background");
        ImGui::Image(background, ImVec2(256*2, 256*2));
        ImGui::End();

        ImGui::Begin("TileMap");
        ImGui::Image(tilemap, ImVec2(271, 407));
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

    
    //tfp->close();

    ImGuiSDL::Deinitialize();

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(tilemap);
    SDL_DestroyTexture(lcd);

    for (int i =0; i<40;i++){
        SDL_DestroyTexture(sprites[i]) ;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui::DestroyContext();

    return 0;  

}