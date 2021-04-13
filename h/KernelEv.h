/*
 * KernelEv.h
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_


class PCB;

class KernelEv
{
public:
	friend class IVTEntry;
	KernelEv(IVTEntry *entry);
	void wait();
	void signal();
	~KernelEv();
	PCB* getOwner();
	void setOwner(PCB *running);
	//static KernelEv* getKernelEv(IVTNo ivtNo);
private:
	IVTEntry*myEntry;
	int val;
	PCB* waiting_thread_;
	PCB* owner_;
};



#endif /* KERNELEV_H_ */
