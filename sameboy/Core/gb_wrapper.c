#include <Core/gb.h>
#include <Core/random.h>

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define snprintf _snprintf
#else
#include <sys/wait.h>
#endif

#include "gb_wrapper.h"


GB_gameboy_t gb_internal;

// static char *async_input_callback(GB_gameboy_t *gb)
// {
//     return NULL;
// }

static void vblank(GB_gameboy_t *gb)
{
    printf("vsync at %d\n",gb->current_line);
}

static uint32_t rgb_encode(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8);
}


bool GB_Init(char * filename, uint32_t *output) {
    GB_random_set_enabled(false);
    GB_init(&gb_internal, GB_MODEL_DMG_B);
    GB_set_vblank_callback(&gb_internal, (GB_vblank_callback_t) vblank);
    if (GB_load_boot_rom(&gb_internal, "/mnt/C/verilator/dmg_boot.bin")) {
        perror("Failed to load boot ROM");
        return false;
    }

    GB_set_rgb_encode_callback(&gb_internal, rgb_encode);

    GB_set_pixels_output(&gb_internal, output);

    /* Configure symbols */
    GB_debugger_load_symbol_file(&gb_internal, "/mnt/C/verilator/registers.sym");

    if (GB_load_rom(&gb_internal, filename)) {
            perror("Failed to load ROM");
            return false;
    }
    gb_internal.turbo = gb_internal.turbo_dont_skip = gb_internal.disable_rendering = true;

    return true;
}

unsigned int runGB(struct gbwrapper * gb){
    unsigned int cycles_sameboy = GB_run(&gb_internal);
    gb->a = gb_internal.a;
    gb->f = gb_internal.f;
    gb->b = gb_internal.b;
    gb->c = gb_internal.c;
    gb->d = gb_internal.d;
    gb->e = gb_internal.e;
    gb->pc = gb_internal.pc;
    gb->hl = gb_internal.hl;
    gb->sp = gb_internal.sp;
    gb->ime = gb_internal.ime;
    gb->interrupt_enable = gb_internal.interrupt_enable;
    gb->halted = gb_internal.halted;
    gb->stopped = gb_internal.stopped;

    // PPU
    gb->display_cycles = gb_internal.display_cycles;
    gb->display_state = gb_internal.display_state;
    gb->current_line = gb_internal.current_line;
    gb->ly_for_comparison = gb_internal.ly_for_comparison;
    gb->cycles_for_line = gb_internal.cycles_for_line;
    gb->lyc_interrupt_line = gb_internal.lyc_interrupt_line;
    gb->effective_scx = gb_internal.effective_scx;
    gb->wy_diff = gb_internal.wy_diff;
    gb->position_in_line = gb_internal.position_in_line;
    gb->stat_interrupt_line = gb_internal.stat_interrupt_line;

    //DIV
    gb->div_counter= gb_internal.div_counter;
    gb->div_cycles= gb_internal.div_cycles;
    gb->div_state= gb_internal.div_state;
    gb->tima_reload_state= gb_internal.tima_reload_state;


    memcpy(gb->io_registers,gb_internal.io_registers,0x80);

    return cycles_sameboy;
}

void  getInfo(struct gbwrapper * gb) {
    gb->a = gb_internal.a;
    gb->f = gb_internal.f;
    gb->pc = gb_internal.pc;
    gb->hl = gb_internal.hl;
    gb->sp = gb_internal.sp;
    gb->ime = gb_internal.ime;
    gb->interrupt_enable = gb_internal.interrupt_enable;
    gb->halted = gb_internal.halted;
    gb->stopped = gb_internal.stopped;

    // PPU
    gb->display_cycles = gb_internal.display_cycles;
    gb->display_state = gb_internal.display_state;
    gb->current_line = gb_internal.current_line;
    gb->ly_for_comparison = gb_internal.ly_for_comparison;
    gb->cycles_for_line = gb_internal.cycles_for_line;
    gb->lyc_interrupt_line = gb_internal.lyc_interrupt_line;
    gb->effective_scx = gb_internal.effective_scx;
    gb->wy_diff = gb_internal.wy_diff;
    gb->position_in_line = gb_internal.position_in_line;
    gb->stat_interrupt_line = gb_internal.stat_interrupt_line;

    //DIV
    gb->div_counter= gb_internal.div_counter;
    gb->div_cycles= gb_internal.div_cycles;
    gb->div_state= gb_internal.div_state;
    gb->tima_reload_state= gb_internal.tima_reload_state;

    memcpy(gb->io_registers,gb_internal.io_registers,0x80);
}

void freeGB(){
    GB_free(&gb_internal);
}
