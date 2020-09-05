#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include "common.h"

class LogThread : public QThread
{
    Q_OBJECT
    QFile *logFile = nullptr;

public:
    LogThread();
    ~LogThread();

private:
    void onPrintLog(logType type, QString str);

protected:
    void run();

signals:
    void printLog(logType type, QString str);
};

#endif // LOGTHREAD_H
/*
 * 已存在bug:
 * 1. 无法打开文件（在主进程的socket也有这个毛病）
*/
