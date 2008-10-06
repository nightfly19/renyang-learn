
#ifndef __EVENT_Q_H__
#define __EVENT_Q_H__

#include "event/event.h"

class EventQ
{
public:
	EventQ();
	~EventQ();

	bool hasEvent(){	return p_head?true:false;	};
	bool NoEvent(){	return p_head?false:true;	};

	Event* Head(){	return p_head;	};

/*	Insert Implementation	*/
	void InsertTail(Event*);
	void InsertHead(Event*);

	void InsertByTime(Event*);

/*	Remove Implementation	*/
/*	記住..Remove From Q but not delete	*/
	Event*	RemoveTail();
	Event*	RemoveHead();

	unsigned int Size();

public: // for debugging
	bool CheckQ();

private:
	Event* p_head;
	Event* p_tail;
	unsigned int size;
};


#endif
