CC=gcc
CFLAGS=
FMT=indent

mysh: shell.c interpreter.c shellmemory.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c pcb.c scheduler.c queue.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o queue.o pcb.o scheduler.o

style: shell.c shell.h interpreter.c interpreter.h shellmemory.c shellmemory.h pcb.h pcb.c queue.h queue.c scheduler.h scheduler.c
	$(FMT) $?

clean: 
	$(RM) mysh; $(RM) *.o; $(RM) *~

