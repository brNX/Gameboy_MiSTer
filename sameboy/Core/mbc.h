#ifndef MBC_h
#define MBC_h

#ifdef __cplusplus
extern "C" {  
#endif  
    
   
#include "gb_struct_def.h"
#include <stdbool.h>

typedef struct {
    enum {
        GB_NO_MBC,
        GB_MBC1,
        GB_MBC2,
        GB_MBC3,
        GB_MBC5,
        GB_HUC1, /* Todo: HUC1 features are not emulated. Should be unified with the CGB IR sensor API. */
        GB_HUC3,
    } mbc_type;
    enum {
        GB_STANDARD_MBC,
        GB_CAMERA,
    } mbc_subtype;
    bool has_ram;
    bool has_battery;
    bool has_rtc;
    bool has_rumble;
} GB_cartridge_t;

#ifdef GB_INTERNAL
extern const GB_cartridge_t GB_cart_defs[256];
void GB_update_mbc_mappings(GB_gameboy_t *gb);
void GB_configure_cart(GB_gameboy_t *gb);
#endif

#ifdef __cplusplus  
} // extern "C"  
#endif


#endif /* MBC_h */
