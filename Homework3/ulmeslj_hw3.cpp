/*
 * Copyright (C) 2019 ulmeslj@miamiOH.edu
 */

/* 
 * File:   ulmeslj_hw3.cpp
 * Author: Leann Hunter
 *
 * Created on September 13, 2019, 1:29 PM
 */
/*
 * Your main method should create an object and calls methods on the object 
 * with suitable parameters.
 * 
 * Two maps
 * 1. pid -> ppid information to ease look-up of parent process ID.
 * 2. pid -> cmd information to ease look-up the command associated with a PID.
 * 
 * Use std::istringstream to process each line. To read the full command 
 * (which has spaces in it) use std::getline method.
 * 
 * For printing the process hierarchy in a top-down manner, you may use an 
 * iterative or a recursive solution as you see fit. However, the recursive 
 * solution will most likely be shorter than an iterative solution. 
 */

#include "ulmeslj_hw3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iomanip>

using namespace std;

ulmeslj_hw3::ulmeslj_hw3() { }

ulmeslj_hw3::~ulmeslj_hw3() {}

std::unordered_map<int, int> ulmeslj_hw3::load(std::istream& is) {
    std::string line;
    std::unordered_map<int, int> ppidMap;
    while (std::getline(is, line)) {
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::replace(line.begin(), line.end(), ' ', ' ');
        std::istringstream is(line);
        std::string uid;
        int pid, ppid;
        is >> uid >> pid >> ppid;
        ppidMap[pid] = ppid;
    }
    return ppidMap;
}

std::unordered_map<int, std::string> ulmeslj_hw3::load2(std::istream& is) {
    std::string line;
    std::unordered_map<int, std::string> cmdMap;
    while (std::getline(is, line)) {
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::istringstream is(line);
        std::string uid, junk, junk1, junk2, cmd, cmd2;
        int pid, ppid, junk3;
        is >> uid >> pid >> ppid >> junk3 >> junk >> junk1 >> junk2 >> cmd;
        getline(is, cmd2);
        cmdMap[pid] = cmd + cmd2;
    }
    return cmdMap;
}

void ulmeslj_hw3::print(std::unordered_map<int, int> ppidMap, 
        std::unordered_map<int, std::string> cmdMap, int input) {
    std::stack<string> out;
    int value = input;
    do {
        std::stringstream is;
        is << value << "\t" << ppidMap[value] << "\t ";
        is << cmdMap[value] << "\n";
        std::string contents = is.str();
        out.push(contents);
        value = ppidMap[value];
    } while (value != 0);
    
    std::cout << "Process tree for PID: " << input;
    std::cout << "\n" << "PID\t" << "PPID\t" << "CMD\n";
    
    while (out.size() > 0) {
        std::string display = out.top();
        std::cout << display;
        out.pop();
    }
}

int main(int argc, char** argv) {
    std::string file = argv[1];
    int input = atoi(argv[2]);
    std::ifstream process(file);
    std::ifstream process2(file);
    ulmeslj_hw3 program;
    std::unordered_map<int, int> ppidMap;
    std::unordered_map<int, std::string> cmdMap;
    ppidMap = program.load(process);
    cmdMap = program.load2(process2);
    
    program.print(ppidMap, cmdMap, input);
    
    return 0;
}
