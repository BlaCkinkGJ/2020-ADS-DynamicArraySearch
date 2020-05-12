#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic-array.h"

int main(void)
{
        int ret = 0;
        size_t nr_case = 0, i = 0;
        struct dynamic_array *array = NULL;

        FILE *fp = fopen("test.inp", "r");

        array = dynamic_array_init();
        if (array == NULL) {
                goto exception;
        }

        pr_info("dynamic array initialize finished\n");

        fscanf(fp, "%I64d", &nr_case);

        for (i = 0; i < nr_case; i++) {
                char command[256];
                key_t key;
                fscanf(fp, "%s %I64d", command, &key);
                if (!strncmp(command, "INSERT", sizeof(command))) {
                        struct item item = { .key = i };
                        ret = dynamic_array_insert(array, item);
                        if (ret) {
                                goto exception;
                        }
                }
                if (!strncmp(command, "SEARCH", sizeof(command))) {
                        dynamic_array_search(array, i);
                }
        }
        pr_info("insert/serach sequence finished\n");

        dynamic_array_free(array);
        pr_info("dynamic array free\n");

exception:
        fclose(fp);
        return ret;
}