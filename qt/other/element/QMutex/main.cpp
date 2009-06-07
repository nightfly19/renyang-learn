#include <qapplication.h>
#include <qpushbutton.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <qthread.h>

// 全局条件变量
QWaitCondition mycond;

// Worker类实现
class Worker : public QPushButton, public QThread
{
    Q_OBJECT

public:
    Worker(QWidget *parent = 0, const char *name = 0)
        : QPushButton(parent, name)
    {
        setText("Start Working");

        // 连接从QPushButton继承来的信号和我们的slotClicked()方法
        connect(this, SIGNAL(clicked()), SLOT(slotClicked()));

        // 调用从QThread继承来的start()方法……这将立即开始线程的执行
        QThread::start();
    }

public slots:
    void slotClicked()
    {
        // 唤醒等待这个条件变量的一个线程
        mycond.wakeOne();
    }

protected:
    void run()
    {
        // 这个方法将被新创建的线程调用……

        while ( TRUE ) {
            // 锁定应用程序互斥锁，并且设置窗口标题来表明我们正在等待开始工作
            qApp->lock();
            setCaption( "Waiting" );
            qApp->unlock();

            // 等待直到我们被告知可以继续
            mycond.wait();

            // 如果我们到了这里，我们已经被另一个线程唤醒……让我们来设置标题来表明我们正在工作
            qApp->lock();
            setCaption( "Working!" );
            qApp->unlock();

            // 这可能会占用一些时间，几秒、几分钟或者几小时等等，因为这个一个和GUI线程分开的线程，在处理事件时，GUI线程不会停下来……
            // do_complicated_thing();
        }
    }
};

	// 主线程——所有的GUI事件都由这个线程处理。
int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    // 创建一个worker……当我们这样做的时候，这个worker将在一个线程中运行
    Worker firstworker( 0, "worker" );

    app.setMainWidget( &firstworker );
    firstworker.show();

    return app.exec();
}
