#include <rp6502.h>
#include <stdio.h>
#include <stdint.h>


/* optimize y-var in calls by using 8-bits since y<240 */
void    set(int x, uint8_t y);
void  unset(int x, uint8_t y);
uint8_t get(int x, uint8_t y);

void setup(void);
void next(void);
void main(void); 
