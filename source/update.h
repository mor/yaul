#ifndef _UPDATE_H_
#define _UPDATE_H_

#include "net.h"

s32 Update_Fetch(void);
struct block Update_CheckVersion(void);
void hexify(char * in_str, char * out_str);
char * my_sum(struct block buffer);

#endif
