/*
 * Sem_List.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: OS1
 */
#include"Sem_List.h"
#include"PCB_List.h"
#include "kersem.h"
#include "locking.h"
#include<stdio.h>
KernelSem_List::KernelSem_List():first_(0),last_(0),number_of_sem_(0){}

KernelSem_List::~KernelSem_List(){
	LOCK
	KernelSemnode *old;
	while(first_){
		old=first_;
		first_=first_->next_;
		delete old;
	}
	number_of_sem_=0;
	UNLOCK
}

void KernelSem_List::add(KernelSem* ks) {
	LOCK
	KernelSemnode* novi = new KernelSemnode(ks);
	if (first_ == 0)
		first_ = novi;
	else
		last_->next_ = novi;
	last_ = novi;
	number_of_sem_++;
  	UNLOCK
}


KernelSem* KernelSem_List::getAndRemove(){
  if (first_ == 0)
    return 0;
  KernelSemnode* old = first_;
  KernelSem* ret = old->sem_;
  first_ = first_->next_;
  if (first_ == 0)
    last_ = 0;
  delete old;
  number_of_sem_--;
  return ret;
}

void KernelSem_List::remove(KernelSem* ks) {
	KernelSemnode* curr = first_, * prev = 0;
	while(curr != 0 && curr->sem_ != ks){
		prev=curr;
		curr = curr->next_;
	}
	if (curr == 0)
		return;
	if (curr == first_){ //remove first
		first_ = first_->next_;
		if(first_==0)last_=0;
	}
	else {
		prev->next_ = curr->next_;
		if (curr == last_)
			last_ = prev;
	}
	delete curr;
	number_of_sem_--;
}

void KernelSem_List::updateTime() { //zakljucavamo kako bi vreme bilo azurirano u celoj listi
	LOCK
	//fsem=fopen("semaf.txt","a+");
	KernelSemnode* curr = first_;
	KernelSem* ks = 0;
	//fprintf(fsem,"Obilazak svih vremenskih semafora! \n");
	int no=0;
	while (curr != 0) {
		//fprintf(fsem,"Obilazak semafora %d! \n",no++);
		ks = curr->sem_;
		//ks->list_of_time_blocked_->print_list(fsem);
		ks->list_of_time_blocked_->timerUpdate(ks);
		curr = curr->next_;
	}
	//fclose(fsem);
	UNLOCK
}

void KernelSem_List::print_list() {
	KernelSemnode *curr = first_;
	if (!curr)
		printf("Lista semafora je prazna!\n");
	else {
		int i = 1;
		while (curr) {
			printf("%d\t%d\n",i,curr->sem_->getVal());
			i++;
			curr = curr->next_;
		}
	}

}

