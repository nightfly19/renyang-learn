#include <iostream>
using namespace std;

#include "thread.h"

Thread::Thread()
{
    stopped = false;
}

void Thread::setMessage(const QString &message)
{
    messageStr = message;
}

void Thread::run()
{
    while (!stopped)
        cerr << messageStr.ascii();
    stopped = false;
    cerr << endl;
}

void Thread::stop()
{
    stopped = true;
}
