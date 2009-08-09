#ifndef _NET_H_
#define _NET_H_

#include <network.h>
#include <errno.h>
#include <ogcsys.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

//#include "dns.h"

/**
 * A simple structure to keep track of the size of a malloc()ated block of memory
 */
struct block
{
	u32 size;
	unsigned char * data;
};

extern const struct block emptyblock;

void Net_Init(void);
struct block Net_GetFile(char * host, char * path);
u32 Net_Getipbyname(char *host);
u32 Net_GetIP(char * host);

#ifdef __cplusplus
}
#endif

#endif /* _NET_H_ */
