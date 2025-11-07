#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
//#include "pcb.h"

int main() {
	printf("start main \n");
	Queue *p = create_queue();
	printf("start creating PCB's\n");
    PCB *p1 = create_pcb(1, 0, 5);
    PCB *p2 = create_pcb(2, 5, 10);
    PCB *p3 = create_pcb(3, 15, 8);
	printf("enqueue now\n");
    enqueue(p,p1);
    enqueue(p,p2);
    enqueue(p,p3);

    printf("Peek PID: %d\n", peek()->pid); // Should be 1

    while (!queue_is_empty(p)) {
        printf("about to dequeue \n");
	PCB *a = dequeue(p);
        printf("Dequeued PID: %d\n", a->pid);
        free_pcb(a);
    }

    if (peek() == NULL) {
        printf("Queue is empty after all dequeues.\n");
    }

    return 0;
}
