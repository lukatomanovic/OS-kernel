/*
 * Routines.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: OS1
 */

#include "Routines.h"
#include "locking.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include <stdio.h>
#include<stdlib.h>
#include"kersem.h"
#include"idle.h"
extern IdleThread *idle;
extern volatile int semupdate;
volatile int time_counter = 0;
volatile unsigned tsp, tss, tbp;
extern PCB *idlePCB, *main_PCB;


unsigned oldTimerOFF, oldTimerSEG;
extern void tick();
void inic()	//inicijalizacija IVT za nasu prekidnu rutinu timer-a
{
#ifndef BCC_BLOCK_IGNORE
	asm {
		cli
		push es
		push ax
		mov ax,0
		mov es,ax
		mov ax, word ptr es:0022h

		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax
		mov word ptr es:0022h, seg timer

		mov word ptr es:0020h, offset timer
		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax

		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax
		pop ax
		pop es
		sti
	}
#endif
}

void restore() {
#ifndef BCC_BLOCK_IGNORE
	asm {
		cli
		push es
		push ax
		mov ax,0
		mov es,ax
		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax
		pop ax
		pop es
		sti
	}
#endif
}

FILE *ft;

void interrupt timer() //nasa prekidna rutina koju podmecemo da se izvrsava na svakih 55ms, koristi se i za promenu konteksta
{
	if ((!dispatch_on_request) && (PCB::running->timeSlice_ != 0)) {
		time_counter--; //nije zahtevana promena konteksta, samo treba da smanjimo vreme koje je preostalo niti da se izvrsava ako joj
		//nije tslice jednako 0

	}
	if (dispatch_on_request||((locked==0)&& ((time_counter <= 0) && (PCB::running->timeSlice_ != 0)))) {

			#ifndef BCC_BLOCK_IGNORE
			asm {
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
			#endif

			PCB::running->sp_ = tsp;
			PCB::running->ss_ = tss;
			PCB::running->bp_ = tbp;
			if ((PCB::running->status_ == RUNNING)&&(idle->getIdlePCB()!=PCB::running)){
				PCB::running->status_=READY;
				Scheduler::put(PCB::running); //Scheduler::put((PCB*) PCB::running);
			}
			PCB::running = Scheduler::get();
			if (!PCB::running) {
				PCB::running=idle->getIdlePCB();
			}
			PCB::running->status_=RUNNING;
			tsp = PCB::running->sp_;
			tss = PCB::running->ss_;
			tbp = PCB::running->bp_;

			time_counter = PCB::running->timeSlice_;
			#ifndef BCC_BLOCK_IGNORE
			asm {
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
			#endif
			if(PCB::running!=idlePCB)
				PCB::running->signalProcessing();

	}
	if(dispatch_on_request){
			dispatch_on_request=0;
	}
	else{
		if(!semupdate)KernelSem::updateTimeOnAllSems();
		tick();
		#ifndef BCC_BLOCK_IGNORE
		asm int 60h;
		#endif
	}
}
