#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <stdio.h>
#include <time.h>

typedef size_t key_t;
typedef unsigned long bitmap_t;

#define BITS (8)
#define NR_LINES (1000)
#define BITS_PER_BITMAP (sizeof(bitmap_t) * BITS)
#define BITMAP_SIZE ((NR_LINES / BITS_PER_BITMAP) + 1)

#define INDEX_EMPTY ((size_t)(~0UL))

#define pr_info(msg, ...)                                                      \
        fprintf(stderr, "[{%lfs} %s(%s):%d] " msg,                             \
                ((double)clock() / CLOCKS_PER_SEC), __FILE__, __func__,        \
                __LINE__, ##__VA_ARGS__)

#define dynamic_array_set_bit(bitmap, index)                                   \
        (bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] |=                      \
         (1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_clear_bit(bitmap, index)                                 \
        (bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] &=                      \
         ~(1 << ((index) % BITS_PER_BITMAP)))
#define dynamic_array_test_bit(bitmap, index)                                  \
        ((bitmap[(bitmap_t)((index) / BITS_PER_BITMAP)] &                      \
          (1 << ((index) % BITS_PER_BITMAP))) != 0)

struct item {
        key_t key;
};

struct line {
        key_t min;
        key_t max;

        size_t size;
        struct item *items;
};

struct dynamic_array {
        size_t size;
        bitmap_t bitmap[BITMAP_SIZE];
        struct line lines[NR_LINES];
};

struct dynamic_array *dynamic_array_init();
struct item *dynamic_array_search(struct dynamic_array *array, key_t key);
int dynamic_array_insert(struct dynamic_array *array, const struct item item);
void dynamic_array_free(struct dynamic_array *array);

static inline void dynamic_array_bitmap_init(bitmap_t *bitmap)
{
        int i;
        for (i = 0; i < BITMAP_SIZE; i++) {
                bitmap[i] &= 0x0;
        }
}

static inline size_t __dynamic_array_find_first_zero_bit(bitmap_t *bitmap,
                                                         size_t bitmap_idx)
{
        size_t i, pos;
        if (bitmap[bitmap_idx] == (size_t)(~0UL)) {
                return INDEX_EMPTY;
        }

        for (i = 0; i < BITS_PER_BITMAP; i++) {
                pos = bitmap_idx * BITS_PER_BITMAP + i;
                if (!dynamic_array_test_bit(bitmap, pos)) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

static inline size_t dynamic_array_find_first_zero_bit(bitmap_t *bitmap)
{
        size_t i, pos;
        for (i = 0; i < BITMAP_SIZE; i++) {
                pos = __dynamic_array_find_first_zero_bit(bitmap, i);
                if (pos != INDEX_EMPTY) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

static inline size_t __dynamic_array_find_first_bit(bitmap_t *bitmap,
                                                    size_t bitmap_idx)
{
        size_t i, pos;
        if (bitmap[bitmap_idx] == (size_t)(~0UL)) {
                return INDEX_EMPTY;
        }

        for (i = 0; i < BITS_PER_BITMAP; i++) {
                pos = bitmap_idx * BITS_PER_BITMAP + i;
                if (dynamic_array_test_bit(bitmap, pos)) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

static inline size_t dynamic_array_find_first_bit(bitmap_t *bitmap)
{
        size_t i, pos;
        for (i = 0; i < BITMAP_SIZE; i++) {
                pos = __dynamic_array_find_first_bit(bitmap, i);
                if (pos != INDEX_EMPTY) {
                        return pos;
                }
        }

        return INDEX_EMPTY;
}

#endif