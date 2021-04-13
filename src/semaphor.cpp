/*
 * Semaphore.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */


#include"locking.h"
#include "kersem.h"
#include<stdio.h>

#include "semaphor.h"

Semaphore::Semaphore(int init) {
#ifndef BCC_BLOCK_IGNORE
		asm {cli}
#endif
		myImpl = new KernelSem(init);
#ifndef BCC_BLOCK_IGNORE
	asm {sti}
#endif
}

Semaphore::~Semaphore(){
	LOCK
	delete myImpl;
	UNLOCK
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::val() const {
	return myImpl->getVal();
}
