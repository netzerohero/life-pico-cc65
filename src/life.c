#include <rp6502.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h> /* for clock() instrumentation; 
                   * needs cc65 w/ pico6502 lib support; 
                   * build cc65 locally from pico's repo.
                   */ 
#include "life.h"

/* buf[] of current-state used in rule-evaluation */
uint8_t buf[320 / 8 * 180];

/*
 * For the set(), unset(), and get() functions,
 * since the y-pixel value is less than 240,
 * using an 8-bit rather than a 16-var improved
 * loop-speed on cc65 from 96.51 to 94.86-secs (8Mhz-6502).
 * 
 * Optimization summary on cc65 (using 'Release' build configuration)
 *   loop-times went from 102.31-sec down to 94.86-secs.
 */


/* 
 * Note: found that using statics for 'bit' in set() & unset()
 *  actually increased loop-time from 94.86 to 95.76-seconds.
 */

/* 
 * Note: set() and unset operate at the bit-level, not byte level.
 * When coupled with read-modify-write to set or clear a bit,
 * this is computationally very inefficient.
 * 
 * Compare to initializing the screen in function setup(),
 *  which is performed at the byte-level.
 * Clearing the screen is computationally efficient.
 * 
 * Exercise: convert this program to byte-level manipulation using a 8bpp-screen.
 * 
 */

/*static inline*/ void set(int x, uint8_t y)
{
    uint8_t bit = 128 >> (x % 8);

    RIA.addr0 = (x / 8) + (320 / 8 * y);
//  RIA.addr0 = (x / 8) + (40 * y);      //no advantage to pre-compute the 40

    RIA.step0 = 0;
//  uint8_t bit = 128 >> (x % 8);
    RIA.rw0 |= bit;
}

/*static inline*/ void unset(int x, uint8_t y)
{
    uint8_t bit = 128 >> (x % 8);

    RIA.addr0 = (x / 8) + (320 / 8 * y);
//  RIA.addr0 = (x / 8) + (40 * y);      //no advantage to pre-compute the 40

    RIA.step0 = 0;
//  uint8_t bit = 128 >> (x % 8);
    RIA.rw0 &= ~bit;
}

/*static inline*/ uint8_t get(int x, uint8_t y)
{
    /* changing these vars to statics reduce loop-times from 101.57 to 96.51-secs in cc65 */
    static uint8_t bit;
    static unsigned addr;

    bit = 128 >> (x % 8);
    addr = (x / 8) + (320 / 8 * y);

    return buf[addr] & bit ? 1 : 0;  //is cell alive or dead?
}

/*static*/ void setup(void)
{
    uint16_t i;


//  xregn(1, 0, 0, 1, 2);
//  xregn(1, 0, 1, 3, 3, 0, 0xff00);
     xreg(1, 0, 0, 2);            //2=320x180-canvas-dimension
     xreg(1, 0, 1, 3, 0, 0xff00); //bitmapped, 1-bpp screen

    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 180);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0xFFFF);

#if 1
    /* Initialize the screen in XRAM.*/
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (/*unsigned*/ i = sizeof(buf); i--;)
        RIA.rw0 = 0xff;  //0=all-dead; 0xff=all-alive
#endif
//  Initial screen is now either all white or all black.

    next(); // do one generation (~100-secs @ 8Mhz) on initial screen then place a glider.

    /* 
     * Initial condition: Glider
     *  Reference: https://en.wikipedia.org/wiki/Glider_(Conway's_Game_of_Life)
     */
    set(11, 20);
    set(12, 21);
    set(10, 22);
    set(11, 22);
    set(12, 22);
}

/*static*/ void next(void)
{
    /* static vars rather than automatic - for speed */
    /* 
     * With phi2 == 8000 == 8Mhz-6502, loop times
     * decreased from 102.31-sec to 101.57-seconds.
     */

    static uint16_t i;
    static uint16_t x;
    static uint8_t  y;
    static uint8_t neighbors;

    /* Keep a copy of current state in buf[] (from video screen's XRAM) */
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (/*unsigned*/ i = 0; i < sizeof(buf); i++)
        buf[i] = RIA.rw0;


    for (/*int*/ x = 1; x < 319; x++)
    {
        for (/*int*/ y = 1; y < 179; y++)
        {

            /* 
             * Evaluate life/death rules
             *  Reference: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
             * 
             *  1. Any live cell with fewer than two live neighbors dies, as if by underpopulation.
             *  2. Any live cell with two or three live neighbors lives on to the next generation.
             *  3. Any live cell with more than three live neighbors dies, as if by overpopulation.
             *  4. Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
             * 
             *  5. (Implicit Rule):
             *     Any dead cell with other than three live neighbors remains dead.
             * 
             * 
             *  Reference: https://rosettacode.org/wiki/Conway's_Game_of_Life
             * 
             *  Cell  #Neighbors    new Cell
             *    1   0,1             ->  0  # Lonely      - Rule#1
             *    1   2,3             ->  1  # Lives       - Rule#2
             *    1   4,5,6,7,8       ->  0  # Overcrowded - Rule#3
             *    0   3               ->  1  # It takes three to give birth! - Rule#4
             *    0   0,1,2,4,5,6,7,8 ->  0  # Barren      - Rule#5
             * 
             */

            /*uint8_t*/ neighbors =
                get(x - 1, y - 1) +
                get(x, y - 1) +
                get(x + 1, y - 1) +
                get(x - 1, y) +
                get(x + 1, y) +
                get(x - 1, y + 1) +
                get(x, y + 1) +
                get(x + 1, y + 1);
            if (get(x, y))
            {
                // Rules #1 & #3
                if (neighbors != 2 && neighbors != 3)
                    unset(x, y); //death
            }
            else
            {
                // Rule #4
                if (neighbors == 3)
                    set(x, y); //birth
            }

              // Rules #2 & #5 
              //  Any live cell with two or three live neighbors lives on to the next generation.
              //  Any dead cell with other than three live neighbors remains dead.
              /*  - do nothing */

        } //end for(y)
    } // end for(x)
} // end next()


void main(void)
{
    uint16_t i;
    uint32_t clock_old;
    uint32_t clock_now;


    printf("Setting up screen for Life\n");
    setup();

    for (i=0; 1; i++)    
 // for (/*int*/ i = 500; i>0; i--)
    {

        clock_now = clock();
        if (i==0) clock_old = clock_now;
        printf("loop i= %d clock()= 0x%06.6lx delta=%ld \n", i, clock_now, (clock_now - clock_old) );

        next();
        clock_old = clock_now;

    } //end for(i)

    // while (1)
    //     ;

    printf("Done.\n");

} //end main()