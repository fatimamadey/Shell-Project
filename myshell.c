#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
/**
 * Prints the given message to standard output.
 */
void myPrint(char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

/**
 * Prints a predefined error message to standard output.
 */
void print_error(void){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
}

/**
 * Trims leading and trailing white spaces from the given string.
 */
char *trim(char *str) {
    // Trim leading white spaces
    char *start = str;
    while (*start && isspace(*start)) {
        start++;
    }
    // Shift the trimmed string to the beginning
    char *end = start + strlen(start) - 1;
    while(isspace(*end)) {
        end--;
    }
    *(end + 1) = '\0'; // Null-terminate the trimmed string
    return start;
}

/**
 * Checks if the given string is empty (contains only white spaces).
 */
int is_empty(char *str){
    int empty = 0;
    char *start = str;
    while(*start){
        if (!isspace(*start)){
            return empty;
        }
        start++;
    }
    empty = 1;
    return empty;
}

/**
 * Parses the input command line into individual commands.
 */
char** parsing(char* input, int* num_substrings) {
    char* line = strdup(input);
    char split = ';';
    int len = strlen(line);

    // Count the number of semicolons in the input string
    int max_substrings = 1;
    for (int i = 0; i < len; i++) {
        if (line[i] == split) {
            max_substrings++;
        }
    }

    char** substrings = malloc(max_substrings * sizeof(char *)); // Array to store pointers to substrings

    int count = 0;
    char* line_copy = strdup(line);
    char* saveptr_line;
    // Tokenize the string using strtok to get substrings
    char* token = strtok_r(line_copy, ";", &saveptr_line);
    while (token != NULL && count < max_substrings) {
        if(is_empty(token)){ // this will cause array to only be filled with cmds that aren't blank
            token = strtok_r(NULL, ";", &saveptr_line);
            continue;
        }
        if (token != NULL) {
            char* trimmed = trim(token);
            substrings[count++] = trimmed;
        }
        token = strtok_r(NULL, ";", &saveptr_line);
    }
    *num_substrings = count;
    return substrings;
}

/**
 * Changes the current working directory to the specified path.
 */
int cd(char** args) {
    int valid = 1;
    char *home_dir = getenv("HOME"); // Get the home directory path
    
    // If no directory argument is provided, change to the home directory
    if(args[2] != NULL){
        valid = 0;
    }
    if (args[1] == NULL) {
        if (chdir(home_dir) < 0) {
            valid = 0;
        }
    } else {
        // Attempt to change to the specified directory
        if (chdir(args[1]) < 0) {
            valid = 0;
        }
    }
    return valid;
}

/**
 * Prints the current working directory to standard output.
 */
int pwd(char** args) {
    char cwd[1024];
    int valid = 1;
    if (args[1] != NULL) {
        valid = 0;
    } else if (getcwd(cwd, sizeof(cwd)) != NULL) {
        myPrint(cwd); //prints current working directory
        myPrint("\n");
    } else {
        valid = 0;
    }
    return valid;
}

/**
 * Exits the shell process.
 */
void my_exit(void) {
    exit(0);
}


/**
 * Executes a command given its arguments.
 * cmd: The command to be executed.
 * redirect: Flag indicating whether redirection is enabled.
 * returns 1 if the command execution is successful, otherwise 0.
 */
int executing_command(char* cmd, int redirect) {
    int max_args = 1000;
    int count = 0;
    
    char** args = malloc(max_args * sizeof(char*)); 

    char* cmd_copy = strdup(cmd);
    char* saveptr_cmd;
    
    // filling up the args with argv type array of strings for use
    char* token = strtok_r(cmd_copy, " \t", &saveptr_cmd);
    while (token != NULL) {
        args[count++] = token;
        token = strtok_r(NULL, " \t", &saveptr_cmd);
    }
    args[count] = NULL;

    // Send commands and arguments to command executables
    int valid = 1;
    if (!strcmp(args[0], "cd") && !redirect) {
        valid = cd(args);
    }
    else if (!strcmp(args[0], "pwd") && !redirect) {
        valid = pwd(args);
    }
    else if (!strcmp(args[0], "exit") && !redirect) {
        my_exit();
    }
    else {
        // otherwise, do execvp()
        pid_t pid = fork();
        if (pid < 0) {
            valid = 0;
        } else if (pid == 0) {
            // Child process
            // Execute the command
            if (execvp(args[0], args) < 0) {
                print_error();
                exit(0);
            }
        } else {
            // Parent process
            wait(NULL);
        }
    }
    return valid;
}

/**
 * Executes a command with output redirection.
 * cmd: The command with redirection to be executed.
 * returns 1 if the command execution is successful, otherwise 0.
 */
int execute_redirect_command(char* cmd) {
    // Checks validity of string
    int char_before = 0;
    int char_after = 0;
    int char_reached = 0;
    int char_counter = 0;
    for(int j = 0; cmd[j] != '\0'; j++){
        if(cmd[j] == '>'){
            char_reached = 1;
            char_counter += 1;
        }
        if(!isspace(cmd[j]) && cmd[j] != '>' && !char_reached){
            char_before = 1;
        }
        if(!isspace(cmd[j]) && cmd[j] != '>' && char_reached){
            char_after = 1;
        }
    }
    if(!(char_after && char_before && char_reached && char_counter == 1)){
        return 0;
    }

    // Builds arguments with string
    char** args_redirect = malloc(1000 * sizeof(char*)); 
    int count = 0;
    char* cmd_copy = strdup(cmd);
    char* saveptr_line;
    char* token = strtok_r(cmd_copy, ">", &saveptr_line);
    while (token != NULL) {
        char* trimmed_str = trim(token);
        args_redirect[count++] = trimmed_str;
        token = strtok_r(NULL, ">", &saveptr_line);
    }
    // ERROR HANDLING
    if((count > 2)){ 
        return 0; //If there are more than 2 tokens aka more than 1 redirect
    }
    char* output_file_name = args_redirect[1];

    if (args_redirect[2] != NULL) {
        return 0; // More than one file name trying to be redirected to
    }
    FILE *test_file = fopen(output_file_name, "r");
    if(test_file != NULL){
        fclose(test_file);
        return 0; //file already exists
    }

    // Initialize redirection
    int og_stdout = dup(STDOUT_FILENO); //store orignal stdout
    int output_file = open(output_file_name, O_CREAT | O_APPEND | O_WRONLY, 0600);
    if (output_file < 0) {
        return 0;
    }
    if (dup2(output_file, STDOUT_FILENO) < 0) {
        close(output_file);
        return 0;
    }

    // Build exectable string
    cmd = args_redirect[0];
    char** args = malloc(100 * sizeof(char*)); 
    char* cmd_copy2 = strdup(cmd);
    char* saveptr_cmd2;
    int count2 = 0;
    // filling up the args with argv type array of strings for use
    char* token2 = strtok_r(cmd_copy2, " \t", &saveptr_cmd2);
    while (token2 != NULL) {
        args[count2++] = token2;
        token2 = strtok_r(NULL, " \t", &saveptr_cmd2);
    }
    args[count2] = NULL;
    if(!strcmp(args[0], "cd") || !strcmp(args[0], "pwd") || !strcmp(args[0], "exit")){
        return 0; //trying to perform redirection with built in command
    }

    // Execute String
    int valid;
    // Do execvp() on command
    pid_t pid = fork();
    if (pid < 0) {
        //myPrint("\n fork failed \n");
        valid = 0;
    } else if (pid == 0) {
        // Child process
        // Execute the command
        if (execvp(args[0], args) < 0) {
            valid = 0;
            print_error();
            exit(0);
        }
    } else {
        // Parent process
        wait(NULL);
    }
    valid = 1;
    // Close output file descriptor after redirection
    dup2(og_stdout, STDOUT_FILENO);
    close(og_stdout);
    close(output_file);
    
    return valid;
}

int main(int argc, char *argv[]) {
    int is_batch = 0;
    char *pinput;

    if(argc > 1){
	    is_batch = 1;
    }

    if(!is_batch){
	    char cmd_buff[5000]; // Maximum length of command line is 512 characters + newline
	    while (1) {
		    myPrint("myshell> ");
		    pinput = fgets(cmd_buff, sizeof(cmd_buff), stdin);
		    if (!pinput) {
			    print_error();
		    }
		    if (strlen(pinput) >= 512 && pinput[512] != '\n') {
			    // Print the whole line and error message
                myPrint(pinput);
			    print_error();
			    // Discard the line
			    continue; // Move to the next command line
		    }
            int num_cmds;
		    char** cmds = parsing(pinput, &num_cmds);
		    if (cmds == NULL) {
			    print_error();
			    continue;
		    }
		    for (int i = 0; i < num_cmds && cmds[i] != NULL; i++) {
			    int valid;
			    valid = executing_command(cmds[i], 0);
			    if(!valid){
			        print_error();
			        break;
		        }
	        }
        }
    }
    if(is_batch){
        char* file_name = argv[1];
        char line[5000];
        FILE* file = fopen(file_name, "r");

        if (argc > 2 || file == NULL) { //if more than 2 files specified or if file is unopenable
            print_error();
            my_exit();
        }
	    while (1){
            pinput = fgets(line, sizeof(line), file);
            // print if not empty line
            if (!is_empty(pinput)) {
                myPrint(pinput);
            }
            if (!strcmp(pinput, "")) {
                continue;
            } //checking if line too long
            if (strlen(pinput) >= 512 && pinput[512] != '\n') {
                // error message
                print_error();
                continue; // do while loop again
            }
            // assume this works (puting cmds that are stripped into array called cmds)
            int num_cmds;
            char** cmds = parsing(pinput, &num_cmds);
            if (*cmds == NULL) {
                continue;
            }
            // iterate through each command and execute them all
            // if in redirect mode will redirect command
            char* redirect;
            for (int i = 0; i < num_cmds; i++) {
                int valid;
                redirect = strchr(cmds[i], '>');
                if (redirect != NULL){
                    valid = execute_redirect_command(cmds[i]);
                }else{
                    valid = executing_command(cmds[i], 0);
                }
                if(!valid){
                    print_error();
                    continue;
                }
            }
        }
    }
}

					