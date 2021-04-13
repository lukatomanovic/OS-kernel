/*
 * thread.h
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */

#ifndef THREAD_H_
#define THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;

class PCB; // Kernel's implementation of a user's thread


typedef void (*SignalHandler)();
typedef unsigned SignalId;

class Thread {
public:
	Thread(PCB *pcb);
	void start();
	void waitToComplete();
	virtual ~Thread();
	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);


/*****Signals*********/
	PCB* myPCB;//vrati u private
	 void signal(SignalId signal);
	 void registerHandler(SignalId signal, SignalHandler handler);
	 void unregisterAllHandlers(SignalId id);
	 void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	 void blockSignal(SignalId signal);
	 static void blockSignalGlobally(SignalId signal);
	 void unblockSignal(SignalId signal);
	 static void unblockSignalGlobally(SignalId signal);

protected:
	friend class PCB;
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {}
private:
	friend class IdleThread;//ja sam ubacio zbog beskonacne niti
	void systemSignals12(SignalId signal);//signals 1 and 2 are used by system to notify parent and current thread

};
void dispatch();

#endif /* THREAD_H_ */
