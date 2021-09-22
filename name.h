#ifndef __NAME_H
#define __NAME_H

extern char* username;   		// Stores username
extern struct utsname* buf;		// buf->nodename contains the hostname

void set_username();
void set_hostname();

void name_free(); 				// To deallocate heap memory

#endif