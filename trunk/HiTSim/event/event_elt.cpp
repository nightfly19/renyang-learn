
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
	p_prev = p_next = NULL;
}
