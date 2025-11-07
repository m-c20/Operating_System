#include <stdio.h>
#include "shellmemory.h"

int main() {
    mem_init();
    printf("script_mem: %p\n", (void*)script_mem);
    printf("script_mem->lines: %p\n", (void*)script_mem->lines);
    printf("script_mem->count: %d\n", script_mem->count);
    printf("script_mem->capacity: %d\n", script_mem->capacity);
    return 0;
}
