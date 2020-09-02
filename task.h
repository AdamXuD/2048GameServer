#ifndef TASK_H
#define TASK_H


class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(QObject *parent = nullptr);

signals:

};

#endif // TASK_H
