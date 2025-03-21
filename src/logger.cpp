#include "logger.h"
#include <time.h>
#include <iostream>

clogger &clogger::getInstance() {
    static clogger logger;
    return logger;
}

clogger::clogger() {
    // 启动写日志线程
    std::thread writeLogTask([&](){
        for(;;){
            // 获取当前日期，获取日志信息，写入日志中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if(pf== nullptr){
                std::cout<<"logger file:"<<file_name<<" open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec, (m_loglevel == INFO ? "info":"error"));
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });

    writeLogTask.detach();


}

void clogger::setLogLevel(loglevel level) {
    m_loglevel = level;
}

void clogger::log(std::string msg) {
    m_lckQue.push(msg);
}