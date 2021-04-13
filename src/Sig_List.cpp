/*
 * Sig_List.cpp
 *
 *  Created on: May 2, 2020
 *      Author: OS1
 */

#include"Sig_List.h"
#include"locking.h"
#include<stdio.h>
#include<stdlib.h>
#include"pcb.h"

Signode::Signode(SignalId id) :
		sig_id_(id), next_(0) {
}

Signal_List::Signal_List() :
		first_(0), last_(0) {
}

Signal_List::~Signal_List() {
	LOCK
	Signode *old;
	while (first_ != 0) {
		old = first_;
		first_ = first_->next_;
		delete old;
	}
	last_=first_=0;
	UNLOCK
}

void Signal_List::insert(SignalId id) {
	LOCK
	Signode *nelem = new Signode(id);
	if (first_ == 0)
		first_ = nelem;
	else
		last_->next_ = nelem;
	last_ = nelem;
	UNLOCK
}

Signode*  Signal_List::getById(SignalId id) {
	LOCK
	Signode *prev = 0;
	Signode *curr = first_;
	while (curr != 0) {
		if (curr->sig_id_ == id) {
			if (prev) {
				prev->next_ = curr->next_;
			} else {
				first_ = curr->next_;
			}
			if (curr->next_ == 0)
				last_ = prev;
			curr->next_=0;
			UNLOCK
			return curr;
		}
		prev = curr;
		curr = curr->next_;
	}
	UNLOCK
	return 0;
}
Signode* Signal_List::getFirst() {
	return first_;
}
Signode* Signal_List::getAndRemoveFirst() {
	LOCK
	if(first_==0){
		UNLOCK
		return 0;
	}
	Signode* ret = first_;
	first_ = first_->next_;
	if (first_ == 0)
		last_ = 0;
	UNLOCK
	return ret;
}

Signode* Signal_List::findById(SignalId id) {
	LOCK
	Signode *curr = first_;
	while (curr != 0 && curr->sig_id_ != id) {
		curr = curr->next_;
	}
	UNLOCK
	return curr;

}

void Signal_List::print(FILE *fo) {
	LOCK
	fprintf(fo,"Ispis liste signala za %d\n",PCB::running->getId());
	Signode *curr = first_;
	while (curr != 0) {
		fprintf(fo,"signall id: %d\n", curr->sig_id_);
		curr=curr->next_;
	}
	UNLOCK

}
