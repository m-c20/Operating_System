#ifndef PCB_H
#   define PCB_H
#   include <stdlib.h>
#   include <stdio.h>




typedef struct PCB {
    int pid;                    // process ID
    int start;                  // start index in shared shell memory of the pcb
    int length;                 // number of lines in the script
    int pc;                     // program counter or index i 
    int job_score_length;
    struct PCB *next;           // the next PCB in the ready queue
} PCB;

PCB *create_pcb(int pid, int s, int l);
void free_pcb(PCB * p);

#endif
