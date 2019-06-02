#include "Vgb.h"
#include "Vgb_gb.h"
#include "Vgb_video.h"
#include "Vgb_lcd.h"
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

void drawLCD(SDL_Texture* tilemap, SDL_Renderer* renderer, Vgb* top) {
    SDL_SetRenderTarget(renderer, tilemap);
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        
        int i=0;
        for (int y=0; y<144;y++){
            for (int x=0;x<160;x++) {

                int r5 = top->gb->lcd->lcd_buffer[i]&0x1F;
                int g5 = (top->gb->lcd->lcd_buffer[i]>>5)&0x1F;
                int b5 = (top->gb->lcd->lcd_buffer[i]>>10)&0x1F;

                int r10 = (r5 * 13) + (g5 * 2) +b5;
                int g10 = (g5 * 3) + b5;
                int b10 = (r5 * 3) + (g5 * 2) + (b5 * 11);

                SDL_SetRenderDrawColor(renderer, (r10&0x1FE)>>1, (g10&0x7F)<<1, (b10&0x1FE)>>1,SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer,x,y);
                i++;
            }
        }
        SDL_SetRenderTarget(renderer, nullptr);
    }
}


int main(int argc, char **argv) {
	
    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    
    int i;
    int clk;
    Verilated::commandArgs(argc, argv);
    // init top verilog instance
    Vgb* top = new Vgb;
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("gb.vcd");
    // initialize simulation inputs
    top->clk_sys = 1;
    top->reset = 1;
    top->ce = 1;
    top->ce_2x = 1;
    loadvram("mario2dx-dumps/vram0-overworld.bin",top->gb->vram0_array);
    loadvram("mario2dx-dumps/vram1-overworld.bin",top->gb->vram1_array);
    top->eval ();  

    Uint32 delay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */

    SDL_TimerID my_timer_id = SDL_AddTimer(1, timerTick, NULL);

    SDL_Window* window = SDL_CreateWindow("SDL2 ImGui Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, 1280, 720);
   
    SDL_Texture* background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 256, 256);
    SDL_Texture* tilemap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 271,407);
    SDL_Texture* lcd = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_TARGET, 160,144);
    //TODO: sprites
    


	bool run = true;
    bool isGBC = false;
    bool runVerilator = false;
    int lcd_mode = -1;
    int lcd_mode_old = -2;
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
                        i++;
                        top->reset = (i < 2);
                        top->gb->video->lcdc = (i > 2)?0x80:0x00;
                        top->gb->video->bgp = 0xe4;
                        top->gb->video->obp0 = 0xd0;
                        top->gb->video->obp1 = 0x38;
                        // dump variables into VCD file and toggle clock
                        for (clk=0; clk<2; clk++) {
                            tfp->dump (2*i+clk);
                            top->clk_sys = !top->clk_sys;
                            top->eval ();
                            lcd_mode_old = lcd_mode;
                            lcd_mode = top->lcd_mode;

                            if ((lcd_mode == 3) && (lcd_mode_old!=3)) { //draw things 1 time
                                drawBackground(background,renderer,top);
                                drawTileMap(tilemap,renderer,top);
                            }
                            if ((lcd_mode == 0) && (lcd_mode_old!=0)) { //draw things 1 time
                                drawLCD(lcd,renderer,top);
                            }

                            //TODO: connect buffer to video and draw that 
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
        ImGui::Image(background, ImVec2(256, 256));
        ImGui::End();

        ImGui::Begin("TileMap");
        ImGui::Image(tilemap, ImVec2(271, 407));
        ImGui::End();

        ImGui::Begin("LCD");
        ImGui::Image(lcd, ImVec2(160, 144));
        ImGui::End();


        ImGui::Begin("GB Config");
        ImGui::Checkbox("Enable Gameboy Color?",&isGBC);
        ImGui::Checkbox("Run?",&runVerilator);
        ImGui::LabelText("LCD Mode","%d",lcd_mode);
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui::DestroyContext();

    return 0;  

}