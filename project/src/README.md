Mert Cezar 261166600
Roman Humenyuk 261240585
	
	I would like to clarify some things for our TA. We prioritized modularity in our knowledge to the best extent. Some lines of code could be simplified. 
	There are test files to test certain commands in the directory. You can run them depending on their type. Some in batch mode some in interactvie mode.
	A lot of the debuging code was deleted for simplicity and your eyes. Some updates were done even though it wasn't asked by the instructions specifiaclly.
	All code was written by Mert and Roman. Copilot (university version) was used in debuging part 2 of 1.2.1 & 1.2.5
	
	In total 6 additional main files were added. pcb.h pcb.c scheduler.c scheduler.h queue.c queue.h the .h files enables the .c files to be shared among each other and have some initializations
	pcb.c manages PCBs and their operations.
	scheduler.c manages scheduling policies and background mode execution.
	queue.c creates and impelents the queues 
	Some adjustments were made in interpreter.c, .h and shell.c, .h 
	Created some test.sh files for specific tests
