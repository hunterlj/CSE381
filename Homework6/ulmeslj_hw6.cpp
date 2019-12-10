/*
 * Copyright [2019] ulmeslj
 */

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp> 
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <thread>
#include <mutex>  
#include <sstream>

//  for boost socket
using namespace boost::asio;
using namespace boost::asio::ip;

//  for vectors
using StrVec = std::vector<std::string>;

//  for threads
using ThreadList = std::vector<std::thread>;

using StrInt = std::vector<int>;

StrVec dictionary;

/*
 * reads the english text file and places each word into a dictionary vector
 */
StrVec load(const std::string& dictionaryFile) {
    std::string words;
    std::ifstream in(dictionaryFile);
    std::istream_iterator<std::string> ini(in), eof;
    StrVec dictionary(ini, eof);
    std::sort(dictionary.begin(), dictionary.end());
    return dictionary;
}

int countWords(std::string line) {
    int count = 0;
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        count++;
    }
    return count;
}

int countEnglish(std::string line) {
    int count = 0;
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        std::transform(word.begin(), word.end(), word.begin(), tolower);
        if (std::binary_search(dictionary.begin(), dictionary.end(), word)) {
            count++;
        }
    }
    return count;
}

std::string cutSpaces(const std::string& s) {
  std::string result = boost::trim_copy(s);
  while (result.find("  ") != result.npos) {
    boost::replace_all(result, "  ", " ");
  }
  return result;
}

std::string process(std::string fileName) {
    std::string url = "/~raodm/cse381/hw6/SlowGet.cgi?file=";
    std::string host = "ceclnx01.cec.miamioh.edu";
    std::string line;
    int wordCount = 0;
    int englishCount = 0;
    ip::tcp::iostream stream;
    stream.connect(host, "80");
    stream << "GET" << url << fileName << " HTTP/1.1\r\n" 
           << "Host: " << host << "\r\n"
           << "Connection: Close\r\n\r\n";
    
    while (std::getline(stream, line), line != "\r") {}
    
    while (std::getline(stream, line)) {
        std::replace_if(line.begin(), line.end(), ispunct, ' ');
        line = cutSpaces(line);
        wordCount += countWords(line);
        englishCount += countEnglish(line);
    }
    std::ostringstream result; 
    result << "URL: http://os1.csi.miamioh.edu" << url << fileName 
            << ", words: " << wordCount << ", English words: " << englishCount;
    return result.str(); 
}

void threadMain(StrVec& fileList, StrVec& resultList,
        const int startIdx, const int count) {
    int end = (startIdx + count);
    for (int i = startIdx; (i < end); i++) {
        resultList[i] = process(fileList[i]);
    } 
}

void thread2(StrVec& fileList, StrVec& resultList, 
        int numThreads) {
    if (numThreads % 2) {
        numThreads++;
    }
    const int count = fileList.size() / numThreads;
    resultList.resize(fileList.size());
    ThreadList thrList;
    
    for (int start = 0, thr = 0; (thr < numThreads); thr++, start += count) {
        thrList.push_back(std::thread(threadMain, std::ref(fileList), 
                std::ref(resultList), start, count));
    }
    for (auto& t : thrList) {
        t.join();
    }
}

int main(int argc, char** argv) {
    dictionary = load("english.txt");
    StrVec inputs;
    int numThreads = std::stoi(argv[1]);
    for (int i = 2; (i < argc); i++) {
        inputs.push_back(argv[i]);
    }
     if (numThreads > 1) { 
        StrVec resultList;
        StrVec fileList;
        thread2(inputs, resultList, numThreads);
        for (size_t i = 0; i < resultList.size(); i++) {
            std::cout << resultList[i] << std::endl;
        }
    } else {
        for (size_t i = 0; i < inputs.size(); i++) {
            std::cout << process(inputs[i]) << std::endl;
        }
    }
}
