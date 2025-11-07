#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "scheduler.h"

// FROM NOW ON EVERYTHING INVOLVING "-" or "v" IN COMMENTS MEANS STUDENT WRITTEN CODE.

int MAX_ARGS_SIZE = 10;

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

// For source command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int source(char *script);
int badcommandFileDoesNotExist();
// -------------------------------------declaration of "exec"
int exec(char *command_args[], int args_size);
// -------------------------------------
// ------------------------------------- MC, initalizing "echo", "ls", "mkdir", "touch", "cd" commands
int echo(char *strOr$);
int my_ls();
int my_mkdir(char *dirname);
int my_touch(char *filename);
int my_cd(char *dirname);
int run(char *argc[]);
// -------------------------------------

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }
    //-----------
    // adding echo, run, my_ls, run, my_mkdir, my_touch and my_cd command to interpeter
    // and exec
    if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2) {
            return badcommand();
        }
        return echo(command_args[1]);

    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 3) {
            return badcommand();
        }
        return run(command_args);
    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) {
            return badcommand();
        }
        return my_ls();
    }

    else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2) {
            return badcommand();
        }
        return my_mkdir(command_args[1]);
    }

    else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2) {
            return badcommand();
        }
        return my_touch(command_args[1]);
    }

    else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2) {
            return badcommand();
        }
        return my_cd(command_args[1]);
    } else if (strcmp(command_args[0], "exec") == 0) {
        if (args_size < 3 || args_size > 6) {
            return badcommand();
        }
        return exec(command_args, args_size);
    }
    //-----------
    else if (strcmp(command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand();
        return set(command_args[1], command_args[2]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand();
        return source(command_args[1]);

    } else
        return badcommand();
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    mem_set_value(var, value);
    return 0;
}


int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}


// v            v               v               v               v 
// Modified source such that it calls a 
int source(char *script) {
    // CHECK IF FILE EXISTS 
    FILE *test_fp = fopen(script, "r");
    if (test_fp == NULL) {
        return badcommandFileDoesNotExist();
    }
    fclose(test_fp);
    // pointers to temporarily store start and length of loaded memory of scripts
    int start, length;
    // queue for fcfs scheduler.
    Queue *fcfs_queue = create_queue();
    // loading given file, load_script will loaded into global struct and will update start and length
    load_script(script, &start, &length);
    // Check if load_script actually set valid values
    if (start < 0 || length <= 0) {
        destroy_queue(fcfs_queue);
        return badcommandFileDoesNotExist();
    }
    //launch_process will create PCB and will add it to fcfs_queue.
    launch_process(start, length, fcfs_queue);
    // scheduler will dequeue it and run it. 
    scheduler(fcfs_queue, fcfs_scheduler);
    // freeing memory.
    destroy_queue(fcfs_queue);

    // I don't know why 0, maybe I should check for errors. Anyway.
    return 0;
}

//-     v   -----    v                v       -----         v            ----            v         ---       v             ----         v 

// run command takes input **command_args, a pointer to pointers of type char. 
int run(char *command_args[]) {

    // creating pid to track special numbers for child and parent procceses. 
    pid_t p;
    // forking, duplicating procces.
    p = fork();
    // Creating value status so that we can tell parent what happened how child terminated, optional.
    int status;

    // child enters this branch
    if (p == 0) {
        // special exec so that we can use rest of arguments
        execvp(command_args[1], &command_args[1]);
        // if execvp is success it won't return otherwise perror and exit 1.
        perror("execvp");
        exit(1);
    }
    // paret goes to this branch if fork is success.
    if (p > 0) {
        if (waitpid(p, &status, 0) == -1) {
            // Same thing we did for exec 
            perror("waitpid");
        }
        // If we want to see what failed child returend we can use WIFEXITED() and WEXITSTATUS()
    }
    // if fork fails
    if (p < 0) {
        perror("fork");
        //exit (1); would be an overkill.
    }
    // function successful.
    return 0;
}




// echo takes char pointer and returns int.
int echo(char *strOr$) {

    char *p;
    p = "$";

    // compare first char of input, using ascii compare, 
    if (strOr$[0] == '$') {
        // if we enter this branch it means it's a value.
        int length = strlen(strOr$);
        char copy[length];
        // copy string 
        for (int i = 0; i < length; i++) {
            copy[i] = strOr$[i + 1];
        }
        // call print
        return print(copy);

    }
    printf("%s\n", strOr$);
    return 0;
}




//  sorted alphabetically and listed files in current directory
int my_ls() {
    DIR *dir;
    struct dirent *entry;
    char *names[1000];          //pointers to filenames
    int count = 0;              //entries found counter

    dir = opendir(".");
    if (dir == NULL)
        return 1;

    // have to read all directory entries and allocate memory 
    // for filname with malloc and copu file from dir to array
    while ((entry = readdir(dir)) != NULL) {
        names[count] = malloc(strlen(entry->d_name) + 1);
        strcpy(names[count], entry->d_name);
        count++;
    }
    closedir(dir);

    // sort for alphabetical order
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (strcmp(names[j], names[j + 1]) > 0) {
                // Swap pointers if out of order    
                char *temp = names[j];
                names[j] = names[j + 1];
                names[j + 1] = temp;
            }
        }
    }

    // Print and free memory
    for (int i = 0; i < count; i++) {
        printf("%s\n", names[i]);
        free(names[i]);
    }
    return 0;
}

int my_mkdir(char *dirname) {
    char actual_name[200];

    // check it's a variable
    if (dirname[0] == '$') {
        char *value = mem_get_value(dirname + 1);

        if (value == NULL || strlen(value) == 0) {
            printf("Bad command: my_mkdir\n");
            return 1;
        }
        // it's only letters and numbers (alphanumeric)
        int i = 0;
        while (value[i] != '\0') {
            char c = value[i];
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
                  || (c >= 'a' && c <= 'z'))) {
                printf("Bad command: my_mkdir\n");
                return 1;
            }
            i++;
        }

        strcpy(actual_name, value);
    } else {
        // just use the name directly
        strcpy(actual_name, dirname);
    }
    // create directory permissions
    mkdir(actual_name, 0777);
    return 0;
}

int my_touch(char *fname) {
    // open creates file if doesn't exist
    int fd = open(fname, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        printf("Bad command: my_touch\n");
        return 1;
    }
    close(fd);
    return 0;
}

// Change directory
int my_cd(char *dirname) {
    // attempt to change directory      
    if (chdir(dirname) != 0) {
        printf("Bad command: my_cd\n");
        return 1;
    }
    return 0;
}

// exec command takes input **command_args, a pointer to pointers of type char, and args_size for number of arguments.
int exec(char *command_args[], int args_size) {


    int num_programs;
    char *policy;
    // if backgorund mode then last argument is pound, the one before policy and the rest after exec is programs to be executed.
    if (strcmp(command_args[args_size - 1], "#") == 0) {
        policy = command_args[args_size - 2];
        num_programs = args_size - 3;
        // turn on background mode
        GLOBAL_FLAG_background_mode = 1;

    } else {
        // policy is always last argument in command_args unless in backgorund case handled above
        policy = command_args[args_size - 1];
        // number of programs to execute, excluding "exec" command itself and policy
        num_programs = args_size - 2;
    }

    // checking if policy is valid,
    if (strcmp(policy, "FCFS") != 0 && strcmp(policy, "SJF") != 0
        && strcmp(policy, "RR") != 0 && strcmp(policy, "AGING")
        && strcmp(policy, "RR30")) {
        // terminate with error if policy not defined
        printf("Unknown policy: %s\n", policy);
        return 1;
    }
    // checking for duplicate filenames in arguments
    for (int i = 1; i <= num_programs; i++) {
        for (int j = i + 1; j <= num_programs; j++) {
            if (strcmp(command_args[i], command_args[j]) == 0) {
                printf("Error: Duplicate script files\n");
                return 1;
            }
        }
    }

    // arrays to store start positions and lengths of loaded programs
    int starts[3], lengths[3];
    // loading all programs into shell memory
    for (int i = 0; i < num_programs; i++) {
        load_script(command_args[i + 1], &starts[i], &lengths[i]);

        // if load fails length will be 0
        if (lengths[i] == 0) {
            printf("Error loading script: %s\n", command_args[i + 1]);
            return 1;
        }
    }

    // if PCB have been loaded and the exec call is after the exec call with # then use the queue uploaded to global struct
    // because otherwise exec will create a new queue and we don't want that, that would end up excluding the exec # call. 
    if (background != NULL && strcmp(command_args[args_size - 1], "#") != 0) {
        for (int i = 0; i < num_programs; i++) {
            launch_process(starts[i], lengths[i], background->q);
        }
        return 0;
    }
    // creating queue for launching
    Queue *exec_queue = create_queue();

    // launching all processes and adding PCBs to queue
    for (int i = 0; i < num_programs; i++) {
        launch_process(starts[i], lengths[i], exec_queue);
    }

    // selecting scheduler based on policy
    fptr scheduler_func;
    if (strcmp(policy, "FCFS") == 0) {
        scheduler_func = fcfs_scheduler;
    } else if (strcmp(policy, "SJF") == 0) {
        scheduler_func = sjf_scheduler;
    } else if (strcmp(policy, "RR") == 0) {
        scheduler_func = rr_scheduler;
    } else if (strcmp(policy, "AGING") == 0) {
        scheduler_func = aging_scheduler;
    } else if (strcmp(policy, "RR30") == 0) {
        scheduler_func = rr30_scheduler;
    }
    if (GLOBAL_FLAG_background_mode) {
        //printf("updating struct and exiting exec without launching\n");
        background = malloc(sizeof(struct info_holder));
        background->q = exec_queue;
        //printf("queue update successful\n");       
        background->scheduler_func = scheduler_func;
        //printf("struct update successful\n");
        return 0;
    }
    // launch
    scheduler(exec_queue, scheduler_func);

    // freeing memory
    destroy_queue(exec_queue);

    return 0;
}
