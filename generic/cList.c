/*
 * cList.c
 *
 *  Implementation of circular list
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "cList.h"

// Add element to circular list
void CList_Add(CList_t* clist, CList_t* new) {
	if(clist->next == clist || clist->next == 0x0) {		// There is only one element in list
		clist->next = clist->prev = new;
		new->next = new->prev = clist;
	} else {
		CList_t* oldNext = clist->next;
		clist->next = new;
		new->prev = clist;
		new->next = oldNext;
		oldNext->prev = new;
	}
}

// Get next element of list
CList_t* CList_Next(CList_t* clist) {
	if(clist->next == 0x0)
		return clist;
	return clist->next;
}

// Get previos element of list
CList_t* CList_Prev(CList_t* clist) {
	if(clist->prev == 0x0)
		return clist;
	return clist->prev;
}

// Remove element from list
void CList_Remove(CList_t* clist) {
	if(clist->prev == clist->next)
		return;		// Cannot remove the only element in list

	clist->prev->next = clist->next;
	clist->next->prev = clist->prev;
}
