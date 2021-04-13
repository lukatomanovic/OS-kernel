/*
 * event.h
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

// File: event.h
#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
//class KernelEv;
#include"IVTEntry.h"//zbog javnog testa
#include "KernelEv.h"//zbog javnog testa

class Event {

public:
	Event (IVTNo ivtNo);
	~Event ();
	void wait  ();

protected:
	friend class KernelEv;
	void signal();

private:
	KernelEv* myImpl;


};


#endif /* EVENT_H_ */
