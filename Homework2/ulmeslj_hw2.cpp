/* 
 * File:   ulmeslj_hw2.cpp
 * Copyright (C) 2019 ulmeslj@miamiOH.edu
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <iomanip>

using namespace std;

using db1 = std::unordered_map<int, std::string>; 
using db2 = std::unordered_map<int, std::vector<int>>;


/**
 * Method used twice, once for getting uid and loginID from passwd
 * then again for getting gid and groupdID from groups
 * @param passwdFile
 * @return 
 */
db1 getUsers(std::ifstream& file) {
    std::string line;
    db1 userInfo;
    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream is(line);
        int uid;
        std::string passkey;
        std::string loginID;
        is >> loginID >> passkey >> uid;
        userInfo[uid] = loginID;   
    }
    return userInfo;
}

db2 getGroups(std::ifstream& file) {
    std::string line;
    db2 groupInfo;
    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream is(line);
        
        int gid, uid;
        std::string groupID, passkey;
        is >> groupID >> passkey >> gid;
        std::vector<int> uidList;
        while (is >> uid) {
            uidList.push_back(uid);
        }
        groupInfo[gid] = uidList;
    }
    return groupInfo;
}

void printUsers(db1 userInfo, db2 groupInfo, db1 groupIDs, 
        int input) {
    std::vector<int> uidList; 
    if (groupInfo.find(input) != groupInfo.end()) {
        uidList = groupInfo[input];
        std::string groupID = groupIDs[input]; 
        
        std::cout << input << " = " << groupID << ":";
        for (auto uid : uidList) {
            std::string loginID = userInfo[uid];
            std::cout << " " << loginID << "(" << uid << ")";
        }
        std::cout << "\n";           
    } else {
        std::cout << input << " = Group not found." << std::endl;  
    }  
}

void readFiles(int input) {
    std::ifstream groupsFile("groups");
    std::ifstream passwdFile("passwd");
    std::ifstream groupsFile2("groups");
    
    db1 userInfo;  // key = uid, value = loginID from passwd
    db2 groupMembers;  // key = gid, value = vector of uids
    db1 groupIDs;  // key = gid, value = groupID
    
    userInfo = getUsers(passwdFile);
    groupMembers = getGroups(groupsFile); 
    groupIDs = getUsers(groupsFile2);
    
    printUsers(userInfo, groupMembers, groupIDs, input);
}

int main(int argc, char *argv[]) {
    for (int i = 1; (i < argc); i++) {
        std::string str = argv[i];
        int input = stoi(str);
        readFiles(input);
    }  
    return 0;      
}

