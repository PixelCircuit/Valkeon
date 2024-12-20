#pragma once

#include <iostream>
#include <string>

inline void logError(const std::string &message) {
  std::cerr << "Error: " << message << std::endl;
}

inline void logInfo(const std::string &message) {
  std::cout << "Info: " << message << std::endl;
}