#include <stdlib.h>
#include <stdio.h>
#include "scheduler.h"
#include "shell.h"


// a qucik function to call in scheduling policies.
int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}


// a fucntion that takes a pointer to another function and a queue pointer.
void scheduler(Queue * q, fptr f) {
    if (!f) {
        fprintf(stderr, "scheduler: null function pointer\n");
        return;
    }
    // call function based with queue
    f(q);
}

// to create unique PID for each PBC
int uniq_pid = 1;

int next_pid() {
    return uniq_pid++;
}

// Name is not very much doing what it says but it's job is to create PCB and place them in the queue
void launch_process(int start, int length, Queue * q) {
    PCB *p = create_pcb(next_pid(), start, length);
    enqueue(q, p);
}

// FCFS scheduler one queue, execute one by one until finished.
void fcfs_scheduler(Queue * q) {
    if (queue_is_empty(q)) {
        printf("all job is done, the scheduler has an empty queue");
    }
    // Special case if running background mode. 
    if (GLOBAL_FLAG_batch_first_slice) {
        // run helper function to execute 1 time slice from global PCB.
        run_batch_first_slice(q, 1);
    }
    // dequeue and execute until queue is empty
    while (!queue_is_empty(q)) {
        PCB *p = dequeue(q);
        for (int i = p->start + p->pc; i < (p->start + p->length); i++) {
            parseInput(script_mem->lines[i]);
            p->pc++;
        }
        // free memory.
        free(p);
    }
}

// sjf scheduler sorts processes by job length before execution
void sjf_scheduler(Queue * q) {
    if (queue_is_empty(q)) {
        printf("all job is done, the scheduler has an empty queue");
        return;
    }
    // special case if running background mode
    if (GLOBAL_FLAG_batch_first_slice) {
        run_batch_first_slice(q, 2);
    }

    /* debug
     * PCB *debug_pcb = q->head;
     printf("DEBUG SJF: Jobs before sorting:\n");
     while (debug_pcb != NULL) {
     printf("  PID %d: length=%d\n", debug_pcb->pid, debug_pcb->length);
     debug_pcb = debug_pcb->next;
     }*/


    // sort queue by length (shortest first)
    // using simple selection sort
    PCB *sorted_head = NULL;
    PCB *sorted_tail = NULL;

    while (!queue_is_empty(q)) {
        PCB *shortest = NULL;
        PCB *prev_shortest = NULL;
        PCB *current = q->head;
        PCB *prev = NULL;

        // find shortest job in remaining queue
        while (current != NULL) {
            if (shortest == NULL || current->length < shortest->length) {
                shortest = current;
                prev_shortest = prev;
            }
            prev = current;
            current = current->next;
        }

        // remove shortest from original queue
        // update head,
        if (prev_shortest == NULL) {
            q->head = shortest->next;
        } else {
            prev_shortest->next = shortest->next;
        }
        if (q->tail == shortest) {
            q->tail = prev_shortest;
        }
        shortest->next = NULL;

        // add to sorted queue
        if (sorted_head == NULL) {
            sorted_head = shortest;
            sorted_tail = shortest;
        } else {
            sorted_tail->next = shortest;
            sorted_tail = shortest;
        }
    }

    // restore sorted queue to original queue structure
    q->head = sorted_head;
    q->tail = sorted_tail;

    // execute all processes in order
    while (!queue_is_empty(q)) {
        PCB *p = dequeue(q);
        for (int i = p->start + p->pc; i < (p->start + p->length); i++) {
            parseInput(script_mem->lines[i]);
            p->pc++;
        }
        free(p);
    }
}

// rr scheduler runs each process for 2 instructions before switching
void rr_scheduler(Queue * q) {
    if (queue_is_empty(q)) {
        printf("all job is done, the scheduler has an empty queue");
        return;
    }
    int time_slice = 2;
    if (GLOBAL_FLAG_batch_first_slice) {
        run_batch_first_slice(q, time_slice);
    }



    while (!queue_is_empty(q)) {
        PCB *p = dequeue(q);

        // run for time_slice instructions or until process finishes
        int instructions_executed = 0;
        while (instructions_executed < time_slice && p->pc < p->length) {
            parseInput(script_mem->lines[p->start + p->pc]);
            p->pc++;
            instructions_executed++;
        }

        // if process not finished, re-enqueue it
        if (p->pc < p->length) {
            enqueue(q, p);
        } else {
            // process finished
            free(p);
        }
    }
}


void aging_scheduler(Queue * q) {

    // instructions executed at a time;
    int time = 1;
    // new queue to sort PCBs using insert sort.
    Queue *new_q = create_queue();
    // for copying sorted queue to the old one.
    PCB *n, *current;



    if (GLOBAL_FLAG_batch_first_slice) {
        run_batch_first_slice(q, time);
    }
    // sort the queue
    while (!queue_is_empty(q)) {
        n = dequeue(q);
        n->next = NULL;
        creation_enqueue_SJF(new_q, n);
    }

    // adjust pointers so that old queue is sorted.
    q->head = new_q->head;
    q->tail = new_q->tail;
    free(new_q);



    // check if the queue is null, if so just exit.
    if (queue_is_empty(q)) {
        printf("All jobs done");
        return;
    }
    // execute instruction 1 time slot at a time as long as there is something to be done.
    while (1) {

        // exit if all finished.
        if (queue_is_empty(q)) {
            break;
        }
        // take the first one
        PCB *p = dequeue(q);

        // if PCB instuctions are finished
        if (p->pc == p->length) {
            // free allocated memory and go to next PCB
            free(p);
            //      printf("P%d is done, \n",p->pid);
            // continue;
        }
        // send one line to the parser which calls interpreter.c
        char *line = script_mem->lines[(p->start + p->pc)];

        parseInput(line);
        // pass the line and reduce job_score_length for waiting PCBs and added it to the queue based on it's score.
        p->pc++;
        current = q->head;


        //   printf("P%d, line: %s\n", p->pid, line);

        while (current != NULL) {
            // debuging   
            // printf("P%d(%d, %d)   ", current->pid, (current->length-current->pc),current->job_score_length);
            current->job_score_length = max(0, current->job_score_length - 1);
            // debuging  
            // printf("(P%d, %d)   ", current->pid, current->job_score_length);
            current = current->next;
        }

        if (p->pc == p->length) {
            // debuging
            //printf("remvoe \n");
            continue;
        }
        ///debug
        //  printf("put back \n");  
        enqueue_SJF(q, p);

    }
}


// copy paste rr scheduler runs each process for 30 instructions before switching
void rr30_scheduler(Queue * q) {
    if (queue_is_empty(q)) {
        printf("all job is done, the scheduler has an empty queue");
        return;
    }


    if (GLOBAL_FLAG_batch_first_slice) {
        run_batch_first_slice(q, 30);
    }

    int time_slice = 30;

    while (!queue_is_empty(q)) {
        PCB *p = dequeue(q);

        // run for time_slice instructions or until process finishes
        int instructions_executed = 0;
        while (instructions_executed < time_slice && p->pc < p->length) {
            parseInput(script_mem->lines[p->start + p->pc]);
            p->pc++;
            instructions_executed++;
        }

        // if process not finished, re-enqueue it
        if (p->pc < p->length) {
            enqueue(q, p);
        } else {
            // process finished
            free(p);
        }
    }
}




// helper function to execute certain amount of instructions from the first element in the queue
// used in background mode as global PCB should get time slice. Afterwards we continue execution in scheduler functions normally. 
int run_batch_first_slice(Queue * q, int time) {

    // if queue empty exit
    if (queue_is_empty(q)) {
        // printf("all done\n");
        GLOBAL_FLAG_batch_first_slice = 0;
        return 0;
    }
    // dequeue the head (in main() batch is first so no need to check)
    PCB *p = dequeue(q);

    // For given amount of instruction to be executed in PCB p. Exectue them and update PCB.
    int executed = 0;
    while (executed < time && p->pc < p->length) {

// debug
// printf("pc %d: %s", p->pc,
// script_mem->lines[p->start + p->pc]);
        parseInput(script_mem->lines[p->start + p->pc]);
        p->pc++;
        executed++;
    }
    // if amount of instruction to execute is larger than PCB instructions free memory of pcb and don't put back
    if (p->pc < p->length) {
        // if not put back from end.
        enqueue(q, p);
    } else {
        free(p);
    }
    // turn off global flag for first slice execution.
    GLOBAL_FLAG_batch_first_slice = 0;
    // return 1, success.       
    return 1;
}
