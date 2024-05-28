#include <iostream>
#include <ctime>
#include <iomanip>

#include "Logger.h"

void Logger::Log(const std::string& message) {
    time_t now = time(0);
    auto timeinfo = std::localtime(&now);

    std::cout << "LOG | " << std::put_time(timeinfo, "%d-%b-%Y %H:%M:%S") << " - " << message << std::endl;
}

void Logger::Err(const std::string& message) {
    time_t now = time(0);
    auto timeinfo = std::localtime(&now);

    std::cout << "ERR | " << std::put_time(timeinfo, "%d-%b-%Y %H:%M:%S") << " - " << message << std::endl;
}