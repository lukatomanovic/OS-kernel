/*
 * PCB_List.h
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */

#ifndef PCB_LIST_H_
#define PCB_LIST_H_

#include "thread.h"
#include<stdio.h>
class PCB;
class KernelSem;

struct PCBnode
{
	//friend class PCB;
	PCB *pcb_;
	PCBnode* next_;
	Time time_to_wait_;
	PCBnode(PCB *pcb, Time time_to_wait=0);
};

class PCB_List{
public:
	PCB_List();
	~PCB_List();
	void add(PCB *p);//add element at the end of list
	void addSortedByTime(PCB *p, Time maxTimeToWait);//add element at list and keep it sorted
	PCB* get();//first - just get, do not remove,POTENCIJALNO ZA UKLANJANJE
	PCB* getAndRemove();//first
	PCB* findFirstByOrdinalNumber();//za semafor
	PCB* findById(ID id);
	PCB* getById(ID id);//get and remove element by ID and update time
	void timerUpdate(KernelSem* sem);//update times on timer tick
	unsigned getNumberOfElements();
	void print_list(FILE *fo);
private:
	PCBnode *first_, *last_;
	unsigned number_of_elements_;
};



#endif /* PCB_LIST_H_ */
