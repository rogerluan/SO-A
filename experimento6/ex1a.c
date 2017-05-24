//
//  Created on May 23rd 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

#include <lib.h>

int main(void) {
	message m;
	_syscall(PM_PROC_NR,WHOSTHERE, &m);
	return 1;
}
