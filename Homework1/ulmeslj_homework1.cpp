/*
 * Copyright (C) 2019 ulmeslj@miamioh.edu
 */

#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>


using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;
using EmployeeMap = std::unordered_map<std::string, int>;

/**
 * Method to read file line by line.
 * @param is file name
 * @param os
 */

void printEmployee(std::istream& is, const std::string& 
                    employee, std::string maxFlag) {    
    // Create the entry for the file to be read
    // Create the map to be populated and returned
    EmployeeMap db;
    // Load employee information into the DB by reading 
    // entry-by-entry and then add them to the unordered_map using
    // operator[].  Use the name of the employee as the key.
    std::string name;
    std::string customer;
    int sales;
    std::getline(is, name);
    while (is >> std::quoted(name) >> std::quoted(customer) 
            >> sales) {          
        db[name] += sales;           
    }   
    // Return the map of people back
    if (db.find(employee) != db.end()) {
        std::cout << "Sales by " << employee << " = " << db[employee] 
                << "\n";
    } else {
        std::cout << "Employee " << employee << " not found.\n";
    }
    if (maxFlag == "true") {
        auto topEmployee = *std::max_element(db.begin(), db.end(), 
                [](auto& e1, auto& e2) { 
                    return e1.second < e2.second; 
        });

        std::cout << "Top employee: " << topEmployee.first << " with sales: " 
                << topEmployee.second << std::endl;
    }    
}

/**
 * Helper method to create an TCP I/O stream to send an HTTP request
 * to a web server and obtain file contents as response.  Note that
 * this method does not process the response. However, it reads and
 * discards the HTTP response headers sent by the server.
 *
 * @param path 
 *
 * @param host An optional host name for the web server. The default
 * host is currently assumed to be ceclnx01.cec.miamioh.edu.
 */
bool setupHttpStream(tcp::iostream& stream, const std::string& path,
                     const std::string& host = "ceclnx01.cec.miamioh.edu") {
    // Establish a TCP connection to the given web server at port
    // number 80.
    stream.connect(host, "80");
    if (!stream.good()) {
        return false;  // invalid connection. Nothing further to do.
    }

    // Send HTTP request to the web server requesting the desired
    // file.  This is the same GET request that a browser generates to
    // obtain the file from the server.
    stream << "GET /"  << path << " HTTP/1.1\r\n"
           << "Host: " << host << "\r\n"
           << "Connection: Close\r\n\r\n";

    // Assuming the file is valid, the web server will send us a
    // response.  First, HTTP response code and ensure it is "200 OK"
    // status code indicating things went well at the server.
    std::string line;
    std::getline(stream, line);  // Read a whole line! yes, important.
    if (line.find("200 OK") == std::string::npos) {
        return false;  // Web server reported an error!
    }

    // Next read and discard HTTP response headers. 
    while (std::getline(stream, line) && (line != "\r") && !line.empty()) {}
    return true;
}

int main(int argc, char *argv[]) {
    const std::string url = argv[1];
    
    std::string test = url.substr(7);
    int point = test.find('/');
    
    const std::string host = test.substr(0, point);
    const std::string file = test.substr(point);
    
    const std::string& employee = argv[2];
    const std::string maxFlag = argv[3];
    
    tcp::iostream stream;
    
    if (!setupHttpStream(stream, file, host)) {
        // Something went wrong in getting the data from the server.
        std::cout << "Error obtaining data from server.\n";
        return 1;  // Unsuccessful run of program (non-zero exit code)
    } 
    
    // load employee info from file to database
    // EmployeeMap db = load(stream);
    printEmployee(stream, argv[2], argv[3]);
    // find the employee provided from command line arguments
    // if prompted, print out the tops sales employee
    
    return 0;
}
