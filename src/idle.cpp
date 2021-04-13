/*
 * idle.cpp
 *
 *  Created on: Apr 27, 2020
 *      Author: OS1
 */


#include "idle.h"
#include"pcb.h"
#include "locking.h"

IdleThread::IdleThread():Thread(defaultStackSize,1){
	LOCK
	doingsomething=1;
	myPCB->stackInitialize();
	UNLOCK
}

void IdleThread::run(){
	while(doingsomething);
}

void IdleThread::start(){
	LOCK
		myPCB->status_=READY;
	UNLOCK
}
void IdleThread::stop(){
	LOCK
	doingsomething=0;
	UNLOCK
}

PCB* IdleThread::getIdlePCB(){
	return myPCB;
}
