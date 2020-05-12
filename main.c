#include <stdio.h>
#include "dynamic-array.h"

int main(void)
{
        unsigned long bitmap_mask[BITMAP_SIZE];
        dynamic_array_bitmap_init(bitmap_mask);
        for (unsigned long i = 1; i <= NR_LINES /*NR_LINES*/; i++) {
                dynamic_array_clear_bit(bitmap_mask, i - 1);
                dynamic_array_set_bit(bitmap_mask, i);
                for (int j = 0; j < BITMAP_SIZE; j++) {
                        for (int k = 0; k < BITS_PER_BITMAP; k++) {
                                printf("%d",
                                       dynamic_array_test_bit(
                                               bitmap_mask,
                                               (j * BITS_PER_BITMAP + k)));
                        }
                        printf(" ");
                }
                printf("\n");
        }
        return 0;
}