#define MAX_USER_INPUT 1000

// Some global flags and structs to save schedule queues and define execution modes. 

#include "scheduler.h"

struct info_holder {
    Queue *q;
    fptr scheduler_func;
};
extern struct info_holder *background;
extern int GLOBAL_FLAG_batch_first_slice;
extern int GLOBAL_FLAG_background_mode;
int parseInput(char inp[]);
