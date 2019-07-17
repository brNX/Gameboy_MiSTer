#ifndef random_h
#define random_h

#ifdef __cplusplus
extern "C" {  
#endif  
    
#include <stdint.h>
#include <stdbool.h>

uint8_t GB_random(void);
uint32_t GB_random32(void);
void GB_random_seed(uint64_t seed);
void GB_random_set_enabled(bool enable);

#ifdef __cplusplus  
} // extern "C"  
#endif


#endif /* random_h */
