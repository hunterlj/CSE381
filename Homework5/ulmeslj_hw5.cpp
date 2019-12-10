/* 
 * Copyright [2019] <ulmeslj>
 * A simple web-server.  
 * 
 * The web-server performs the following tasks:
 * 
 *     1. Accepts connection from a client.
 *     2. Processes cgi-bin GET request.
 *     3. If not cgi-bin, it responds with the specific file or a 404.
 * 
 */

#include <ext/stdio_filebuf.h>
#include <unistd.h>
#include <sys/wait.h>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

// Using namespaces to streamline code below
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

/** Forward declaration for method defined further below.  You will
    need to use this url_decode method in your serveClient method.
 */
std::string url_decode(std::string data);
using StrVec = std::vector<string>;
// Named-constants to keep pipe code readable below
const int READ = 0, WRITE = 1;
void child(int pipfd[], std::string cmd, StrVec contents);
void parent(int pipefd[]);

void lines(std::istream& in) {
    std::string input;
    while (std::getline(in, input)) {        
        // Send chunk size to client.
        std::cout << std::hex << input.size()+1 << "\r\n";
        // Write the actual data for the line.
        std::cout << input << "\r\n";
        std::cout << "\r\n";
    }
    std::cout << "0\r\n";  // Last line
}
void myExec(std::string cmd, StrVec args) {
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == 0) {
        parent(pipefd);
    } else {
        child(pipefd, cmd, args);
        waitpid(pid, nullptr, 0);
    }
}

void processInputs(std::string line) {
    std::string cmd, args;
    cmd = line.erase(0, line.find_first_of('='));
    cmd = url_decode(cmd.substr(0, cmd.find('&')));
    args = line.erase(0, line.find_last_of('='));
    args = url_decode(args);
    
    std::string temp;
    std::istringstream is(args);
    StrVec contents;
    while (is >> quoted(temp)) {
        contents.push_back(temp);
    }
    myExec(cmd, contents);
}
/**
 * Process HTTP request (from first line & headers) and
 * provide suitable HTTP response back to the client.
 * 
 * @param is The input stream to read data from client.
 * @param os The output stream to send data to client.
 */
void serveClient(std::istream& is, std::ostream& os) {
    std::string line, cgi, s2;
    StrVec inputs;
    getline(is, line);
    while (!line.empty()) {
        inputs.push_back(line);
        getline(is, line);
    }

    std::string s1 = inputs[0];
    s2 = s1.erase(0, s1.find('/') + 1);
    s1.erase(s1.find(' '), s1.length());
    
    std::ifstream input(s1);
    std::string file = s1;
    
    cgi = file.substr(0, 7);
    if (cgi == "cgi-bin") {
        os << "HTTP/1.1 200 OK\r\n";
        os << "Content-Type: text/plain\r\n";
        os << "Transfer-Encoding: chunked\r\n";
        os << "Connection: Close\r\n";
        os << "\r\n";
        processInputs(line);
    } else {
        os << "HTTP/1.1 404 Not Found\r\n";
    }
    lines(input);

    // Implement this method as per homework requirement. Obviously
    // you should be thinking of structuring your program using helper
    // methods first. You should add comemnts to your helper methods
    // and then go about implementing them.
}

void parent(int pipefd[]) {
    close(pipefd[WRITE]); 
    
    __gnu_cxx::stdio_filebuf<char> fb(pipefd[READ], std::ios::in, 1);
    std::istream is(&fb);
    std::string line;
    
    while (std::getline(is, line)) {
        std::cout << std::hex << line.size()+1 << "\r\n";
        std::cout << line << "\r\n\r\n";
    }
    
    std::string exitCode = "Exit code: 0\r\n\r\n";
    std::cout << std::hex << exitCode.size() << "\r\n\r\n";
    std::cout << exitCode;
    close(pipefd[READ]);
}

void child(int pipefd[], std::string cmd, StrVec contents) {
    std::vector<char*> args;
    args.push_back(&cmd[0]);
    
    for (size_t i = 0; i < contents.size(); i++) {
        args.push_back(&contents[i][0]);
    }
    args.push_back(nullptr);
    
    close(pipefd[READ]); 
    dup2(pipefd[WRITE], WRITE);
    
    __gnu_cxx::stdio_filebuf<char> fb(pipefd[WRITE], std::ios::out, 1);
    std::ostream os(&fb);
    std::string line;
    fflush(stdout);
    execvp(&cmd[0], &args[0]);
    fflush(stdout);
    close(pipefd[WRITE]);
}

// -----------------------------------------------------------
//       DO  NOT  ADD  OR MODIFY CODE BELOW THIS LINE
// -----------------------------------------------------------

/** Convenience method to decode HTML/URL encoded strings.

    This method must be used to decode query string parameters
    supplied along with GET request.  This method converts URL encoded
    entities in the from %nn (where 'n' is a hexadecimal digit) to
    corresponding ASCII characters.

    \param[in] str The string to be decoded.  If the string does not
    have any URL encoded characters then this original string is
    returned.  So it is always safe to call this method!

    \return The decoded string.
*/
std::string url_decode(std::string str) {
    // Decode entities in the from "%xx"
    size_t pos = 0;
    while ((pos = str.find_first_of("%+", pos)) != std::string::npos) {
        switch (str.at(pos)) {
            case '+': str.replace(pos, 1, " ");
            break;
            case '%': {
                std::string hex = str.substr(pos + 1, 2);
                char ascii = std::stoi(hex, nullptr, 16);
                str.replace(pos, 3, 1, ascii);
            }
        }
        pos++;
    }
    return str;
}

/**
 * Runs the program as a server that listens to incoming connections.
 * 
 * @param port The port number on which the server should listen.
 */
void runServer(int port) {
    io_service service;
    // Create end point
    tcp::endpoint myEndpoint(tcp::v4(), port);
    // Create a socket that accepts connections
    tcp::acceptor server(service, myEndpoint);
    std::cout << "Server is listening on port "
              << server.local_endpoint().port() << std::endl;
    // Process client connections one-by-one...forever
    while (true) {
        tcp::iostream client;
        // Wait for a client to connect
        server.accept(*client.rdbuf());
        // Process information from client.
        serveClient(client, client);
    }
}

/*
 * The main method that performs the basic task of accepting connections
 * from the user.
 */
int main(int argc, char** argv) {
    if (argc == 2) {
        // Process 1 request from specified file for functional testing
        std::ifstream input(argv[1]);
        serveClient(input, std::cout);
    } else {
        // Run the server on some available port number.
        runServer(0);
    }
    return 0;
}

// End of source code
