/*
 * Semaphore.h
 *
 *  Created on: Apr 23, 2020
 *      Author: OS1
 */

// File: semaphor.h
#ifndef _semaphor_h_
#define _semaphor_h_

typedef unsigned int Time;

class KernelSem;

class Semaphore {

public:

	Semaphore (int init=1);
	virtual ~Semaphore ();
	virtual int wait (Time maxTimeToWait);
	virtual int signal(int n=0);
	int val () const;  // Returns the current value of the semaphore


	KernelSem* myImpl;//vratiti
private:
	//KernelSem* myImpl;

};

#endif
