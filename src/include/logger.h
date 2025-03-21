#ifndef MNRPC_LOGGER_H
#define MNRPC_LOGGER_H

#include "lockqueue.h"
#include <string>

enum loglevel{
    INFO, //普通消息 General message
    ERROR, // 错误信息 error message
};

class clogger{
public:
    void setLogLevel(loglevel level);
    void log(std::string msg);

    static clogger& getInstance();

private:
    int m_loglevel;
    cLockQueue<std::string> m_lckQue;

    clogger();
    clogger(const clogger&) = delete;
    clogger(clogger&&) = delete;


};

// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do                              \
    {                               \
        clogger &logger = clogger::getInstance(); \
        logger.setLogLevel(INFO);   \
        char c[1024] = {0};         \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.log(c);\
    } while (0);

#define LOG_ERR(logmsgformat, ...) \
    do                              \
    {                               \
        clogger &logger = clogger::getInstance(); \
        logger.setLogLevel(ERROR);   \
        char c[1024] = {0};         \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.log(c);\
    } while (0);
#endif //MNRPC_LOGGER_H
