#include "Vgb.h"
#include "Vgb_gb.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

    
#include "SDL.h"

#include "imgui.h"
#include "imgui_sdl.h"


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
    top->gb->vram0_array[0] = 0x55;
    top->gb->vram1_array[0] = 0x55;
    top->eval ();
    
    // run simulation for 100 clock periods
    for (i=0; i<20; i++) {
      top->reset = (i < 2);
      // dump variables into VCD file and toggle clock
      for (clk=0; clk<2; clk++) {
        tfp->dump (2*i+clk);
        top->clk_sys = !top->clk_sys;
        top->eval ();
      }
      if (Verilated::gotFinish())  exit(0);
    }
    tfp->close();

    SDL_Window* window = SDL_CreateWindow("SDL2 ImGui Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, 1280, 720);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 100, 100);
    
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawPoint(renderer,51,50);
    SDL_RenderDrawPoint(renderer,50,51);
    SDL_RenderDrawPoint(renderer,51,51);
    SDL_RenderDrawPoint(renderer,50,50);
    SDL_RenderDrawPoint(renderer,49,50);
    SDL_RenderDrawPoint(renderer,50,51);
    SDL_RenderDrawPoint(renderer,51,51);
    SDL_RenderDrawPoint(renderer,50,50);
    SDL_SetRenderTarget(renderer, nullptr);
    

	bool run = true;
	while (run)
	{
		ImGuiIO& io = ImGui::GetIO();

		int wheel = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) run = false;
			else if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					io.DisplaySize.x = static_cast<float>(e.window.data1);
					io.DisplaySize.y = static_cast<float>(e.window.data2);
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				wheel = e.wheel.y;
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

		ImGui::Begin("Image");
		ImGui::Image(texture, ImVec2(100, 100));
		ImGui::End();

		SDL_SetRenderDrawColor(renderer, 114, 144, 154, 255);
		SDL_RenderClear(renderer);

		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);
	}

	ImGuiSDL::Deinitialize();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui::DestroyContext();

	return 0;  

}