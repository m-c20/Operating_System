#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "interpreter.h"
#include  "shellmemory.h"
// declaring function pointer with alias for modularity. it always takes a queue. 
// we could've said only void (*fptr)(Queue *q) instead and called scheduler as scheduler(Queue *q, void (*fptr)(Queue *));

extern int uniq_pid;
typedef void (*fptr)(Queue * q);
void launch_process(int start, int length, Queue * q);
void scheduler(Queue * q, fptr f);
void fcfs_scheduler(Queue * q);
void sjf_scheduler(Queue * q);
void rr_scheduler(Queue * q);
void aging_scheduler(Queue * q);
void rr30_scheduler(Queue * q);
int run_batch_first_slice(Queue * q, int time);
