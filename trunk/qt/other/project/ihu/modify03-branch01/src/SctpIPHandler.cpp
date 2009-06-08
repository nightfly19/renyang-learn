#include "SctpIPHandler.hpp"

SctpIPHandler::SctpIPHandler()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::SctpIPHandler()");
#endif
	// renyang-modify - 固定時間檢查是否有在時間內收到資料
	checkReceiveTimer = new QTimer(this);
	connect(checkReceiveTimer,SIGNAL(timeout()),this,SLOT(checkReceive()));
	// renyang-modify - 固定時間內傳送資料給所有的peer address
	checkSendTimer = new QTimer(this);
	connect(checkSendTimer,SIGNAL(timeout()),this,SLOT(sendConfrim()));
}

SctpIPHandler::~SctpIPHandler()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::~SctpIPHandler()");
#endif
	end();
}

void SctpIPHandler::start()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::start()");
#endif
	checkReceiveTimer->start(1000);
	checkSendTimer->start(500);
}

void SctpIPHandler::addIP(QString &IP)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpIPHandler::addIP(%1)").arg(IP));
#endif
	IPStruct newIPStruct;
	IP2Info[IP] = newIPStruct;
	// renyang-modify - 開始計時
	newIPStruct.recv.start();
	// renyang-modify - 預設不連通
	newIPStruct.connection = false;
	qWarning("the count is %d",IP2Info.count());
}

void SctpIPHandler::removeIP(QString &IP)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpIPHandler::removeIP(%1)").arg(IP));
#endif
	if (!IP2Info.empty())
	{
		QMap<QString,IPStruct>::Iterator it;
		if ((it=IP2Info.find(IP)) == IP2Info.end())
		{
			qWarning("there is no match IP");
		}
		else
		{
			IP2Info.remove(it);
		}
	}
}

void SctpIPHandler::setRecvingTime(QString &IP)
{
#ifdef FANG_DEBUG
	qWarning(QString("SctpIPHandler::setRecvingTime(%1)").arg(IP));
#endif
	if (!IP2Info.empty())
	{
		QMap<QString,IPStruct>::Iterator it;
		if ((it=IP2Info.find(IP)) == IP2Info.end())
		{
			qWarning("there is no match IP");
		}
		else
		{
			it.data().recv.restart();
			if (it.data().connection == false)
			{
				qWarning(QString("set the %1 to be true").arg(it.key()));
				it.data().connection = true;
				emit SigAddressAvailable(it.key());
			}
		}
	}
}

void SctpIPHandler::end()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::end()");
#endif
	if (!IP2Info.empty())
	{
		IP2Info.clear();
	}
	// renyang-modify - 停止檢查是否有定時收到封包
	checkReceiveTimer->stop();
	// renyang-modify - 停止固定時間傳送封包
	checkSendTimer->stop();
}

void SctpIPHandler::checkReceive()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::checkReceive()");
#endif
	if (!IP2Info.empty())
	{
		QTime currentTime = QTime::currentTime();
		QMap<QString,IPStruct>::Iterator it;
		for (it=IP2Info.begin();it!=IP2Info.end();++it)
		{
			if (currentTime.msecsTo(it.data().recv)<-3000)
			{
				// renyang-modify - 設定沒有辦法連通
				qWarning(QString("set the %1 to be false").arg(it.key()));
				it.data().connection = false;
				emit SigAddressFail(it.key());
				qWarning(QString("We don't recv any data within this time from %1").arg(it.key()));
			}
		}
	}
}

void SctpIPHandler::sendConfrim()
{
#ifdef FANG_DEBUG
	qWarning("SctpIPHandler::sendConfrim()");
#endif
	if (!IP2Info.empty())
	{
		QMap<QString,IPStruct>::Iterator it;
		for (it=IP2Info.begin();it!=IP2Info.end();++it)
		{
			emit SigAddressConfrim(it.key());
		}
	}
}

// renyang-modify - 尋找一個目前是連通的ip
QString SctpIPHandler::getAvailableIP()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::getAvailableIP()");
#endif
	if (!IP2Info.empty())
	{
		QMap<QString,IPStruct>::Iterator it;
		for (it=IP2Info.begin();it!=IP2Info.end();++it)
		{
			qWarning(QString("%1 is %2").arg(it.key()).arg(it.data().connection));
			if (it.data().connection == true)
			{
				return it.key();
			}
		}
	}
	return QString("NO_AVAILABLE_IP");
}
