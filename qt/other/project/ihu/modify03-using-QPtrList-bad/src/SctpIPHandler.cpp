#include "SctpIPHandler.hpp"

SctpIPHandler::SctpIPHandler()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::SctpIPHandler()");
#endif
	checkReceiveTimer = new QTimer(this);
	checkSendTimer = new QTimer(this);
}

SctpIPHandler::~SctpIPHandler()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::~SctpIPHandler()");
#endif
	end();
}

void SctpIPHandler::start()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::start()");
#endif
	checkReceiveTimer->start(1000);
	checkSendTimer->start(500);
	connect(checkReceiveTimer,SIGNAL(timeout()),this,SLOT(checkReceive()));
	connect(checkSendTimer,SIGNAL(timeout()),this,SLOT(sendConfrim()));
}

bool SctpIPHandler::valid()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::valid()");
#endif
	if (allPeerIP.count()>0)
		return true;
	return false;
}

void SctpIPHandler::addIP(QString IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::addIP(%1)").arg(IP));
#endif
	// renyang-modify - 建立一個struct
	IPStruct *newIPStruct = new IPStruct();
	// renyang-modify - 初始化ip
	newIPStruct->ip = IP;
	// renyang-modify - 初始化available
	newIPStruct->available = false;
	// renyang-modify - 初始化檢查時沒有收到封包的次數
	newIPStruct->fail_count = 0;

	// renyang-modify - 這裡可能還要初始化recv與send
	newIPStruct->recv.start();

	// renyang-modify - 加入allPeerIP中
	allPeerIP.append(newIPStruct);
	qWarning("the count is %d",allPeerIP.count());
}

void SctpIPHandler::end()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::end()");
#endif
	if (valid())
	{
		allPeerIP.first();
		for (unsigned int i=0;i<allPeerIP.count();i++)
		{
			// renyang-modify - 釋放記憶體位置
			free(allPeerIP.current());
			// renyang-modify - 把指標由QPtrList中移除掉
			allPeerIP.remove();
		}
	}
}

void SctpIPHandler::removeIP(QString IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::removeIP(%1)").arg(IP));
#endif
	IPStruct * currentIPStruct;
	currentIPStruct = findIP(IP);
	if (currentIPStruct != NULL)
	{
		delete currentIPStruct;
		allPeerIP.remove(currentIPStruct);
	}
}

IPStruct * SctpIPHandler::findIP(QString IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::findIP(%1)").arg(IP));
#endif
	if (valid())
	{
		allPeerIP.first();
		for (unsigned int i=0;i<allPeerIP.count();i++,allPeerIP.next())
		{
			if (allPeerIP.current()->ip == IP)
			{
				return allPeerIP.current();
			}
		}
	}
	return NULL;
}

void SctpIPHandler::setSendingTime(QString IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::setSendingTime(%1)").arg(IP));
#endif
	IPStruct *currentIPStruct;
	currentIPStruct = findIP(IP);
	if (currentIPStruct != NULL)
	{
		currentIPStruct->send.restart();
	}
}

void SctpIPHandler::setRecvingTime(QString IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::setRecvingTime(%1)").arg(IP));
#endif
	IPStruct *currentIPStruct = findIP(IP);
	if (currentIPStruct != NULL)
	{
		currentIPStruct->recv.restart();
		currentIPStruct->fail_count = 0;
		if (currentIPStruct->available == false)
		{
			setAvailable(currentIPStruct->ip,true);
			emit SigAddressAvailable(currentIPStruct->ip);
		}
	}
}

void SctpIPHandler::setAvailable(QString IP, bool enable)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::setAvailable(%1,%2)").arg(IP).arg(enable));
#endif
	IPStruct *currentIPStruct = findIP(IP);
	if (currentIPStruct != NULL)
	{
		currentIPStruct->available = enable;
	}
}

void SctpIPHandler::checkReceive()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::checkReceive()");
#endif
	if (valid())
	{
		QTime currentTime = QTime::currentTime();
		allPeerIP.first();
		for (unsigned int i=0;i<allPeerIP.count();i++,allPeerIP.next())
		{
			if (currentTime.msecsTo(allPeerIP.current()->recv)<-1000)
			{
				qWarning(QString("%1 don't receive any data within times").arg(allPeerIP.current()->ip));
				allPeerIP.current()->fail_count++;
				if (allPeerIP.current()->fail_count >= 2)
				{
					// renyang-modify - 只有當原本是可以連線的情況下
					// renyang-modify - 在兩次沒有收到封包時, 才會設定false
					if (allPeerIP.current()->available == true)
					{
						qWarning("want to disable the address");
						// renyang-modify - 多次沒有收到資料, 宣告這一個ip沒有辦法使用
						// setAvailable(allPeerIP.current()->ip,false);
						allPeerIP.current()->available = false;
						allPeerIP.current()->fail_count = 0;
						emit SigAddressFail(allPeerIP.current()->ip);
						qWarning("Check next");
					}
				}
			}
			qWarning("try the next");
		}
	}
}

QString SctpIPHandler::getAvailableIP()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::getAvailableIP()");
#endif
	if (valid())
	{
		allPeerIP.first();
		for (unsigned int i=0;i<allPeerIP.count();i++,allPeerIP.next())
		{
			if (allPeerIP.current()->available)
			{
				return allPeerIP.current()->ip;
			}
		}
	}
	return QString("No_available_IP");
}

void SctpIPHandler::sendConfrim()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::sendConfirm()");
#endif
	if (valid())
	{
		allPeerIP.first();
		// renyang-modify - 對所有的peer端的ip傳送confirm封包
		for (unsigned int i=0;i<allPeerIP.count();i++,allPeerIP.next())
		{
			emit SigAddressConfrim(allPeerIP.current()->ip);
		}
	}
}
