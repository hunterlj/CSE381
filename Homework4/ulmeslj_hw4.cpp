/*
 * Copyright (C) 2019 ulmeslj@miamiOH.edu
 */

/*
 * Repeatedly prompt (via a simple "> ") and obtain a line of input 
 * (via std::getline) and process each line of input in the following order:
 * • If the line is empty or the line begins with a pound (#) sign, 
 *   ignore those lines. They serve as comments similar to how bash shell works.
 * • The 1st word in the line is assumed to be a command (case sensitive) 
 *   and must be processed as:
 *     1. If the command is exit your shell must terminate
 *     2. If the command is SERIAL then the 2nd word is name of a file 
 *        that contains the actual commands to be executed one at a time. 
 *        The shell waits for each command to finish.
 *     3. If the command is PARALLEL then the 2nd word is name of a file that 
 *        contains the actual commands to be executed. In this case, all the 
 *        commands are first exec'd. Then wait for all the processes to finish 
 *        in the same order they were listed.
 *     4. If the 1st word is not one of the above 3, then it is assumed to be 
 *        the name of the program to be executed and rest of the words on the 
 *        line are interpreted as a command-line arguments for the program.
 * 
 * For every command run (including SERIAL and PARALLEL runs), the shell must 
 * print the command and command line arguments. Once the command has finished, 
 * the shell must print the exit code. 
 * 
 * 
 * TIPS:
 * • use concepts from lab exercise on working with fork and exec. 
 * • code you need is in lecture slides: string processing, vector, fork, exec 
 * • use the myExec program from lecture slides to minimize sources of errors
 *  (forgetting to add the nullptr)
 * • Get base case first. string processing is trivial with std::quoted. 
 *   Do not overcomplicate the string processing. For reading command from user
 *   prefer the following style of coding –
 * // Adapt the following loop as you see fit
 *    std::string line;
 *    while (std::cout << "> ", std::getline(std::cin, line)) {
 * // Process the input line here.
 *    }
 * • code the base case method to use generic I/O streams (Exercise #1)
 * • Use a std::vector to hold PIDs of child processes, you can call waitpid 
 *   on each one when running them in parallel. Exit codes are obtained from 
 *   waitpid. printing zero for exit code is totally wrong..
 * 
 * 
 */
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>

using StrVec = std::vector<std::string>;
using IntVec = std::vector<int>;

void myExec(StrVec argList) { 
    //  method from slides
    std::vector<char*> args;
    // list of pointers to args
    for (auto& s : argList) { 
        args.push_back(&s[0]);
        // address of 1st character
    }
    args.push_back(nullptr);
    // Make execvpsystem call to run desired process
    execvp(args[0], &args[0]);
}

int forkEx(StrVec args) {
    //  fork + exec method from slides
    const int pid = fork();
    if (pid == 0) {
        myExec(args);
    } else {
        return pid;
    }
    return pid;
}

StrVec commandArgs(std::string line) {
    //  place commands in vector
    StrVec args;
    std::istringstream is(line);
    std::string command;
    
    while (is >> std::quoted(command)) {
        args.push_back(command);
    }  
    //  if the first command is not parallel or serial
    if (args[0] != "SERIAL" && args[0] != "PARALLEL") {
        std::cout << "Running:";
    } 
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "SERIAL" && args[0] != "PARALLEL") {
            i += 2;
        } 
        if (args[0] != "SERIAL" && args[0] != "PARALLEL") {
            std::cout << " " << args[i];
        }
    }
    if (args[0] != "SERIAL" && args[0] != "PARALLEL") {
        std::cout << std::endl;
    }
    return args;    
}

void processes(StrVec args, IntVec pids) {
    //  for serial and parallel processes
    std::ifstream file(args[1]);
    std::string line;
    int pid;
    int exitCode;
    while (std::getline(file, line)) {
        if (line.substr(0, 1) != "" && line.substr(0, 1) != "#") {
            StrVec args = commandArgs(line);
            pid = forkEx(args);
            if (args[0] == "PARALLEL") {
                pids.push_back(pid);
            } else {
                waitpid(pid, &exitCode, 0);
                std::cout << "Exit code: " << exitCode << std::endl;
            }
        }
    }
    if (args[0] == "PARALLEL") {
        for (size_t i = 0; i < pids.size(); i++) {
            waitpid(pids[i], &exitCode, 0);
            std::cout << "Exit code: " << exitCode << std::endl;
        }       
    }      
}

int main(int argc, char** argv) {
    std::string line;
    std::string word;
    int pid;
    int exitCode;
    while (std::cout << "> ", std::getline(std::cin, line)) {      
        if (line.substr(0, 1) == "#") {
            continue;
        } else if (line == "") {
            continue;
        } else if (line == "exit") {
            break;   
        } else {
            StrVec args = commandArgs(line);  
            IntVec pids;
            if (args[0] == "SERIAL" || args[0] == "PARALLEL") {
                processes(args, pids);
            } else {
                pid = forkEx(args);
                waitpid(pid, &exitCode, 0);
                std::cout << "Exit code: " << exitCode << std::endl;
            }
        }    
    }
    return 0;
}
