/*
 * SigH_Lst.h
 *
 *  Created on: May 2, 2020
 *      Author: OS1
 */

#ifndef SIGH_LST_H_
#define SIGH_LST_H_

#include<stdio.h>
typedef unsigned SignalId;
typedef void (*SignalHandler)();

struct SigHandnode {
	SignalHandler sh_;
	SigHandnode* next_;
	SigHandnode(SignalHandler sh);
};

class SignalHandler_List {

public:
	SignalHandler_List();
	~SignalHandler_List();

	void insert(SignalHandler sh);
	void swap(SignalHandler sh1, SignalHandler sh2);
	SignalHandler_List* copy();//because child inherits it form father
	void print(FILE *fo);
private:


	SigHandnode * first_, * last_;
	friend class PCB;
	friend class SignalList;
};


#endif /* SIGH_LST_H_ */
