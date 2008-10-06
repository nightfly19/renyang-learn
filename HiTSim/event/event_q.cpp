
#include "qobject.h"
#include "event/event_q.h"


EventQ::EventQ()
	: p_head(NULL)
	, p_tail(NULL)
	, size(0)
{
}

EventQ::~EventQ()
{
	Event* tmp = 0;
	// delete all q
	if(p_head == NULL || p_tail == NULL)	return;

	while(p_head != NULL){
		tmp = (Event*)p_head -> Next();
		if( p_head == p_tail )
			p_tail = NULL;
		delete p_head; 
	}
}

void EventQ::InsertTail(Event* e)
{
	size ++;
	if(p_head == NULL && p_tail == NULL){
		p_head = e;
		p_tail = e;
		return;
	} /*else if(p_head == NULL xor p_tail == NULL){
		// some error happend @@
		return;
	}*/
	
	//debug("real insert event hdltime = %e",e->HdlTime()); // Insert
	p_tail	->setNext( e );
	e	->setPrev( p_tail );
	p_tail	= e;

	return; 
}
void EventQ::InsertHead(Event* e)
{
	size ++;
	if(p_head == NULL && p_tail == NULL){
		p_head = e;
		p_tail = e;
		return;
	}
	/*
	if(p_head == NULL || p_tail == NULL){
		// some error happend @@
		return;
	}
	*/
	
	// Insert
	p_head	->setPrev( e );
	e	->setNext( p_head );
	p_tail	= e;

	return; 
}

void EventQ::InsertByTime(Event*)
{
	debug("EventQ::InsertByTime - not implement yet");
	// not impl
	size ++;
}

Event* EventQ::RemoveHead()
{
	if( p_head == NULL)	return NULL;
	size --;

	Event* tmp = p_head;
	p_head =  (Event*) p_head -> Next();
	if( p_head == NULL)
		p_tail = NULL;
	
	tmp -> setNext(NULL);
	tmp -> setPrev(NULL);

	return tmp;
}

Event* EventQ::RemoveTail()
{
	if( p_tail == NULL )	return NULL;
	size --;

	Event* tmp = p_tail;
	p_tail = (Event*) p_tail -> Prev();

	if( p_tail == NULL)
		p_head = NULL;
	
	tmp -> setNext(NULL);
	tmp -> setPrev(NULL);

	return tmp;
}

unsigned int EventQ :: Size()
{
	return size;
}

bool EventQ :: CheckQ()
{
	int s = 0;
	if( p_tail == NULL|| p_head == NULL )
		return true;
	if(p_tail == NULL && p_head )	return false;
	if(p_head == NULL && p_tail)	return false;
	

	Event *tmp = p_head;
	s ++;
	while( tmp != p_tail ){
		tmp = (Event*) tmp -> Next();
		if(tmp == NULL)
			return false;
		s ++;
	}
	//debug("size = %d",s);
	return true;
}
