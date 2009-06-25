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
	connect(checkSendTimer,SIGNAL(timeout()),this,SLOT(sendConfirm()));
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
	checkReceiveTimer->start(2000);
	// checkSendTimer->start(500);
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
	connect(newIPStruct.ConfirmTimer,SIGNAL(timeout()),this,SLOT(sendConfirm()));
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
#ifdef REN_DEBUG
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
			#ifdef FANG_DEBUG
			qWarning(QString("set the %1 to be true").arg(it.key()));
			#endif
			it.data().connection = true;
			if (it.data().ConfirmTimer->isActive())
			{
				it.data().ConfirmTimer->stop();
			}
			emit SigAddressAvailable(it.key());
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
			if (currentTime.msecsTo(it.data().recv)<-4000)
			{
				// renyang-modify - 設定沒有辦法連通
				#ifdef FANG_DEBUG
				qWarning(QString("set the %1 to be false").arg(it.key()));
				#endif
				// renyang-modify - 不在這裡設定是否有收到資料
				// it.data().connection = false;
				emit SigAddressFail(it.key());
				#ifdef FANG_DEBUG
				qWarning(QString("We don't recv any data within this time from %1").arg(it.key()));
				#endif
			}
		}
	}
}

void SctpIPHandler::sendConfirm()
{
#ifdef REN_DEBUG
	qWarning("SctpIPHandler::sendConfirm()");
#endif
	if (!IP2Info.empty())
	{
		QMap<QString,IPStruct>::Iterator it;
		for (it=IP2Info.begin();it!=IP2Info.end();++it)
		{
			// renyang-modify - 只有對每一個連線為false傳送Confirm封包
			if (it.data().connection == false)
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

void SctpIPHandler::setIPConnectable(QString IP, bool enabled)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpIPHander::setIPConnectable(%1,%2)").arg(IP).arg(enabled));
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
			it.data().connection = enabled;
			if (enabled == false)
			{
#ifdef REN_DEBUG
				qWarning(QString("%1 start timer").arg(it.key()));
#endif
				it.data().ConfirmTimer->start(10000);
			}
		}
	}
}

void SctpIPHandler::stopConfirmTimer(QString &IP)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpIPHandler::stopConfirmTimer(%1)").arg(IP));
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
			if (it.data().ConfirmTimer->isActive())
			{
				it.data().ConfirmTimer->stop();
			}
		}
	}
}
