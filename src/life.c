#include <rp6502.h>
#include <stdio.h>
#include <stdint.h>

uint8_t buf[320 / 8 * 180];

static inline void set(int x, int y)
{
    RIA.addr0 = (x / 8) + (320 / 8 * y);
    RIA.step0 = 0;
    uint8_t bit = 128 >> (x % 8);
    RIA.rw0 |= bit;
}

static inline void unset(int x, int y)
{
    RIA.addr0 = (x / 8) + (320 / 8 * y);
    RIA.step0 = 0;
    uint8_t bit = 128 >> (x % 8);
    RIA.rw0 &= ~bit;
}

static inline uint8_t get(int x, int y)
{
    unsigned addr = (x / 8) + (320 / 8 * y);
    uint8_t bit = 128 >> (x % 8);
    return buf[addr] & bit ? 1 : 0;
}

static void setup()
{
    xregn(1, 0, 0, 1, 2);
    xregn(1, 0, 1, 3, 3, 0, 0xff00);

    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 180);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0xFFFF);

    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = sizeof(buf); i--;)
        RIA.rw0 = 0;

    set(11, 20);
    set(12, 21);
    set(10, 22);
    set(11, 22);
    set(12, 22);
}

static void next()
{
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = 0; i < sizeof(buf); i++)
        buf[i] = RIA.rw0;

    for (int x = 1; x < 319; x++)
    {
        for (int y = 1; y < 179; y++)
        {
            uint8_t neighbors =
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
                if (neighbors != 2 && neighbors != 3)
                    unset(x, y);
            }
            else
            {
                if (neighbors == 3)
                    set(x, y);
            }
        }
    }
}

int main(void)
{
    setup();

    for (int i = 500; i--;)
        next();

    // while (1)
    //     ;
}