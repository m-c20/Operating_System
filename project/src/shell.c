#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
//everything added btw the lines ------ is student code 
//--------------

// initializing PCB to store all lines after exec . . . # command
PCB *batch_script_process = NULL;

// initializing background mode global flag to false. This flag is later used in exec to stop parsing input. 
int GLOBAL_FLAG_background_mode = 0;

// this is your code
int multiple_commands(char user_input[]);

// initializing global struct pointer so that exec can send queue back to main.
struct info_holder *background = NULL;

// initializing first slice global flag, the scheduler functions will now have to check whether background mode is on and execute 1 time slot as written in instructions. 
int GLOBAL_FLAG_batch_first_slice = 0;


int multiple_commands(char user_input[]) {
    char single_command[MAX_USER_INPUT];
    int result_code = 0;
    char *current_cmd;
    char *input_buffer;

    // copy input, don't modify it
    input_buffer = (char *) malloc(strlen(user_input) + 1);
    strcpy(input_buffer, user_input);

    current_cmd = strtok(input_buffer, ";");

    while (current_cmd != NULL) {
        // skip whitespace
        while (*current_cmd == ' ' || *current_cmd == '\t')
            current_cmd++;

        strcpy(single_command, current_cmd);

        // add newline if needed
        if (strlen(single_command) > 0) {
            if (single_command[strlen(single_command) - 1] != '\n') {
                strcat(single_command, "\n");
            }
        }
        // process the command
        if (strlen(single_command) > 1) {
            result_code = parseInput(single_command);
            if (result_code == -1)
                break;          // quit command
        }

        current_cmd = strtok(NULL, ";");
    }

    free(input_buffer);
    return result_code;
}

//-----------------

int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
    printf("Shell version 1.4 created December 2024\n");
    fflush(stdout);

    GLOBAL_FLAG_background_mode = 0;


    char prompt = '$';          // Shell prompt
    char userInput[MAX_USER_INPUT + 1]; // user's input stored here
    int errorCode = 0;          // zero means no error, default
    userInput[MAX_USER_INPUT + 1] = '\0';

    //init user input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }
    batch_script_process = NULL;
    //init shell memory
    mem_init();
    // creating a new var to exit loop if it's reading files.
    while (1) {
        // isatty Returns 1 if the file descriptor refers to a terminal device. Returns 0 if the file descriptor does not refer to a terminal device. 
        // We'll use it to decide whether we should print $.
        if (isatty(STDIN_FILENO)) {
            printf("%c ", prompt);
        }
        // Check for EOF - this will fix the hanging
        if (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL) {
            break;              // Exit cleanly when no more input
        }

        if (strchr(userInput, ';') != NULL) {
            errorCode = multiple_commands(userInput);
        } else {
            // checking whether execution in the background mode is on
            if (GLOBAL_FLAG_background_mode) {

                // initializing global PCB to avoid segmentation issues.
                if (batch_script_process == NULL) {
                    batch_script_process = malloc(sizeof(struct PCB));
                    batch_script_process->pid = uniq_pid++;
                    batch_script_process->pc = 0;
                    batch_script_process->length = 0;
                }

                batch_script_process->pc = 0;

                //  debuging, there were also loops but deleted them to make easier for eyes to read printf("loading line by line in main\n");

                // calling line loader for background mode so that PCB is loaded. 
                load_line_background(userInput, &batch_script_process->start,
                                     &batch_script_process->length);

            } else {
                errorCode = parseInput(userInput);
            }

            if (errorCode == -1)
                exit(99);
            memset(userInput, 0, sizeof(userInput));
        }
    }
    // After reaching EOF checking whether it was background mode
    if (GLOBAL_FLAG_background_mode) {

        // updating head of the queue given by exec 
        PCB *h = background->q->head;
        background->q->head = batch_script_process;

        // new head is global PCB, reconneting linked list
        batch_script_process->next = h;

        // turn off background mode.
        GLOBAL_FLAG_background_mode = 0;
        // turn on first slice before scheduler call
        GLOBAL_FLAG_batch_first_slice = 1;

        // launch depending on policy.
        scheduler(background->q, background->scheduler_func);

        // destroy queue and free memory       
        destroy_queue(background->q);
        free(background);
        background = NULL;
        batch_script_process = NULL;

    }
    // success
    return 0;
}

int wordEnding(char c) {
    // You may want to add ';' to this at some point,
    // or you may want to find a different way to implement chains.
    return c == '\0' || c == '\n' || c == ' ';
}

int parseInput(char inp[]) {
    char tmp[200], *words[100];
    int ix = 0, w = 0;
    int wordlen;
    int errorCode;
    for (ix = 0; inp[ix] == ' ' && ix < 1000; ix++);    // skip white spaces
    while (inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000) {
        // extract a word
        for (wordlen = 0; !wordEnding(inp[ix]) && ix < 1000; ix++, wordlen++) {
            tmp[wordlen] = inp[ix];
        }
        tmp[wordlen] = '\0';
        words[w] = strdup(tmp);
        w++;
        if (inp[ix] == '\0')
            break;
        ix++;
    }
    // To prevent execvp from returning bad address ---vvvvvvvvvvvvvvv----
    words[w] = NULL;
    //---^^^^^^^^^^^----
    errorCode = interpreter(words, w);
    return errorCode;

}
