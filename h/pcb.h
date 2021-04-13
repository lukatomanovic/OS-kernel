/*
 * pcb.h
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include"thread.h"
#include "PCB_List.h"
#define MAX_STACK_SIZE 65536
#define MAX_SIGNAL_NO 16
/***SIGNALS****/
class Signal_List;

class SignalHandler_List;

//unsigned count;
class KernelSem;
enum State {INIT=0, RUNNING=1, BLOCKED=2, READY=3, FINISHED=4};
class PCB{
public:
	static PCB* running;
	unsigned *stack_;
	unsigned sp_;
	unsigned ss_;
	unsigned bp_;
	State status_; //unsigned flag
	Time timeSlice_;
	ID getId(){return threadID_;}
	static PCB_List *pcb_list_;
	PCB(Thread* thread, StackSize stackSize, Time timeSlice);
	~PCB();

	KernelSem * getSemBlockedOn(){return sem_blocked_on_;}
	void setSemBlockedOn(KernelSem *ks){sem_blocked_on_=ks;}
	int getIsTimeBlocked(){return isTimeBlocked_;}
	void setIsTimeBlocked(int block){isTimeBlocked_=block;}
	int getSemOrdinalNumber(){return sem_ordinal_number_;}
	void setSemOrdinalNumber(int num){sem_ordinal_number_=num;}

	void signalProcessing();
	void setPCBThread(Thread *t){pcbThread_=t;};

protected:
  friend class Thread;
  friend class IdleThread;
  void stackInitialize();
  static void wrapper();
  ID threadID_;
private:
  Thread *pcbThread_;//nit kojoj pripada pcb
  StackSize stack_size_;

  PCB_List *pcb_waiting_for_me_;

  //*****KerSem*****
  KernelSem *sem_blocked_on_;
  int isTimeBlocked_;
  int sem_ordinal_number_;

  /**********Signali*********/
  PCB* parent_;
  Signal_List* signals_;
  static unsigned next_id;
  SignalHandler_List* signal_hendlers[MAX_SIGNAL_NO];

  static void SignalHandler0();
  unsigned block_signal_flag[MAX_SIGNAL_NO];
  static unsigned block_signal_global_flag[MAX_SIGNAL_NO];
  static int inicialaze_global_signal_flag;

};


#endif /* PCB_H_ */
