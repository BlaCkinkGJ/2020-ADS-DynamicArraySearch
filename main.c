#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic-array.h"

int main(void)
{
        int ret = 0;
        size_t nr_case = 0, i = 0;
        struct dynamic_array *array = NULL;
        size_t err = 0, total = 0, array_size = 0;

        FILE *fp = fopen("test.inp", "r");

        fscanf(fp, SIZE_T_FORMAT, &nr_case);

        array_size = dynamic_array_get_size(nr_case);
        array = dynamic_array_init(array_size);
        if (array == NULL) {
                goto exception;
        }

        pr_info("dynamic array initialize finished\n");

        for (i = 0; i < nr_case; i++) {
                char command[256];
                key_t key;
                fscanf(fp, "%s " KEY_FORMAT, command, &key);
                if (!strncmp(command, "INSERT", sizeof(command))) {
                        struct item item = { .key = key };
                        ret = dynamic_array_insert(array, item);
                        if (ret) {
                                goto exception;
                        }
                }
                if (!strncmp(command, "SEARCH", sizeof(command))) {
                        struct item *item = dynamic_array_search(array, key);
                        if (item == NULL) {
                                err++;
                        }
                        total++;
                }
        }
        pr_info("insert/serach sequence finished (err: %.2f%%)\n",
                (float)(err * 100.0 / total));

        dynamic_array_free(&array);
        pr_info("dynamic array free\n");

exception:
        fclose(fp);
        return ret;
}