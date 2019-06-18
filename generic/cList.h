/*
 * CList.h
 *
 *  Implementation of circular list
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef CLIST_H_
#define CLIST_H_

// Definition of circular list structure
struct CList_t {
	struct CList_t* next;
	struct CList_t* prev;
};
typedef struct CList_t CList_t;

// Functions declarations
void CList_Add(CList_t* clist, CList_t* new);
CList_t* CList_Next(CList_t* clist);
CList_t* CList_Prev(CList_t* clist);
void CList_Remove(CList_t* clist);

#endif /* CLIST_H_ */
