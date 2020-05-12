#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <stdio.h>
#include <time.h>

#define BITS (8)
#define NR_LINES (100)
#define BITS_PER_BITMAP (sizeof(unsigned long) * BITS)
#define BITMAP_SIZE ((NR_LINES / BITS_PER_BITMAP) + 1)

#define pr_info(msg, ...)                                                      \
        fprintf(stderr, "[{%lfs} %s(%s):%d] " msg,                             \
                ((double)clock() / CLOCKS_PER_SEC), __FILE__, __func__,        \
                __LINE__, ##__VA_ARGS__)

#define dynamic_array_set_bit(bitmap, index)                                   \
        (bitmap[(unsigned long)((index) / BITS_PER_BITMAP)] |=                 \
         (1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_clear_bit(bitmap, index)                                 \
        (bitmap[(unsigned long)((index) / BITS_PER_BITMAP)] &=                 \
         ~(1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_test_bit(bitmap, index)                                  \
        ((bitmap[(unsigned long)((index) / BITS_PER_BITMAP)] &                 \
          (1 << ((index) % BITS_PER_BITMAP))) != 0)

struct item {
        int value;
};

struct line {
        unsigned long size;
        unsigned long index;
        struct item *array;
};

/**
 * @brief bitmap_mask의 경우 2^(next_line_num) - 1을 따르며, next_line_num은 0보다 크다.
 * 
 */
struct dynamic_array {
        unsigned long index;
        unsigned long bitmap_mask[BITMAP_SIZE];
        unsigned long bitmap[BITMAP_SIZE];
        struct line lines[NR_LINES];
};

int dynamic_array_init(struct dynamic_array *array);
void dynamic_array_free(struct dynamic_array *array);

static inline void dynamic_array_bitmap_init(unsigned long *bitmap)
{
        for (int i = 0; i < BITMAP_SIZE; i++) {
                bitmap[i] &= 0x0;
        }
}

static inline void dynamic_array_calc_bitmap_mask(unsigned long *bitmap_mask,
                                                  unsigned long next_line_num)
{
        for (int i = 0; i < BITMAP_SIZE; i++) {
                if (next_line_num >= BITS_PER_BITMAP) {
                        bitmap_mask[i] |= ((unsigned long)(~0UL));
                } else {
                        bitmap_mask[i] |=
                                ((unsigned long)(1 << next_line_num) - 1);
                } // end of if

                if (next_line_num < BITS_PER_BITMAP) {
                        break;
                }
                next_line_num -= BITS_PER_BITMAP;
        } // end of for
}

#endif