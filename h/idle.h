/*
 * idle.h
 *
 *  Created on: Apr 27, 2020
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include"thread.h"

class IdleThread:public Thread{
public:
  IdleThread();
  PCB * getIdlePCB();
  void run();
  void start();
  virtual ~IdleThread(){};
  void stop();


  static void deleteIdleThread();
private:
  int doingsomething;
};


#endif /* IDLE_H_ */
