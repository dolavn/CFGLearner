#include "Logger.h"
#include <iostream>

using namespace std;

Logger::Logger(LoggingLevel level, LoggingLevel printLevel):level(level), printLevel(printLevel){

}

void Logger::setLoggingLevel(LoggingLevel level){
    this->level = level;
}

void Logger::setPrintLevel(LoggingLevel level){
    printLevel = level;
}


int log_rank(Logger::LoggingLevel value)
{
    switch (value)
    {
        case Logger::LOG_DEBUG:
            return 1;

        case Logger::LOG_WARNING:
            return 2;

        case Logger::LOG_ERRORS:
            return 3;
    }
}

bool operator<=(Logger::LoggingLevel left, Logger::LoggingLevel right)
{ return log_rank(left) <= log_rank(right); }
