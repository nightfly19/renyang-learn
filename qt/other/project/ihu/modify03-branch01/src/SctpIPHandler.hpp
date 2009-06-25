#ifndef SCTPIPHANDLER_H
#define SCTPIPHANDLER_H

#include <qobject.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qtimer.h>

class IPStruct
{
	public:
		IPStruct()
		{
			ConfirmTimer = new QTimer();
		}
		// renyang-modify - 記錄上一次接收資料的時間是什麼時候
		QTime recv;
		// renyang-modify - 記錄上次傳送資料的時間是什麼時候
		QTime send;
		// renyang-modify - 本地端到對方是否是連通的
		bool connection;
		QTimer *ConfirmTimer;
};

class SctpIPHandler:public QObject
{
	Q_OBJECT
	public:
		SctpIPHandler();
		~SctpIPHandler();
		// renyang-modify - 在IP2Info新增一個mapping
		void addIP(QString &);
		// renyang-modify - 在IP2Info刪除一個mapping
		void removeIP(QString &);
		// renyang-modify - 當收到資料時, 則更新receive time
		void setRecvingTime(QString &);
		void start();
		void end();
		QString getAvailableIP();
		void setIPConnectable(QString, bool);
		// renyang-modify - 停止每一個Confirm計時的timer
		void stopConfirmTimer(QString&);

	public slots:
		// renyang-modify - 固定時間檢查是否有在時間內接收到資料
		void checkReceive();
		// renyang-modify - 固定時間傳送封包
		void sendConfirm();

	signals:
		// renyang-modify - 要求傳送某一個確認封包給某一個address
		void SigAddressConfrim(QString);
		// renyang-modify - 某ip與本機端沒有連接
		void SigAddressFail(QString);
		// renyang-modify - 宣告某一個ip與本機端是有連接的
		void SigAddressAvailable(QString);
	
	private:
		// renyang-modify - 由IP可以得到其接收到的傳送時間與接收時間的資料
		QMap<QString,IPStruct> IP2Info;
		// renyang-modify - 每一個固定時間，檢查是否有接收到資料
		QTimer *checkReceiveTimer;
		// renyang-modify - 定時送封包給所有的peer端ip
		QTimer *checkSendTimer;
};

#endif
