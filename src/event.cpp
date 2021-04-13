/*
 * Event.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

#include "event.h"
#include "KernelEv.h"
#include"IVTEntry.h"
#include "pcb.h"

Event::Event(IVTNo ivtNo) {
	myImpl = IVTEntry::getEntryEvent(ivtNo);
	myImpl->setOwner(PCB::running);
}

void Event::wait() {
	if (myImpl->getOwner()==PCB::running)
		myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}

Event::~Event() {}

