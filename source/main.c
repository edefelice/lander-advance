#include <tonc.h>

int main(void) {
    REG_DISPCNT = DCNT_MODE(3) | DCNT_BG2;
    memset16(vid_mem, RGB15(31, 0, 0), 240 * 160);
    while(1);
}