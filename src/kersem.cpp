/*
 * kersem.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: OS1
 */

#include"kersem.h"
#include"locking.h"
#include"PCB_List.h"
#include "pcb.h"
#include "Sem_List.h"
#include "SCHEDULE.H"
#include<stdio.h>
#include<stdlib.h>

volatile int semupdate=0;

int KernelSem::next_number_in_queue = 0;
KernelSem_List* KernelSem::sem_list_ = new KernelSem_List();
KernelSem::KernelSem(int val) :
		val_(val) {
	list_of_blocked_ = new PCB_List();
	list_of_time_blocked_ = new PCB_List();
	sem_list_->add(this);

}
KernelSem::~KernelSem(){
	LOCK
	sem_list_->remove(this);
	PCB *pop=list_of_blocked_->getAndRemove();
	while(pop){
		if(pop->status_==BLOCKED){
			pop->status_=READY;
			Scheduler::put(pop);
		}
		pop=list_of_blocked_->getAndRemove();
	}
	pop=list_of_time_blocked_->getAndRemove();
	while(pop){
		if(pop->status_==BLOCKED){
			pop->status_=READY;
			Scheduler::put(pop);
		}
		pop=list_of_time_blocked_->getAndRemove();
	}
	delete list_of_blocked_;
	delete list_of_time_blocked_;
	val_ = 0;
	UNLOCK
}

void KernelSem::block(Time maxTimeToWait) {
	//ne mora se zaklucavati, ovim operacijama upravljaju wait i signal
	PCB::running->status_ = BLOCKED;
	PCB::running->setSemBlockedOn(this);
	PCB::running->setSemOrdinalNumber(next_number_in_queue++);
	if (maxTimeToWait) {
		list_of_time_blocked_->addSortedByTime(PCB::running, maxTimeToWait);
		PCB::running->setIsTimeBlocked(1);
	} else {
		list_of_blocked_->add(PCB::running);
		PCB::running->setIsTimeBlocked(0);
	}
}
void KernelSem::deblock(int timesig) {
	//ne mora se zaklucavati, ovim operacijama upravljaju wait i signal
	//ako je deblokiranje izazvano vremenom, vadimo iz vremenske liste
	PCB *get_to_be_ready = 0;
	if (timesig) {
		get_to_be_ready = list_of_time_blocked_->getAndRemove();
	}
	else {
		PCB *tmp_time=list_of_time_blocked_->findFirstByOrdinalNumber();
		PCB *tmp_no_time=list_of_blocked_->get();//get samo dohvata prvi, ne uklanja
		if(tmp_time==0&&tmp_no_time==0){
			get_to_be_ready=0;
		}
		else if(tmp_time!=0&&tmp_no_time==0){

			get_to_be_ready=list_of_time_blocked_->getById(tmp_time->getId());
		}
		else if(tmp_time==0&&tmp_no_time!=0)
		{

			get_to_be_ready=list_of_blocked_->getAndRemove();
		}
		else{//ako je doslo do ovoga, gleda se koji je prvi pcb zakucao na semafor

			if(tmp_time->getSemOrdinalNumber()<tmp_no_time->getSemOrdinalNumber()){
				get_to_be_ready=list_of_time_blocked_->getById(tmp_time->getId());
			}
			else{
				get_to_be_ready=list_of_blocked_->getAndRemove();//sklanja prvi, moze i ovde byId
			}
		}
	}

	if (get_to_be_ready != 0 && get_to_be_ready->status_ != FINISHED) { //nije realno da bude finished, cisto radi sigurnosti, ocekuje se da bude BLOCKED
		get_to_be_ready->status_=READY;
		Scheduler::put(get_to_be_ready);
	}
}

int KernelSem::wait(Time maxTimeToWait) {
	LOCK
	this->val_--;
	if (this->val_ < 0) {
		block(maxTimeToWait);
		UNLOCK
		dispatch();
		LOCK //da neko slucajno ne promeni fleg-mozda i da se obrise
	    if (PCB::running->getIsTimeBlocked()== 1){
	    	PCB::running->setIsTimeBlocked(0);//nije obavezno, svakako se uvek setuje flag, cisto zbog cistoce koda
	    	UNLOCK
	    	return 0;
	    }

	}
	UNLOCK
	return 1;
}
void KernelSem::timesignal(){
	LOCK
	if(list_of_time_blocked_->getNumberOfElements()!=0){
		deblock(1);
		val_++;
	}
	UNLOCK
}

int KernelSem::signal(int n) {
	LOCK
	if (n == 0) {
		if (val_++ < 0)
			deblock();
	}
	else if (n > 0) {
		int num_for_deblock=0;
		if (val_ < 0) {
			if(-1*n<val_)num_for_deblock=-1*val_;
			else num_for_deblock=n;
			for (int i = 0; i < num_for_deblock; i++)
				deblock();
		}
		val_ += n;
		n = num_for_deblock;
	}
	UNLOCK
	return n;

}

void KernelSem::updateTimeOnAllSems(){
	semupdate=1;
	if (sem_list_ != 0){
		sem_list_->updateTime();
	}
	semupdate=0;

}
