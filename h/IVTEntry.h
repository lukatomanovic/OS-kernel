/*
 * IVTEntry.h
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef unsigned char IVTNo;
typedef void interrupt (*ptrInterrupt)(...);
class KernelEv;


class IVTEntry
{
	friend class KernelEv;
public:
	IVTEntry(IVTNo no, ptrInterrupt ptrNewInter);
	~IVTEntry();
	void callOld();
	KernelEv* getMyKernelEvent();
	static KernelEv* getEntryEvent(IVTNo no);
	static IVTEntry* IVT[256];
private:
	KernelEv *myKernelEvent;
	IVTNo ivt_no_;
	ptrInterrupt ptrOldInter;
};

#define PREPAREENTRY(ivtEntryNo, callOldF)\
	void interrupt inter##ivtEntryNo(...);\
	IVTEntry ivtEntry##ivtEntryNo(ivtEntryNo, inter##ivtEntryNo);\
	void interrupt inter##ivtEntryNo(...) {\
		ivtEntry##ivtEntryNo.getMyKernelEvent()->signal();\
		if (callOldF == 1)\
	    	ivtEntry##ivtEntryNo.callOld();\
	}\




#endif /* IVTENTRY_H_ */
