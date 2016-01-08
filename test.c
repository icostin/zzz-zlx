#include <stdint.h>
#include <stdio.h>
#include <zlx.h>

#define ZLX_BODY
// #define T uint8_t
// #define F(_x) zlx_u8a_##_x

#define T uint8_t
#include <zlx/array.h>

#define T char
#define F(_n) zlx_cha_##_n
#include <zlx/array.h>

int main ()
{
    char c[9];
    uint8_t b[0x10];

    printf("zlx test using %s\n", zlx_lib_name);
    zlx_cha_set(c, 8, 'a');
    zlx_u8a_set(b, 4, 'b');
    c[8] = 0;
    printf("c: \"%s\"\n", c);
    printf("cmp: %d\n", zlx_cha_cmp(c, "aaaaaaa@", 8));
    return 0;
}

