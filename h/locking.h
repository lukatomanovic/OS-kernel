/*
 * locking.h
 *
 *  Created on: Apr 18, 2020
 *      Author: OS1
 */

#ifndef LOCKING_H_
#define LOCKING_H_

extern volatile int locked;
extern volatile int dispatch_on_request;

#define LOCK locked+=1;
#define UNLOCK locked-=1;// za svaki slucaj, da nema neki unlock viska inace je dovolnjo locked--; na ovo obratiti paznju



#endif /* LOCKING_H_ */
