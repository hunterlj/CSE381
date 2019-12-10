/*
 * Copyright (C) 2019 ulmeslj@miamiOH.edu
 */

/* 
 * File:   ulmeslj_hw3.h
 * Author: Leann Hunter
 *
 * Created on September 13, 2019, 1:29 PM
 */

/*
 * You will need 2 public methods – 
 * 1. a method that loads process data from a 
 * given file into 2 unordered maps (discussed below) 
 * 2. a method that prints the process tree for a given PID. 
 * You may add any private helper methods as you see fit. 
 */

#ifndef ULMESLJ_HW3_H
#define ULMESLJ_HW3_H
#include <string>
#include <unordered_map>
class ulmeslj_hw3 {   
public:
    ulmeslj_hw3();
    
    virtual ~ulmeslj_hw3();

    virtual std::unordered_map<int, int> load(std::istream& is);
    
    virtual std::unordered_map<int, std::string> load2(std::istream& is);

    virtual void print(std::unordered_map<int, int> ppidMap, 
    std::unordered_map<int, std::string> cmdMap, int input);

private:
};
#endif /* ULMESLJ_HW3_H */

