#ifndef Log_hpp
#define Log_hpp
#define debuginfo
#include <iostream>
#include <string>
#ifdef debuginfo
inline void log_info(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

#else
inline void log_info(const std::string&) {
    // No-op in release mode
} 
#endif
#endif // Log_hpp