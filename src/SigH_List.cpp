/*
 * SigH_List.cpp
 *
 *  Created on: May 2, 2020
 *      Author: OS1
 */
#include"SigH_Lst.h"
#include"locking.h"


SigHandnode::SigHandnode(SignalHandler sh):sh_(sh),next_(0){

}

SignalHandler_List::SignalHandler_List():first_(0),last_(0){}

SignalHandler_List::~SignalHandler_List() {
	LOCK
	SigHandnode *old;
	while (first_ != 0) {
		old = first_;
		first_ = first_->next_;
		delete old;
	}
	first_=last_=0;
	UNLOCK
}

void SignalHandler_List::insert(SignalHandler sh) {
	  LOCK
	  SigHandnode *nelem=new SigHandnode(sh);
		if (first_ == 0)
			first_ = nelem;
		else
			last_->next_ = nelem;
		last_ = nelem;
	  UNLOCK
}

void SignalHandler_List::swap(SignalHandler sh1, SignalHandler sh2){

	if (sh1 == sh2)
		return;
	LOCK
	SigHandnode *curr = first_,*shn1= 0, *shn2=0;
	while (curr!=0&&shn1!=0&&shn2!=0){
		if (curr->sh_ == sh1)
			shn1 = curr;
		else if(curr->sh_ == sh2)
			shn2= curr;

		curr = curr->next_;
	}
	if (shn1 != 0 && shn2 != 0){
		SignalHandler tmp=shn1->sh_;
		shn1->sh_=shn2->sh_;
		shn2->sh_=tmp;

	}
	UNLOCK
	return;
}
SignalHandler_List* SignalHandler_List::copy(){
	LOCK
	SignalHandler_List *nshl=new SignalHandler_List();

	SigHandnode* curr = this->first_;
	while (curr){
		nshl->insert(curr->sh_);
		curr = curr->next_;
	}
	UNLOCK
	return nshl;
}
void SignalHandler_List::print(FILE *FO){
	LOCK
	SigHandnode* cur = first_;
	while (cur){
		fprintf(FO,"%d ",cur->sh_);
		cur = cur->next_;
	}
	fprintf(FO,"\n");
	UNLOCK
}
