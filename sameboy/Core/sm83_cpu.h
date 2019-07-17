#ifndef sm83_cpu_h
#define sm83_cpu_h

#ifdef __cplusplus
extern "C" {  
#endif  

#include "gb_struct_def.h"
#include <stdint.h>

void GB_cpu_disassemble(GB_gameboy_t *gb, uint16_t pc, uint16_t count);
#ifdef GB_INTERNAL
void GB_cpu_run(GB_gameboy_t *gb);
#endif

#ifdef __cplusplus  
} // extern "C"  
#endif


#endif /* sm83_cpu_h */
