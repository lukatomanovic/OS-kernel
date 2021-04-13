/*
 * IVTEntry.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */
#include "IVTEntry.h"
#include "KernelEv.h"
#include <dos.h>
#include "locking.h"

IVTEntry* IVTEntry::IVT[256];

IVTEntry::IVTEntry(IVTNo no, ptrInterrupt ptrNewInter) : ivt_no_(no) {
	LOCK
#ifndef BCC_BLOCK_IGNORE
	ptrOldInter =getvect(no);
	setvect(no, ptrNewInter);
#endif
	myKernelEvent = new KernelEv(this);
	IVT[no] = this;
	UNLOCK
}

IVTEntry::~IVTEntry() {
	LOCK
#ifndef BCC_BLOCK_IGNORE
	setvect(ivt_no_, ptrOldInter);
#endif
	IVT[ivt_no_] = 0;
	delete myKernelEvent;
	UNLOCK
}

void IVTEntry::callOld() { ptrOldInter(); }

KernelEv* IVTEntry::getMyKernelEvent() { return myKernelEvent; }

KernelEv* IVTEntry::getEntryEvent(IVTNo no) { return IVT[no]->myKernelEvent; }





