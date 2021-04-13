/*
 * main.cpp
 *
 *  Created on: May 3, 2020
 *      Author: OS1
 */



#include <stdio.h>
#include"thread.h"
#include <iostream.h>
#include "pcb.h"
#include "Routines.h"
#include<stdlib.h>
#include "idle.h"
IdleThread *idle;
volatile int locked = 0;
volatile int dispatch_on_request = 0;

extern int userMain(int argc, char* argv[]);


PCB *main_PCB;
PCB* idlePCB;
int main(int argc, char* argv[]){
	PCB::running=0;
  main_PCB = new PCB(0, 4096, 1);
  main_PCB->status_ = RUNNING;
  PCB::running = main_PCB;
  Thread *maint=new Thread(main_PCB);
  main_PCB->setPCBThread(maint);
  idle=new IdleThread();
  idlePCB=idle->getIdlePCB();
  idle->start();
  inic();
  char *argsmy[4];
  argsmy[0]=argv[0];
  argsmy[1]="50";
  argsmy[2]="19";
  argsmy[3]="2";
  int ret = userMain(4,argsmy);

  cout << ret << endl;
  restore();
  idle->stop();
  delete idle;
  delete maint;
  return ret;
}


