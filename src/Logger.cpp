#include "Logger.h"
#include <iostream>
#include <sstream>
#include <execinfo.h>

using namespace std;

Logger::Logger(LoggingLevel level, LoggingLevel printLevel):level(level), printLevel(printLevel){

}

void Logger::setLoggingLevel(LoggingLevel level){
    this->level = level;
}

void Logger::setPrintLevel(LoggingLevel level){
    printLevel = level;
}

string Logger::getCaller() {
    void *array[2];
    stringstream stream;
    size_t size;
    // get void*'s for all entries on the stack
    return stream.str();
}


int log_rank(Logger::LoggingLevel value)
{
    switch (value)
    {
        case Logger::LOG_DEBUG:
            return 4;
        case Logger::LOG_DETAILS:
            return 3;
        case Logger::LOG_WARNING:
            return 2;
        case Logger::LOG_ERRORS:
            return 1;
    }
}

bool operator<=(Logger::LoggingLevel left, Logger::LoggingLevel right)
{
    return log_rank(left) <= log_rank(right); }

