
#ifndef SHELLMEMORY_H
#   define SHELLMEMORY_H

#   define MEM_SIZE 1000
void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
// declearing memory loader function in teh header file so that interpeter.c can use it from shellmemory.c

struct script_memory_manager {
    char **lines;
    int count;
    int capacity;
};

extern struct script_memory_manager *script_mem;

void load_script(const char *filename, int *a, int *b);
void load_line_background(char *line, int *start, int *length);
#endif
