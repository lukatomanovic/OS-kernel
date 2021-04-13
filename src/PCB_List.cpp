/*
 * PCB_List.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */


#include "PCB_List.h"
#include "pcb.h"
#include "locking.h"
#include"SCHEDULE.H"
#include<stdio.h>
#include"kersem.h"
#include<stdlib.h>

PCBnode::PCBnode(PCB *pcb, Time time_to_wait){
	pcb_=pcb;
	time_to_wait_=time_to_wait;
	next_=0;
}

PCB_List::PCB_List() : first_(0), last_(0), number_of_elements_(0){}

PCB_List::~PCB_List(){
	LOCK
	PCBnode *old;
	while(first_){
		old=first_;
		first_=first_->next_;
		delete old;
	}
	number_of_elements_=0;//nije potrebno, jer se unistava
	UNLOCK
}

unsigned PCB_List::getNumberOfElements(){return number_of_elements_;}

void PCB_List::add(PCB *p){

	LOCK

	PCBnode* new_el = new PCBnode(p);
	if (first_ == 0)
		first_ = new_el;
	else
		last_->next_ = new_el;
	last_ = new_el;
	number_of_elements_++;
	UNLOCK

}

PCB* PCB_List::getById(ID id){
	LOCK
	PCBnode *prev = 0;
	PCBnode *curr = first_;
	while (curr != 0) {
		if (curr->pcb_->getId()== id) {
			if (prev) {
				prev->next_ = curr->next_;
			}
			else{ //curr is first elem
				first_ = curr->next_;
			}
			if (curr->next_)
				curr->next_->time_to_wait_ += curr->time_to_wait_;
			else {//curr is last elem
				last_ = prev;
			}
			PCB *tmp=curr->pcb_;
			delete curr;
			number_of_elements_--;
			UNLOCK
			return tmp;
		}
		prev = curr;
		curr = curr->next_;
	}
	UNLOCK
	return 0;
}

PCB* PCB_List::get(){
	PCBnode *curr = first_;
	return curr->pcb_;
}

//radi i za time to wait
PCB* PCB_List::getAndRemove(){
	LOCK

	if (first_ != 0) {
		PCB* head = first_->pcb_;
		PCBnode *old=first_;
		first_=first_->next_;
		if (first_ == 0) {
			last_ = 0;
		}
		else {
			first_->time_to_wait_ += old->time_to_wait_;
		}
		delete old;
		number_of_elements_--;
		UNLOCK
		return head;
	}

	UNLOCK
	return 0;
}

PCB* PCB_List::findFirstByOrdinalNumber(){//koristi se u sigurnim operacijama, ne moram zakljucavati, deblock koristi, a on stavlja lock
	LOCK
	if(first_==0){
		UNLOCK
		return 0;
	}
	PCBnode *curr = first_,*tmp=first_;
	int min=curr->pcb_->getSemOrdinalNumber();
	curr=curr->next_;
	while (curr != 0) {
		if (curr->pcb_->getSemOrdinalNumber()<min)
		{
			tmp=curr;
			min=curr->pcb_->getSemOrdinalNumber();
		}
		curr= curr->next_;
	}
	UNLOCK
	return tmp->pcb_;
}


PCB* PCB_List::findById(ID id){
	LOCK
	PCBnode *curr = first_;
	while (curr != 0) {
		if (curr->pcb_->getId()== id)
		{
			UNLOCK
			return curr->pcb_;
		}
		curr= curr->next_;
	}
	UNLOCK
	return 0;
}



void PCB_List::addSortedByTime(PCB *p, Time maxTimeToWait){
	LOCK

	PCBnode *nnode = new PCBnode(p, maxTimeToWait);
	PCBnode *curr=first_;
	PCBnode *prev=0;
	if(curr==0){
		first_=last_=nnode;
		number_of_elements_++;
	}
	else{
		int diff;
		while(curr!=0){
			diff=maxTimeToWait-curr->time_to_wait_;
			if(diff>0){
				maxTimeToWait-=curr->time_to_wait_;
				prev=curr;
				curr=curr->next_;
			}
			else{
				nnode->time_to_wait_=maxTimeToWait;
				nnode->next_=curr;
				curr->time_to_wait_-=nnode->time_to_wait_;
				if(prev){
					prev->next_=nnode;
				}
				else{
					first_=nnode;
				}
				number_of_elements_++;
				UNLOCK
				return;
			}
		}
		//add node to the end of list
		nnode->time_to_wait_=maxTimeToWait;
		last_->next_=nnode;
		last_=nnode;
		number_of_elements_++;
	}
	UNLOCK
	return;

}


void PCB_List::timerUpdate(KernelSem* ks){
	LOCK

	PCBnode *curr=first_;

	  if (curr == 0){
		UNLOCK
	    return;
	  }
	  if (curr->time_to_wait_ != 0)
		  curr->time_to_wait_--;
	  while(curr){
		  if(curr->time_to_wait_>0){
			  UNLOCK
			  return;
		  }
		  else{
			  curr = curr->next_;
			  //RESENO, FINALNI KOD OSTAJE OVAKAV
			  //treba odblokirati nit
			  //postaviti ready i povratni rezultat za wait
			  //vratiti u red spremnih
			  //izbaciti ga iz reda cekanja i preci na sledeci
			  //ponoviti dokle god je wait_time 0
			  //takodje treba obavestiti onoga na kome je blokiran
			  ks->timesignal();
		  }
	  }
	  UNLOCK
}
void PCB_List::print_list(FILE *fo) {
	PCBnode *curr = first_;
	if (!curr)
		fprintf(fo,"Lista je prazna!\n");
	else {
		int i = 1;
		while (curr) {
			fprintf(fo,"%d\t%d\t%d\n",i,curr->pcb_->getId(),curr->time_to_wait_);
			i++;
			curr = curr->next_;
		}
	}
}

