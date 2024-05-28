#include "Logger.h"

#include <ctime>
#include <iomanip>
#include <iostream>

const std::string kRed = "\033[31m";
const std::string kReset = "\033[0m";

void Logger::Log(const std::string& message) {
    time_t now = time(0);
    auto timeinfo = std::localtime(&now);

    std::cout << "LOG | " << std::put_time(timeinfo, "%d-%b-%Y %H:%M:%S") << " - " << message << std::endl;
}

void Logger::Err(const std::string& message) {
    time_t now = time(0);
    auto timeinfo = std::localtime(&now);

    std::cout << kRed <<"ERR | " 
            << std::put_time(timeinfo, "%d-%b-%Y %H:%M:%S") 
            << " - " << message << kReset << std::endl;
}