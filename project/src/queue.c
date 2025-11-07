//#include "pcb.h"
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// dequeue function pops an element from queue given to it.
PCB *dequeue(Queue * q) {
    // error check
    if (q->head == NULL) {
        fprintf(stderr, "cannot dequeue empty queue");
        return NULL;
    }
    // operation to update pointers, head is poped.
    PCB *p = q->head;
    // head.next is new head, using pcb pointer.
    q->head = q->head->next;
    // if this was last element then update tail.
    if (q->head == NULL) {
        q->tail = NULL;
    }
    // the one poped ? it is not a list just one pcb, so we update pointer.
    p->next = NULL;
    //return poped.
    return p;
}

// enqueue takes 2 elements, a queue to add to and a PCB to be added.
void enqueue(Queue * q, PCB * p) {
    // base case where queue is empty
    if (q->head == NULL) {
        // the head is the given element
        q->head = p;
        // there is only one element so the head is the tail too.
        q->tail = p;
        // terminate
        return;
    }
    // if queue is not empty then tail is extended with PCB p
    q->tail->next = p;
    // the new tail is PCB p.
    q->tail = p;

}


// creates queue and returns it
Queue *create_queue() {
    // allocate a new queue    
    Queue *q = malloc(sizeof(Queue));
    // if fails crash
    if (q == NULL) {
        fprintf(stderr, "allocate memory for queue\n");
        exit(1);
    }
    // pointers initialized
    q->head = NULL;
    q->tail = NULL;
    // return new queue
    return q;
}

// free the alocted queue from memory
void destroy_queue(Queue * q) {
    // for every element in the queue, pop it and free using free_pcb.
    while (q->head != NULL) {
        PCB *p = dequeue(q);
        free_pcb(p);
    }
    // free space alocted for the queue
    free(q);
}

// boolean function to check if queue is empty
int queue_is_empty(Queue * q) {
    return q->head == NULL;
}

// simple way of getting first element of queue
PCB *peek(Queue * q) {
    return q->head;
}

void enqueue_SJF(Queue * q, PCB * p) {

    if (q->head == NULL) {
        enqueue(q, p);
        return;
    }
    PCB *current, *previous;
    current = q->head;
    previous = NULL;

    while (1) {
        if ((previous == NULL)
            && (p->job_score_length == current->job_score_length)) {
            q->head = p;
            p->next = current;
            break;

        } else if ((previous == NULL)
                   && (p->job_score_length < current->job_score_length)) {
            PCB *n = current;
            q->head = p;
            p->next = current;
            break;

        } else if (p->job_score_length < current->job_score_length) {
            previous->next = p;
            p->next = current;
            break;

        } else if (p->job_score_length == current->job_score_length) {
            PCB *n = current->next;
            current->next = p;
            p->next = n;
            if (current == q->tail)
                q->tail = p;
            break;




        } else if (current->next == NULL) {
            current->next = p;
            q->tail = p;
            p->next = NULL;
            break;
        }
        previous = current;
        current = current->next;
    }


}

void creation_enqueue_SJF(Queue * q, PCB * p) {


    if (q->head == NULL) {
        enqueue(q, p);
        p->next = NULL;
        return;
    }

    PCB *prev = NULL, *cur = q->head;
    // Pass over <= to place ties AFTER existing equals (FCFS)
    while (cur && cur->job_score_length <= p->job_score_length) {
        prev = cur;
        cur = cur->next;
    }
    if (prev == NULL) {         // insert at head
        p->next = q->head;
        q->head = p;
    } else {                    // middle or append
        prev->next = p;
        p->next = cur;
        if (cur == NULL)
            q->tail = p;        // appended → update tail
    }
}
