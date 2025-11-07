int interpreter(char *command_args[], int args_size);
int help();
//adding parser into .h file so that we can call it in scheduler.c
int parseInput(char ui[]);
