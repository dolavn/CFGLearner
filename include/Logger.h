//
// Created by dolavn on 9/4/19.
//

#ifndef CFGLEARNER_LOGGER_H
#define CFGLEARNER_LOGGER_H


#include <iostream>

class Logger{
public:
    enum LoggingLevel{
        LOG_ERRORS,
        LOG_WARNING,
        LOG_DEBUG
    };
    Logger(LoggingLevel, LoggingLevel);

    void setLoggingLevel(LoggingLevel);
    void setPrintLevel(LoggingLevel);

    template<typename T>
    Logger& operator<< (const T& data)
    {
        if(level<=printLevel){
            std::cout << data << std::endl;
        }
    }
private:
    LoggingLevel level;
    LoggingLevel printLevel;
};

#endif //CFGLEARNER_LOGGER_H
