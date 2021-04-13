/*
 * kersem.h
 *
 *  Created on: Apr 23, 2020
 *      Author: OS1
 */

#ifndef KERSEM_H_
#define KERSEM_H_

class PCB_List;
class KernelSem_List;

typedef unsigned int Time;

class KernelSem{
public:

	int signal(int n=0);
	void timesignal();
	int wait(Time maxTimeToWait);
	static KernelSem_List* sem_list_;
	int getVal(){return val_;}
	static void updateTimeOnAllSems();


	PCB_List *list_of_blocked_;
	PCB_List *list_of_time_blocked_;
protected:
	friend class Semaphore;
	KernelSem(int val);
	~KernelSem();

	void block(Time maxTimeToWait);
	void deblock(int timesig=0);

private:
	int val_;
	static int next_number_in_queue;

};


#endif /* KERSEM_H_ */
