/* Extended functions for Joerg Wunsch's uart libarary
 * written by Matthew Nevels
 * 2/27/10
 */ 
/*
 * Get string using uart_getchar()
 */

#include <stdio.h>
#include <stdlib.h>		//for atoi,atof,strtod

#include "uart_extended.h"

 
int uart_getstring(char* c, FILE *stream)  
{
	int i=0;
	c[i] = uart_getchar(stream);
	while(c[i] != '\n'){
		i++;
		c[i] = uart_getchar(stream);
		}
	c[i] = '\0';
	return 0;
}

int uart_getint(FILE *stream){
	char s[80];
	uart_getstring(s,stream);
	return atoi(s);
}
float uart_getfloat(FILE *stream){
	char s[80];
	uart_getstring(s,stream);
	return atof(s);
}

