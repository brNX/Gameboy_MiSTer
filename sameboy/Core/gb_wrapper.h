 
#ifndef gb_wrapper_h
#define gb_wrapper_h

#ifdef __cplusplus
extern "C" {
#endif
    
        
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

struct gbwrapper{
        /* Registers */
        uint16_t pc;
        uint16_t hl,
                 sp;
#ifdef GB_BIG_ENDIAN
       uint8_t a, f,
               b, c,
               d, e,
               h, l;
#else
       uint8_t f, a,
               c, b,
               e, d,
               l, h;
#endif
        uint8_t ime;
        uint8_t interrupt_enable;
        bool halted;
        bool stopped;

        //DIV
        uint16_t div_counter;
        int32_t div_cycles;
        int32_t div_state;
        uint8_t tima_reload_state;

        // PPU
        int32_t display_cycles;
        int32_t display_state;
        uint8_t current_line;
        uint16_t ly_for_comparison;
        uint16_t cycles_for_line;
        bool lyc_interrupt_line;
        uint8_t effective_scx;
        uint8_t wy_diff;
        uint8_t position_in_line;
        bool stat_interrupt_line;

        // IO registers
        uint8_t io_registers[0x80];
};

bool GB_Init(char * filename, uint32_t *output);
unsigned int  runGB(struct gbwrapper * gb);
void  getInfo(struct gbwrapper * gb);
void freeGB();

#ifdef __cplusplus
} // extern "C"
#endif

#endif 
