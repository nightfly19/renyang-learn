
#ifndef __EVT_ELT_H__
#define __EVT_ELT_H__


class EventElt
{
public:	
	EventElt();
	EventElt(EventElt* p,EventElt* n);
	virtual ~EventElt();

	virtual EventElt* Prev(){	return p_prev;	};
	virtual EventElt* Next(){	return p_next;	};
	virtual void setPrev( EventElt* p ){	p_prev = p;	};
	virtual void setNext( EventElt* n ){	p_next = n;	};

private:
	EventElt* p_prev;
	EventElt* p_next;


};


#endif

