#include "dynamic-array.h"
#include <stdlib.h> // qsort 및 malloc을 위해서 존재
#include <errno.h>
#include <string.h>

static int dynamic_array_line_alloc(struct dynamic_array *array,
                                    unsigned long line_index)
{
        struct line *line;
        if (line_index >= NR_LINES) {
                pr_info("line index is over(%ld/%d)", line_index, NR_LINES);
                return -EINVAL;
        }

        line = &array->lines[line_index];
        line->array = (struct item *)calloc(line->size, sizeof(struct item));
        if (line->array == NULL) {
                pr_info("Memory allocation failed\n");
                return -ENOMEM;
        }

        dynamic_array_set_bit(array->bitmap, line_index);

        return 0;
}

static int dynamic_array_line_dealloc(struct dynamic_array *array,
                                      unsigned long line_index)
{
        struct line *line;
        if (line_index >= NR_LINES) {
                pr_info("line index is over(%ld/%d)", line_index, NR_LINES);
                return -EINVAL;
        }

        line = &array->lines[line_index];
        if (line->array != NULL) {
                free(line->array);
        }

        dynamic_array_clear_bit(array->bitmap, line_index);

        return 0;
}

int dynamic_array_insert(struct dynamic_array *array, const struct item item)
{
        struct line *current_line = &array->lines[array->index];

        if (current_line == NULL) {
                pr_info("Invalid location detected %ld\n", array->index);
                return -EACCES;
        }

        if (current_line->size == current_line->index) {
        }
}

int dynamic_array_init(struct dynamic_array *array)
{
        array = (struct dynamic_array *)malloc(sizeof(struct dynamic_array));
        if (array == NULL) {
                pr_info("Memory allocation failed\n");
                return -ENOMEM;
        }
        array->index = 0;
        dynamic_array_bitmap_init(array->bitmap_mask);
        dynamic_array_bitmap_init(array->bitmap);
        for (int line_index = 0; line_index < NR_LINES; line_index++) {
                struct line *line = &array->lines[line_index];
                line->size = (1 << line_index);
                line->index = 0;
                line->array = NULL;
        }
        return 0;
}

void dynamic_array_free(struct dynamic_array *array)
{
        for (int line_index = 0; line_index < NR_LINES; line_index++) {
                dynamic_array_line_dealloc(array, line_index);
        } // end of for
        free(array);
}
