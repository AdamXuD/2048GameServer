#include "task.h"

Task::Task(std::function<void()> task, std::function<void(QString content)> log)
{
    setAutoDelete(true);
    this->_task = task;
    this->_log = log;
}

Task::~Task() { }

void Task::run()
{
    _log("A task is running at thread : " + QString("%1").arg((uintptr_t)QThread::currentThread()));
    return this->_task();
}
