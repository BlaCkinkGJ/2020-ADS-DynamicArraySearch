#include "dynamic-array.h"
#include <stdlib.h> // qsort 및 malloc을 위해서 존재
#include <errno.h>
#include <string.h>

static int dynamic_array_line_alloc(struct dynamic_array *array,
                                    size_t line_index)
{
        struct line *line;
        if (line_index >= NR_LINES) {
                pr_info("line index is over(" SIZE_T_FORMAT "/%d)", line_index,
                        NR_LINES);
                return -EINVAL;
        }

        line = &array->lines[line_index];
        line->items = (struct item *)calloc(line->size, sizeof(struct item));
        if (line->items == NULL) {
                pr_info("Memory allocation failed\n");
                return -ENOMEM;
        }

        dynamic_array_set_bit(array->bitmap, line_index);

        return 0;
}

static int dynamic_array_line_dealloc(struct dynamic_array *array,
                                      size_t line_index)
{
        struct line *line;
        if (line_index >= NR_LINES) {
                pr_info("line index is over(" SIZE_T_FORMAT "/%d)", line_index,
                        NR_LINES);
                return -EINVAL;
        }

        line = &array->lines[line_index];
        if (line->items != NULL) {
                free(line->items);
                line->items = NULL;
        }

        line->min = line->max = -1;

        dynamic_array_clear_bit(array->bitmap, line_index);

        return 0;
}

static int dynamic_array_compare(const void *left, const void *right)
{
        struct item *item1 = (struct item *)left;
        struct item *item2 = (struct item *)right;

        if (item1->key < item2->key) {
                return -1;
        }

        if (item1->key > item2->key) {
                return 1;
        }

        return 0;
}

int dynamic_array_insert(struct dynamic_array *array, const struct item item)
{
        int ret;
        size_t next_pos, line_pos, item_pos, next_item_pos;
        struct line *line = NULL;
        struct line *next_line = NULL;
        struct item *item_ptr = NULL;

        next_item_pos = 0;

        next_pos = dynamic_array_find_first_zero_bit(array->bitmap);

        ret = dynamic_array_line_alloc(array, next_pos);
        if (ret) {
                goto exception;
        }

        next_line = &array->lines[next_pos];
        for (line_pos = 0; line_pos < next_pos; line_pos++) {
                line = &array->lines[line_pos];
                for (item_pos = 0; item_pos < line->size; item_pos++) {
                        item_ptr = &next_line->items[next_item_pos++];
                        memcpy(item_ptr, &line->items[item_pos],
                               sizeof(struct item));
                } // end of line copy
                dynamic_array_line_dealloc(array, line_pos);
        } // end of previous items copy

        // item copy sequence
        item_ptr = &next_line->items[next_item_pos];
        memcpy(item_ptr, &item, sizeof(struct item));

        qsort(next_line->items, next_line->size, sizeof(struct item),
              dynamic_array_compare);

        next_line->min = next_line->items[0].key;
        next_line->max = next_line->items[next_item_pos].key;
        if (next_pos + 1 > array->size) {
                array->size = next_pos + 1;
        }
exception:
        return ret;
}

static struct item *__dynamic_array_search(struct line *line, key_t key)
{
        struct item *item = NULL;
        size_t low = 0;
        size_t high = line->size - 1;
        size_t mid;

        while (low <= high) { // binary search operation
                mid = (low + high) / 2;
                item = &line->items[mid];

                if (item->key == key) {
                        return item;
                } else if (item->key > key) {
                        high = mid - 1;
                } else {
                        low = mid + 1;
                }
        }
        return NULL;
}

struct item *dynamic_array_search(struct dynamic_array *array, key_t key)
{
        size_t i;
        key_t min, max;
        struct item *item = NULL;
        for (i = 0; i < array->size; i++) {
                min = array->lines[i].min;
                max = array->lines[i].max;
                if (min <= key && key <= max) {
                        item = __dynamic_array_search(&array->lines[i], key);
                        if (item != NULL) {
                                goto ret;
                        } // find key;
                } // min, max check
        } // array search
ret:
        return item;
}

struct dynamic_array *dynamic_array_init()
{
        struct dynamic_array *array;
        size_t line_index;
        array = (struct dynamic_array *)malloc(sizeof(struct dynamic_array));
        if (array == NULL) {
                pr_info("Memory allocation failed\n");
                return NULL;
        }
        array->size = 0;
        dynamic_array_bitmap_init(array->bitmap);
        for (line_index = 0; line_index < NR_LINES; line_index++) {
                struct line *line = &array->lines[line_index];
                line->size = (1 << line_index);
                line->items = NULL;
        }
        return array;
}

void dynamic_array_free(struct dynamic_array *array)
{
        size_t line_index;
        for (line_index = 0; line_index < NR_LINES; line_index++) {
                dynamic_array_line_dealloc(array, line_index);
        } // end of for
        free(array);
}