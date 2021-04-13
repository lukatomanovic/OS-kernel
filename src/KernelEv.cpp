/*
 * KernelEv.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

#include"locking.h"
#include"KernelEv.h"
#include"pcb.h"
#include"SCHEDULE.H"
#include <stdio.h>

KernelEv::KernelEv(IVTEntry *entry) :
	waiting_thread_(0), val(0),owner_(0),myEntry(entry) {
}

KernelEv::~KernelEv() {
	LOCK
	if (waiting_thread_ != 0){
		if (waiting_thread_->status_ != FINISHED) {
			waiting_thread_->status_ = READY;
			Scheduler::put (waiting_thread_);
		}
	}
	UNLOCK
}

void KernelEv::wait() {
	LOCK
	if (val == 0) {
		PCB::running->status_=BLOCKED;
		waiting_thread_ = PCB::running;
		UNLOCK
		dispatch();
		return;
	} else if (val == 1) {
		val = 0;
	}
	UNLOCK
}


void KernelEv::signal() {
	LOCK
	if (waiting_thread_ != 0) {
		if (waiting_thread_->status_ != FINISHED) {
			waiting_thread_->status_ = READY;
			Scheduler::put (waiting_thread_);
		}
		waiting_thread_ = 0;
		val = 0;

	} else {
		val = 1;
	}
	UNLOCK
	//moze se raditi promena konteksta i ne mora, doci ce na red svakako i ona
}

PCB* KernelEv::getOwner(){
	return owner_;
}

void KernelEv::setOwner(PCB *running){
	owner_=PCB::running;
}





