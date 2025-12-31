#include <gserfunction/filefunc.hpp>
#include <fstream>
#include <sstream>
std::string readFile(const std::string& filePath){
    std::ifstream file(filePath);
    if (!file) {
        // Handle error
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Function to write a string to a file
void writeFile(const std::string& filePath, const std::string& content){
    std::ofstream file(filePath);
    if (!file) {
        // Handle error
        return;
    }
    file << content;
}

// Function to split a string into lines
std::vector<std::string> splitIntoLines(const std::string& text){
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}