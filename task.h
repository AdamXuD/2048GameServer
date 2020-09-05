#ifndef TASK_H
#define TASK_H

#include "common.h"

class Task : public QRunnable
{
    std::function<void()> _task;
    std::function<void(QString content)> _log;

public:
    Task(std::function<void()> task, std::function<void(QString content)> log);
    ~Task();

protected:
    void run();


protected:

signals:

};

#endif // TASK_H
