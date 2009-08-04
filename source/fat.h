#ifndef _FAT_H_
#define _FAT_H_

#include "net.h"

/* Prototypes */
s32 Fat_MountSDHC(void);
s32 Fat_UnmountSDHC(void);
s32 Fat_ReadFile(const char *, void **);
s32 Fat_WriteFile(const char *, struct block buffer);
bool Fat_IsMounted(void);

#endif
