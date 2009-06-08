#ifndef SCTPIPHANDLER_H
#define SCTPIPHANDLER_H

#include <qobject.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qtimer.h>

class IPStruct
{
public:
	// renyang-modify - 記錄目前是描述哪一個ip
	QString ip;
	// renyang-modify - 記錄上一次接收資料的時間是什麼時候
	QTime	recv;
	// renyang-modify - 記錄上一次傳送資料的時間是什麼時候
	QTime	send;
	// renyang-modify - 本地端到對方是否是連通的
	bool available;
	// renyang-modify - 記錄檢查時沒有收到封包的次數
	int fail_count;
};

class SctpIPHandler:public QObject
{
	Q_OBJECT
	public:
		SctpIPHandler();
		~SctpIPHandler();
		// renyang-modify - 驗證每一個allPeerIP裡面是否有元素
		bool valid();
		// renyang-modify - 在allPeerIP裡面增加一個IPStruct
		void addIP(QString);
		// renyang-modify - 由allPeerIP中移除掉一個IPStruct
		void removeIP(QString);
		// renyang-modify - 設定某一個ip的最新sending time
		void setSendingTime(QString);
		// renyang-modify - 設定某一個ip的最新receive time
		void setRecvingTime(QString);
		// renyang-modify - 設定某一個ip, 是否可以由本地端連過去
		void setAvailable(QString,bool);
		// renyang-modify - 清除所有資料
		void end();
		IPStruct * findIP(QString);
		// renyang-modify - 開始計時啦
		void start();
		// renyang-modify - 取得目前可以使用的ip
		QString getAvailableIP();
	
	public slots:
		// renyang-modify - 檢查每一個ip是否有收到資料
		void checkReceive();
		// renyang-modify - 對每一個封包傳送資料
		void sendConfrim();
	
	signals:
		// renyang-modify - 告知這一個ip有一陣子沒有由他那裡收到ip啦
		void SigAddressConfrim(QString);
		// renyang-modify - 多次沒有收到資料, 宣告這一個ip沒有辦法使用
		void SigAddressFail(QString);
		// renyang-modify - 通知某一個ip有收到資料, 復活啦
		void SigAddressAvailable(QString);

	private:
		// renyang-modify - 存放所有IPStruct的指標
		QPtrList<IPStruct> allPeerIP;
		// renyang-modify - 每一段時間檢查是否有每一個ip都有收到資料
		QTimer *checkReceiveTimer;
		// renyang-modify - 固定時間送資料出去
		QTimer *checkSendTimer;
};

#endif
