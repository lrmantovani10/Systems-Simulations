#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include<fcntl.h>

void printSafe(char *txt)
{
    write(STDOUT_FILENO, txt, strlen(txt));
}

void raiseError(int errorCode){
    printSafe("An error has occurred\n");

    // Two or more input files to a program
    if(errorCode == 1)
        exit(0);
    
    // All other commands -- such as one that does not exist or cannot be executed
    // or A very long command line (over 512 characters, excluding the carriage return) --
    // simply print the error message, and the shell's execution continues.

}

int isBuiltIn(char *commandName){

    int builtIn = 0;

    if  ((strcmp(commandName, "exit") == 0) || 
        (strcmp(commandName, "cd") == 0) || 
        (strcmp(commandName, "pwd") == 0))
        builtIn = 1;

    return builtIn;
}

int isRedirection(char *compareString){
    if ((strcmp(compareString, ">") == 0) || (strcmp(compareString, ">+") == 0))
        return 1;
    else
        return 0;
}

int fileExists(char *filename){
    FILE *test;
    test = fopen(filename, "r");
    if(test){
        fclose(test);
        return 1;
    }

    return 0;
}

void processCommand(char **commandArray, int bufferLength){

    // Check for errors in the command
    int actualParameters = 0;
    int redirectCount = 0;
    for(int i = 0; i < bufferLength; i++){
        if(strlen(commandArray[i]) > 0){
            actualParameters++;

            // Raise error if redirecton is applied after a built-in command
            if(isBuiltIn(commandArray[i]) && (i + 1) < bufferLength && 
            isRedirection(commandArray[i + 1])){
                raiseError(0);
                return;
            }

            // Redirect format specifications
            if(isRedirection(commandArray[i])){

                redirectCount++;

                // Raise error if basic redirection is used for file that already exists
                if((strcmp(commandArray[i], ">") == 0) && 
                (i + 1) < bufferLength && strlen(commandArray[i + 1]) > 0 ){
                    if(fileExists(commandArray[i + 1])){
                        raiseError(0);
                        return;
                    }
                }

                // Raise error if redirection output file is not specified
                if((i + 1) < bufferLength && strlen(commandArray[i + 1]) == 0){
                    raiseError(0);
                    return;
                }

                // Raise error if there is more than one output file
                if((i + 2) < bufferLength && strlen(commandArray[i + 2]) > 0){
                    raiseError(0);
                    return;
                }   
            }
        }

    }

    // Maximum of one redirect per command
    if(redirectCount > 1){
        raiseError(0);
        return;
    }

    // Execute commands
   for(int i = 0; i < actualParameters; i++){

       // Built-in Commands
       if(isBuiltIn(commandArray[i])){

            // Exit takes at most one numerical argument
            if(strcmp(commandArray[i], "exit") == 0 && (((i + 1) == actualParameters) ||
            (atoi(commandArray[i + 1]) && (i + 2) == actualParameters)))
                exit(0);
            
            else if(strcmp(commandArray[i], "exit") == 0){
                raiseError(0);
                return;
            }

            else if(strcmp(commandArray[i], "cd") == 0){

                // If no argument is passed, go to home directory
                if((i + 1) == actualParameters)
                    chdir(getenv("HOME"));
                
                // If path doesn't exist, raise error
                else{
                    if(chdir(commandArray[i + 1]) < 0){
                        raiseError(0);
                        return;
                    }
                    i++;
                }
            }

            // pwd only works with no arguments
            else if(strcmp(commandArray[i], "pwd") == 0 && (
            (i + 1) == actualParameters)){

                int pwdbufferSize = 256;
                char pwdbuffer[pwdbufferSize];
                getcwd(pwdbuffer, pwdbufferSize);
                printSafe(pwdbuffer);
                printSafe("\n");
            }

            else if(strcmp(commandArray[i], "pwd") == 0){
                raiseError(0);
                return;
            }
       }

       // All other commands: create child processes 
       else if (strlen(commandArray[i]) > 0){

            // Getting the command's arguments to run it using execvp.
            // First step: allocating enough memory

            int argumentAccumulator = 0;
            for(int a = i + 1; a < actualParameters; a++){
                if(isRedirection(commandArray[a])){
                    break;
                }

                else if (strlen(commandArray[a]) > 0){
                    argumentAccumulator++;
                }
            }

            char** commandArguments = malloc(2 + (sizeof(char *) * argumentAccumulator));

            commandArguments[0] = malloc(strlen(commandArray[i]) + 1);
            argumentAccumulator = 1;
            for(int a = i + 1; a < actualParameters; a++){
                if(isRedirection(commandArray[a])){
                    break;
                }

                else if (strlen(commandArray[a]) > 0){
                    commandArguments[argumentAccumulator] = malloc(
                        strlen(commandArray[a]) + 1);
                    argumentAccumulator++;
                }
            }

            // Setting first argument to command name
            commandArguments[0] = commandArray[i];

            // Second step: filling up the argument array
            argumentAccumulator = 1;

            // Determine what type of redirection is occuring. 1 for basic,
            // 2 for advanced, and 0 for none. 
            int redirectionType = 0;
            char* redirectionFile;

            for(int a = i + 1; a < actualParameters; a++){

                if(strcmp(commandArray[a], ">") == 0){
                    redirectionType = 1;
                    redirectionFile = commandArray[a + 1];
                    break;
                }

                else if(strcmp(commandArray[a], ">+") == 0){
                    redirectionType = 2;
                    redirectionFile = commandArray[a + 1];
                    break;
                }

                else if (strlen(commandArray[a]) > 0){
                    commandArguments[argumentAccumulator] = strdup(commandArray[a]);
                    argumentAccumulator++;
                }
            }

            // Setting last pointer in argument to NULL and making 
            // argument array constant to match function specifications

            commandArguments[argumentAccumulator] = NULL;
            char* const* constantArguments = (char* const*) commandArguments;
            pid_t childPid = 0;
            int redirectionFD;
            int existingFile;
            char *temporaryFile;
            int temporaryFD;

            // Check if file exists (set to non-existent if basic redirect)
            if(redirectionType == 1)
                existingFile = 0;
            else if(redirectionType == 2)
                existingFile = fileExists(redirectionFile);

            // Basic redirection and advanced redirection for new file
            if(redirectionType > 0 && !existingFile){

                // Create file with read and write permissions for file owner,
                // read permissions for other users

                redirectionFD = creat(redirectionFile, S_IRUSR | 
                S_IWUSR | S_IRGRP | S_IROTH);
                if(redirectionFD < 0){
                    raiseError(0);
                    return;
                }

                // Create copy of the standard out file descriptor
                // on file descriptor #4
                dup2(STDOUT_FILENO, 4);

                // Redirect stdout output to file descriptor of newly created file
                dup2(redirectionFD, STDOUT_FILENO);

            }

            // Advanced redirection
            else if (redirectionType == 2){

                temporaryFile = strcat(strdup(redirectionFile), "_TEMP");

                // Create file with read and write permissions for file owner,
                // read permissions for other users
                temporaryFD = creat(temporaryFile, S_IRUSR | 
                S_IWUSR | S_IRGRP | S_IROTH);

                if(temporaryFD < 0){
                    raiseError(0);
                    return;
                }

                // Create copy of the standard out file descriptor
                // on file descriptor #4
                dup2(STDOUT_FILENO, 4);

                // Redirect stdout output to file descriptor of newly created file
                dup2(temporaryFD, STDOUT_FILENO);
            }

            // Child process
            if (fork() == 0){
                childPid = getpid();
                int statusCode = execvp(commandArray[i], constantArguments);

                // Commands that don't exist will raise an error
                if (statusCode){

                    /* To make redirecton not work for a non-existent program:
                    if(redirectionType > 0){
                        if(close(redirectionFD) < 0){
                            raiseError(0);
                            return;
                        }

                        // Restore standard output
                        dup2(4, 1);

                        // Delete created file
                        if(remove(redirectionFile) != 0){
                            raiseError(0);
                            return;
                        }
                    }
                    */
                    raiseError(0);
                }
                
                exit(0);
            }

            // Parent process
            else{
                int childStatus;
                waitpid(childPid, &childStatus, 0);

                // Make stdout the default output again after closing the opened file
                if(redirectionType > 0){
                    
                    // Now that we have written the command's output to the temporary 
                    // file, we must copy the contents of the file we want to modify to it. 
                    // Then, we can replace the original file's old content with the new file's.
                    if(redirectionType == 2 && existingFile){

                        // File we want to modify
                        FILE *redirectionStream = fopen(redirectionFile, "r");
                        
                        char buffer[] = {'\0', '\0'};
                        if(redirectionStream == NULL){
                            raiseError(0);
                            return;
                        }

                        fgets(buffer, 1, redirectionStream);

                        // Write everything we want (old file contents and new
                        // command output) to temporary file
                        char currentCharacter = (char) fgetc(redirectionStream);
                        while(currentCharacter != EOF){
                            buffer[0] = currentCharacter;
                            buffer[1] = '\0';
                            write(STDOUT_FILENO, buffer, 1);
                            currentCharacter = (char) fgetc(redirectionStream);
                        }

                        // Close temporary file
                        if(close(temporaryFD) < 0){
                            raiseError(0);
                            return;
                        }

                        // Restore standard output
                        dup2(4, 1);

                        // Close redirection file
                        if(fclose(redirectionStream) != 0){
                            raiseError(0);
                            return;
                        }
                        
                        if(remove(redirectionFile) < 0){
                            raiseError(0);
                            return;
                        }
                        
                        // Rename temporary file to redirection file's name
                        if(rename(temporaryFile, redirectionFile) != 0){
                            raiseError(0);
                            return;
                        }
                    }

                    else{
                        // Close redirection file
                        if(close(redirectionFD) < 0){
                            raiseError(0);
                            return;
                        }

                        // Restore standard output
                        dup2(4, 1);

                    }
                }

                return;
            }

       }
   }
}

void clearArray(char **array, int arrayLength){
    int currentLength;
    for(int a = 0; a < arrayLength; a++){
        currentLength = strlen(array[a]);
        for(int b = 0; b < currentLength; b++){
            array[a][b] = '\0';
        }
    }
}

int main(int argc, char *argv[]) 
{   
    int bufferLength = 514;
    char cmdBuff[bufferLength];

    // Fill cmd buff with null characters
    for(int i = 0; i < bufferLength; i++)
        cmdBuff[i] = '\0';
    
    char *pinput;
    FILE *f;

    // Open file if in batch mode
    if(argc == 2){
        f = fopen(argv[1], "r");
        if(!f)
            raiseError(1);
    }

    // Valid file runs
    if (argc <= 2){
        
        while (1) {

            int clearance = 1;
            int strLength;

             // Interactive mode
            if(argc == 1){
                printSafe("myshell> ");

                pinput = fgets(cmdBuff, bufferLength, stdin);
                strLength = strlen(pinput);
                
                // Error obtaining input / reading file
                if (!pinput) 
                    exit(0);
            }

            // Batch mode
            else{   
                pinput = fgets(cmdBuff, bufferLength, f);
                // Empty input -- break loop
                if(!pinput)
                    return 0;
                strLength = strlen(pinput);

                // Error obtaining input / reading file
                if (!pinput) 
                    exit(0);
            }

            // Check for command line that is too long
            int letterSum = 0;
            for (int i = 0; i <= strLength; i++){
                int asciiValue = cmdBuff[i];
                if(asciiValue != 10 && asciiValue > 0)
                    letterSum++;
            }

            // How to deal with long inputs - discard everything up to new line / null
            if(letterSum > 512){
                char *jointString = strcat(strdup(cmdBuff), "");

                if(argc == 2)
                    printSafe(jointString);
                char *currentCharacter = "1";
                char characterString[2];

                FILE *chosenFile;

                if(argc == 2)
                    chosenFile = f;
                else
                    chosenFile = stdin;

                while(currentCharacter[0] >= 32){
                    currentCharacter = fgets(characterString, 2, chosenFile);
                    characterString[1] = '\0';

                    if(argc == 2)
                        printSafe(characterString);
                }
                raiseError(0);
                clearance = 0;
            }

            // In batch mode, don't print blank lines
            int notBlank = 0;
            for(int i = 0; i <= strlen(cmdBuff); i++){
                if((int) cmdBuff[i] >= 33)
                    notBlank = 1;
            }

            if(clearance && argc == 2 && notBlank)
                printSafe(cmdBuff);

            int externalAccumulator = 0;
            int internalAccumulator = 0;
            int arraySize = bufferLength / 2;
            char **commands = malloc(arraySize * sizeof(char *));
            int parsingSpaces = 0;

            // filling command array with null characters
            for(int a = 0; a < arraySize; a++){
                commands[a] = malloc(bufferLength);
                for(int b = 0; b < bufferLength; b++)
                    commands[a][b] = '\0';
            }

            // Process input
            if(clearance){
                for(int i = 0; i <= strLength; i++){

                    int asciiValue = (int) cmdBuff[i];
                    // Not a space, newline, tab, nor NULL
                    if(asciiValue > 0 && asciiValue != 32 && asciiValue != 9){

                        // Semicolons or newline characters
                        if(asciiValue == 59 || asciiValue == 10){
                            processCommand(commands, arraySize);
                            clearArray(commands, externalAccumulator + 1);
                            externalAccumulator = 0;
                            internalAccumulator = 0;
                        }

                        // Redirections
                        else if(asciiValue == 62){
                            
                            // Redirection sign in the middle of another command
                            if(strlen(commands[externalAccumulator]) > 0)
                                externalAccumulator++;
                            commands[externalAccumulator][0] = '>';

                            // Advanced redirection
                            if((i + 1) < strLength && (int) cmdBuff[i + 1] == 43){
                                commands[externalAccumulator][1] = '+';
                                i++;
                            }
                            externalAccumulator++;
                            parsingSpaces = 0;
                            internalAccumulator = 0;
                        }

                        // Other characters
                        else{
                            commands[externalAccumulator][internalAccumulator] = cmdBuff[i];
                            internalAccumulator++;
                            parsingSpaces = 0;
                        }
                    }

                    // Spaces and tabs
                    else if((asciiValue == 32 || asciiValue == 9) && (
                        strlen(commands[externalAccumulator]) > 0)
                        && (!parsingSpaces)){
                            parsingSpaces = 1;                            
                            externalAccumulator++;
                            internalAccumulator = 0;
                    }
                }
            }
        }
    }

    // More than one file argument provided --  terminate program
    else{
        raiseError(1);
    }

    // Close opened file
    if(argc == 2)
        fclose(f);
}
