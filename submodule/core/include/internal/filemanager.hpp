#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
namespace fsmanager{
    namespace file{
        std::string read(std::string path){
            std::ifstream filer(path);
            if (!filer)
            {
                return "";
            }
            std::string buffer;
            filer.seekg(0, std::ios::end);
            buffer.resize(filer.tellg());
            filer.seekg(0, std::ios::beg);
            filer.read(&buffer[0], buffer.size());
            filer.close();
            return buffer;
        }
        bool write(std::string path, const std::string& data){
            std::ofstream filer(path);
            if (!filer)
            {
                return false;
            }
            filer.write(data.c_str(), data.size());
            filer.close();
            return true;
        }
        std::vector<unsigned char> readbin(std::string path){
            std::vector<unsigned char> buffer;
            std::ifstream filer(path,std::ios::binary);
            if (!filer)
            {
                return buffer;
            }
            filer.seekg(0, std::ios::end);
            buffer.resize(filer.tellg());
            filer.seekg(0, std::ios::beg);
            filer.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            return buffer;
        }
        bool writebin(std::string path, const std::vector<unsigned char>& data){
            std::ofstream filer(path, std::ios::binary);
            if (!filer)
            {
                return false;
            }
            filer.write(reinterpret_cast<const char*>(data.data()), data.size());
            return true;
        }
        bool exists(std::string path){
            return std::filesystem::exists(path)&& !std::filesystem::is_directory(path);
        }
        bool remove(std::string path){
            return std::filesystem::remove(path);
        }
    }
    namespace directory{
        bool exists(std::string path){
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        }
        bool remove(std::string path){
            return std::filesystem::remove_all(path);
        }
        bool create(std::string path){
            return std::filesystem::create_directory(path);
        }
        std::vector<std::string> list_files_in_directory(std::string path){
            std::vector<std::string> filelist;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if(!std::filesystem::is_directory(entry.path())){
                    filelist.push_back(entry.path().filename().string());
                }
            }
            return filelist;
        }
    }
}