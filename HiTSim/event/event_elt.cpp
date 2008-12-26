
#include <qobject.h>
#include "event/event_elt.h"

EventElt::EventElt()
	: p_prev(NULL)
	, p_next(NULL)
{

}
EventElt::EventElt(EventElt* p,EventElt* n)
	: p_prev(p)
	, p_next(n) 
{		
};
EventElt::~EventElt()
{
	p_prev = p_next = NULL;	// 我覺得這一行不需要,反正這一個物件刪除,其指標變數也會刪除
}
