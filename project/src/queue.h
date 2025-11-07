#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"
#ifndef QUEUE_H
#   define QUEUE_H

typedef struct Queue {
    PCB *head;
    PCB *tail;
} Queue;
extern Queue *GLOBAL_QUEUE;

PCB *dequeue();
int queue_is_empty();
PCB *peek();
void enqueue(Queue * q, PCB * p);

void destroy_queue(Queue * q);
Queue *create_queue();
void enqueue_SJF(Queue * q, PCB * p);
void creation_enqueue_SJF(Queue * q, PCB * p);
#endif
