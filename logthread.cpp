#include "logthread.h"

LogThread::LogThread() { }

void LogThread::run()
{
    QEventLoop loop;
    qRegisterMetaType<logType>("logType");
    connect(this, &LogThread::printLog, this, &LogThread::onPrintLog);

    QDir dir;
    if(!dir.exists("log"))
        dir.mkdir("log");

    logFile = new QFile(QDir::currentPath() + "/log/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss") + ".log");
    if(logFile->open(QIODevice::WriteOnly | QIODevice::Unbuffered))
        emit printLog(logType::logThread, "The logThread started successfully.");
    else
        emit printLog(logType::logThread, "The logThread started but Log File created failed.");

    loop.exec();
}

void LogThread::onPrintLog(logType type, QString str)
{
    QString typeStr;
    switch ((int)type)
    {
    case logType::socket: typeStr = "SOCKET"; break;
    case logType::database: typeStr = "DATABASE"; break;
    case logType::logThread: typeStr = "LOGTHREAD"; break;
    case logType::threadpool: typeStr = "THREADPOOL"; break;
    }
    QString logLine = QString("[%1][%2] : %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(typeStr).arg(str);
    if(logFile->isOpen())
        logFile->write(QString(logLine + "\n").toUtf8());
    qDebug() << logLine.toUtf8().data();
}

LogThread::~LogThread()
{
    if(this->logFile != nullptr)
        delete logFile;
}
