#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"


PCB *create_pcb(int pid, int start, int length) {
    struct PCB *p = malloc(sizeof(PCB));
    if (p == NULL) {
        fprintf(stderr, "allocation for PCB failed\n");
        exit(1);
    }
    p->pid = pid;
    p->start = start;
    p->length = length;
    p->pc = 0;
    p->next = NULL;
    p->job_score_length = length;
    return p;
}

void free_pcb(PCB * p) {
    free(p);
}
