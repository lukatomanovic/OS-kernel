/*
 * Sig_List.h
 *
 *  Created on: May 2, 2020
 *      Author: OS1
 */

#ifndef SIG_LIST_H_
#define SIG_LIST_H_
#include<stdio.h>

typedef void (*SignalHandler)();
typedef unsigned SignalId;
class SignalHandler_List;

struct Signode {
	SignalId sig_id_;
	Signode* next_;
	Signode(SignalId id);
};



class Signal_List {
public:
	Signal_List();
	~Signal_List();

	void insert(SignalId id);
	Signode* getFirst();
	Signode* getAndRemoveFirst();
	Signode* getById(SignalId);
	Signode* findById(SignalId);


	void print(FILE *fo);
private:
	Signode *first_, * last_;

};

#endif /* SIG_LIST_H_ */
