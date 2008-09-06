#include <qsemaphore.h>
#include <qthread.h>

#include <cstdlib>
#include <iostream>
using namespace std;

const int DataSize = 100000;
const int BufferSize = 4096;
char buffer[BufferSize];

QSemaphore freeSpace(BufferSize);
QSemaphore usedSpace(BufferSize);

void acquire(QSemaphore &semaphore)
{
    semaphore++;
}

void release(QSemaphore &semaphore)
{
    semaphore--;
}

class Producer : public QThread
{
public:
    void run();
};

void Producer::run()
{
    for (int i = 0; i < DataSize; ++i) {
        acquire(freeSpace);
        buffer[i % BufferSize] = "ACGT"[(uint)rand() % 4];
        release(usedSpace);
    }
}

class Consumer : public QThread
{
public:
    void run();
};

void Consumer::run()
{
    for (int i = 0; i < DataSize; ++i) {
        acquire(usedSpace);
        cerr << buffer[i % BufferSize];
        release(freeSpace);
    }
    cerr << endl;
}

int main()
{
    usedSpace += BufferSize;

    Producer producer;
    Consumer consumer;
    producer.start();
    consumer.start();
    producer.wait();
    consumer.wait();
    return 0;
}
