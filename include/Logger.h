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
        LOG_DETAILS,
        LOG_DEBUG
    };
    static Logger& getLogger(){
        static Logger log(LOG_DEBUG, LOG_ERRORS);
        return log;
    }

    Logger(const Logger&)=delete;
    Logger& operator=(const Logger&)=delete;
    void setLoggingLevel(LoggingLevel);
    void setPrintLevel(LoggingLevel);


    template<typename T>
    Logger& operator<< (const T& data)
    {
        if(level<=printLevel){
            std::cout << " " << getCaller() << data;
        }
        return *this;
    }

    const char endline = '\n';
private:
    Logger(LoggingLevel, LoggingLevel);
    std::string getCaller();
    LoggingLevel level;
    LoggingLevel printLevel;
};


#endif //CFGLEARNER_LOGGER_H
