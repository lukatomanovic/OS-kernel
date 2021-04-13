/*
 * thread.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */


#include "pcb.h"
#include "SCHEDULE.H"
#include "locking.h"
#include<stdio.h>
#include<stdlib.h>
#include"Sig_List.h"
#include "SigH_Lst.h"

Thread::Thread(PCB *pcb){
	myPCB=pcb;
}

Thread::Thread (StackSize stackSize, Time timeSlice){
  myPCB = new PCB(this, stackSize, timeSlice);
}

Thread::~Thread()
{
	waitToComplete();
	LOCK
	delete myPCB;
	UNLOCK
}

void Thread::start() {

	if (myPCB->status_ != INIT) //stiti od visestrukog pokretanja niti
		return;
	LOCK

	myPCB->stackInitialize();
	//zakljucava se da ne bi doslo do promene konteksta u sred inicijalizacije niti, a i put ne znam kako je implementiran, inicijalizacija steka moze da se izvuce napolje
	myPCB->status_ = READY;
	Scheduler::put(myPCB); //KAD DODAM BESKONACNU NIT, OGRANICITI DA NE IDE U SCHEDULER

	UNLOCK

}

void Thread::waitToComplete(){//da li moze doci do promene konteksta u sred poziva waitToComplete
	LOCK//pre if-a zbog stanja finished, ako dodje do pomene nakon ispitivanja, a nit u medjuvremenu zavrsi
	if ( PCB::running != this->myPCB && myPCB->status_ != FINISHED){//mozda treba iskljuciti i beskonacnu nit kada nema niti u sistemu
		PCB::running->status_=BLOCKED;
		myPCB->pcb_waiting_for_me_->add(PCB::running);
		UNLOCK
		dispatch();
		return;
	}
	UNLOCK
}

ID Thread::getId(){
  return myPCB->getId();
}
ID Thread::getRunningId () { return PCB::running->getId(); }//ako dodje do promene konteksta, nema problema, jer ce se ovo nastaviti tek kad se running opet vrati

Thread* Thread::getThreadById(ID id) {
	PCB *tmp=PCB::pcb_list_->findById(id);//vraca 0 ako ne nadje
	if(tmp!=0)
		return tmp->pcbThread_;
	return 0;
}


void dispatch (){
	asm{cli}
	dispatch_on_request=1;
	asm{
		int 8h; //zove se timer rutina
	}
	asm{sti}
}



/******************SIGNALS**************************/
void Thread::signal(SignalId signal){
	if (signal>=0&&signal < MAX_SIGNAL_NO && signal != 1 && signal != 2){//SIGNALS 1 AND 2 ARE USED ONLY BY SYSTEM
		myPCB->signals_->insert(signal);
	}
}
void Thread::systemSignals12(SignalId signal){
	if (signal == 1 || signal == 2){//check just to be sure that call is correct
		myPCB->signals_->insert(signal);
	}
}
void Thread::registerHandler(SignalId signal, SignalHandler handler){
	if (signal>=0&&signal < MAX_SIGNAL_NO)
		myPCB->signal_hendlers[signal]->insert(handler);
}
void Thread::unregisterAllHandlers(SignalId id) {
	if (id >= 0 && id < MAX_SIGNAL_NO && myPCB->signal_hendlers[id] != 0) {
		delete myPCB->signal_hendlers[id];
		myPCB->signal_hendlers[id] = new SignalHandler_List(); //da bude spremno za nova ubacivanje
	}
}
void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	if (id >= 0 && id < MAX_SIGNAL_NO)
		myPCB->signal_hendlers[id]->swap(hand1, hand2);
}
void Thread::blockSignal(SignalId signal){
	if (signal >= 0 && signal < MAX_SIGNAL_NO)
		myPCB->block_signal_flag[signal]=1;
}
void Thread::blockSignalGlobally(SignalId signal){
	if (signal >= 0 && signal < MAX_SIGNAL_NO)
		PCB::block_signal_global_flag[signal]=1;
}
void Thread::unblockSignal(SignalId signal){
	if (signal >= 0 && signal < MAX_SIGNAL_NO)
		myPCB->block_signal_flag[signal]=0;
}
void Thread::unblockSignalGlobally(SignalId signal){
	if (signal >= 0 && signal < MAX_SIGNAL_NO)
		PCB::block_signal_global_flag[signal]=0;
}

