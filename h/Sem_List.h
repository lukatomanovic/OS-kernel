/*
 * kersem_List.h
 *
 *  Created on: Apr 23, 2020
 *      Author: OS1
 */

#ifndef SEM_LIST_H_
#define SEM_LIST_H_

class KernelSem;
struct KernelSemnode
{
	KernelSem *sem_;
	KernelSemnode* next_;
	KernelSemnode(KernelSem *sem):sem_(sem),next_(0){}
};

class KernelSem_List{
public:
	KernelSem_List();
	~KernelSem_List();
	void add(KernelSem* ks);//dodavanje na kraj
	void remove(KernelSem* ks);//mozda i ne bude trebalo
	KernelSem* getAndRemove();//first
	void updateTime();
	void print_list();
private:
	KernelSemnode *first_, *last_;
	unsigned number_of_sem_;
};



#endif /* SEM_LIST_H_ */
