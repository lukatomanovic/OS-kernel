/*
 * pcb.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */

#include "pcb.h"
#include <dos.h>
#include "locking.h"
#include "SCHEDULE.H"
#include<stdio.h>
#include<stdlib.h>
#include"Sig_List.h"
#include"SigH_Lst.h"


//inicijalizacija statickih promenljivih
PCB* PCB::running = 0;
PCB_List* PCB::pcb_list_ = new PCB_List();
unsigned PCB::next_id = 0;
unsigned PCB::block_signal_global_flag[MAX_SIGNAL_NO];
int PCB::inicialaze_global_signal_flag=0;


extern PCB *main_PCB, *idlePCB;

PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice) :
		pcbThread_(thread), timeSlice_(timeSlice), status_(INIT) {
	stack_=0;//stek init on start thread
	if (stackSize > MAX_STACK_SIZE)
		stackSize = MAX_STACK_SIZE;
	stack_size_ = stackSize / sizeof(*stack_); //unsigned 2B
	threadID_ = next_id;
	next_id++;
	pcb_waiting_for_me_ = new PCB_List();
	pcb_list_->add(this);
	sem_blocked_on_=0;
	isTimeBlocked_=0;
	sem_ordinal_number_=-1;
	//stackInitialize ce se zvati tek kada dodje do starta


	//SIGNALS
	if(inicialaze_global_signal_flag==0){
		for (int g = 0; g < MAX_SIGNAL_NO; g++) {
			block_signal_global_flag[g]=0;
		}
		inicialaze_global_signal_flag=1;
	}
	parent_ = PCB::running;
	signals_=new Signal_List();
	if(parent_!=0){
		for (int l = 0; l < MAX_SIGNAL_NO; l++) {
			block_signal_flag[l]=parent_->block_signal_flag[l];
		}
	}
	else{
		for (int l = 0; l < MAX_SIGNAL_NO; l++) {
			block_signal_flag[l]=0;
		}
	}


	for (int i = 0; i < MAX_SIGNAL_NO; i++) {
		signal_hendlers[i] = new SignalHandler_List();
		if (i == 0 && threadID_ == 0) {
			signal_hendlers[0]->insert(SignalHandler0);
		} else if(threadID_!=0)
			signal_hendlers[i] = PCB::running->signal_hendlers[i]->copy();
	}

}

PCB::~PCB(){
	LOCK
	PCB *provera=pcb_list_->getById(this->threadID_);
	if(pcb_waiting_for_me_!=0)	delete pcb_waiting_for_me_;
	delete[] this->stack_;
	if(signals_!=0) delete this->signals_;
	for (int i = 0; i < MAX_SIGNAL_NO; i++){
		if (signal_hendlers[i]!=0){
			delete signal_hendlers[i];
			signal_hendlers[i] = 0;
		}
	}
	UNLOCK
}

void PCB::stackInitialize() {
#ifndef BCC_BLOCK_IGNORE
	asm{cli}
#endif
	stack_ = new unsigned[stack_size_];
#ifndef BCC_BLOCK_IGNORE
	asm{sti}
#endif
#ifndef BCC_BLOCK_IGNORE
	stack_[stack_size_ - 1] = 0x200; //set PSW bit to 1 0010 0000 0000
	stack_[stack_size_ - 2] = FP_SEG(PCB::wrapper);
	stack_[stack_size_ - 3] = FP_OFF(PCB::wrapper);
	ss_ = FP_SEG(stack_ + stack_size_ - 12);
	sp_ = FP_OFF(stack_ + stack_size_ - 12);
	bp_ = sp_;
#endif
}

void PCB::wrapper() {
	PCB::running->pcbThread_->run();
	LOCK
	//zakljucavam kako bih posto je nit na kraju ipak vratio sve niti koje su cekale, da ne cekaju bez potrebe

	if (PCB::running->parent_ != 0)
		PCB::running->parent_->pcbThread_->systemSignals12(1);
	PCB::running -> pcbThread_->systemSignals12(2);
	PCB::running->signalProcessing();
	running->status_ = FINISHED;
	PCB *tmp = running->pcb_waiting_for_me_->getAndRemove();
	//NIT SAMO TREBA OBAVESTITI, MOZDA ONA CEKA NA JOS NEKU NIT
	while (tmp) { //vadimo niti koje su cekale na ovu i stavljamo ih u scheduler
		if (tmp->status_ == BLOCKED) {
			tmp->status_ = READY;
			Scheduler::put(tmp);
		}
		tmp = running->pcb_waiting_for_me_->getAndRemove();
	}

	UNLOCK
	if(locked!=0)locked=0;
	dispatch();
}


/********SIGNALS**********/

void PCB::SignalHandler0(){
	LOCK
	//notify parent that child is done, but main thread probably would not have parent(good reason to check it)
	if (PCB::running->parent_ != 0)
		PCB::running->parent_->pcbThread_->systemSignals12(1);

	//notify thread owner that pcb is done
	PCB::running->status_=FINISHED;
	PCB::running->pcbThread_->systemSignals12(2);

	//get back to scheduler all blocked pcbs waiting for running
	PCB *tmp = running->pcb_waiting_for_me_->getAndRemove();
	while (tmp) {
		if(tmp->status_ == BLOCKED){
		tmp->status_ = READY;
		Scheduler::put(tmp);
		}
		tmp = running->pcb_waiting_for_me_->getAndRemove();
	}
	//delete memory witch never gonna be used again
	delete running->pcb_waiting_for_me_;
	running->pcb_waiting_for_me_=0;

	//delete handlers
	for (int i = 0; i < MAX_SIGNAL_NO; i++){
		if (PCB::running->signal_hendlers[i]!=0){
			delete PCB::running->signal_hendlers[i];
			PCB::running->signal_hendlers[i] = 0;
		}
	}
	delete running->signals_;
	running->signals_=0;
	delete[] running->stack_;
	running->stack_=0;
	UNLOCK
	if(locked!=0)locked=0;//pravilo je problem kada se ovo izvrsava, signalProcessing se ne nastavi kada se nit unisti i ostane sistem zakljucan
	dispatch();
}

void PCB::signalProcessing(){

	if(PCB::running==idlePCB)return;
	LOCK
	Signode *curr=signals_->getFirst();
	while(curr!=0){
		if(block_signal_flag[curr->sig_id_]==0&&PCB::block_signal_global_flag[curr->sig_id_]==0){
			int sigid=curr->sig_id_;
			curr=curr->next_;
			Signode *proccess_sig_node=signals_->getById(sigid);
			volatile SigHandnode *handler=this->signal_hendlers[sigid]->first_;
			while(handler!=0){
				handler->sh_();
				handler=handler->next_;
			}
			delete proccess_sig_node;
		}
		else{
			curr = curr->next_;
		}
	}
	UNLOCK
}


