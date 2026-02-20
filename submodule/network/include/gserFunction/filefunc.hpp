#ifndef FILEFUNC_H
#define FILEFUNC_H

#include <string>
#include <vector>

// Function to read a file and return its contents as a string
std::string readFile(const std::string& filePath);

// Function to write a string to a file
void writeFile(const std::string& filePath, const std::string& content);

// Function to split a string into lines
std::vector<std::string> splitIntoLines(const std::string& text);
namespace Tfile
{
    void setup();
    void getworldfile(std::vector<unsigned char>&chunkData, const std::string& worldName,long long chunkX, long long chunkZ);
    void saveworldfile(const std::vector<unsigned char>&chunkData, const std::string& worldName,long long chunkX, long long chunkZ);
    
} // namespace Tfile


#endif // FILEFUNC_H