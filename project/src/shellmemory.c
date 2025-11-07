#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"


// if you see a * comment it means student writen code. The rest is untouched. 
// *>

struct script_memory_manager *script_mem;
// constructor for structs that hold memory
void init_scrpt_memory() {
    // debug statement
    // printf("init \n");
    // memory allocation and checks for succcess
    script_mem = malloc(sizeof(struct script_memory_manager));
    if (script_mem == NULL) {
        fprintf(stderr, "allocation memory for script_mem failed.\n");
        exit(1);
    }
    // memory allocation again.
    script_mem->lines = malloc(sizeof(char *) * 1000);
    // initializing struct branchs
    script_mem->count = 0;
    script_mem->capacity = 1000;

}

// <*


struct memory_struct {
    char *var;
    char *value;
};

struct memory_struct shellmemory[MEM_SIZE];

// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i])
            matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else
        return 0;
}

// Shell memory functions

void mem_init() {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        shellmemory[i].var = "none";
        shellmemory[i].value = "none";
    }
    // *
    init_scrpt_memory();
    // *
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, "none") == 0) {
            shellmemory[i].var = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        }
    }

    return;
}

//get value based on input key
char *mem_get_value(char *var_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp(shellmemory[i].var, var_in) == 0) {
            return strdup(shellmemory[i].value);
        }
    }
    return "Variable does not exist";
}

//      *                      *                         *            *                *              *>



// resizing function for extending capacity of the struct that is loading script.
// will not explain this because it's just resizing.
void resize(struct script_memory_manager *s) {
    printf("resize() \n");
    int new_cap = s->capacity * 2;
    char **new_s = realloc(s->lines, sizeof(char *) * new_cap);
    if (new_s == NULL) {
        fprintf(stderr,
                "memory allocation failed when increasing capacity of data structure that records the script");
        exit(1);
    }
    s->lines = new_s;
    s->capacity = new_cap;

}


void load_script(const char *script, int *start, int *length) {
    // allocating memory for global struct to avoid segmentation faults.
    // removed init_scrpt_memory();
    //debug statement       
    // printf("opening file \n");

    // opening file
    FILE *fp = fopen(script, "r");

    // checking file. 
    if (fp == NULL) {
        //   printf
        //     ("something wrong happened when calling void, in terms of given file to it");
        return;
    }
    // debug statement
    //printf("inside load \n");

    // updating start pointer to have value of number of lines in the script.   
    *start = script_mem->count;
    // initializing getline inputs to store lines wihtout memory limitations 
    char *line = NULL;
    size_t len = 0;
    // first get line call to start loop
    ssize_t read = getline(&line, &len, fp);

    // while loop 
    while (read != -1) {

        // if capacity is not enough resize
        if (script_mem->count >= script_mem->capacity) {
            resize(script_mem);
        }
        // copy line using strdup to struct because we're using pointers
        script_mem->lines[script_mem->count] = strdup(line);
        // increasing count
        script_mem->count++;
        // reading file and updating variables for the while loop and lines to be stored
        read = getline(&line, &len, fp);
    }
    // debuging loop
    // for (int i = 0; i < script_mem->count; i++) {
    //     printf("Line %d: %s", i, script_mem->lines[i]);
    // }
    // update length so that we can store it in pcb
    *length = script_mem->count - *start;
    free(line);
    fclose(fp);
}

// load_line_background takes 3 inputs. The start, length of the GLOBAL pcb and the line that should be stored in teh corresponding memory address (the global struct)
void load_line_background(char *line, int *start, int *length) {
    //  printf("loading %s\n", line);

    // updating start of PCB to address in script_mem global struct. 
    if (*length == 0) {
        *start = script_mem->count;
    }
    // memory allocation for global struct.
    if (script_mem->count >= script_mem->capacity) {
        resize(script_mem);
    }
    // copy line using strdup to struct because we're using pointers
    script_mem->lines[script_mem->count] = strdup(line);

    // increasing count
    script_mem->count++;
    // updating length in PCB. 
    (*length)++;
}




//              < *                  *                *                   *                *          *
