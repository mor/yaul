#ifndef _COVER_H_
#define _COVER_H_

#include <sys/stat.h>

#include "fat.h"
#include "net.h"
#include "video.h"
#include "config.h"

s32 Cover_Fetch(u8 * discid);
bool Cover_Exists(u8 * discid);
void Cover_GetFilepath(u8 * discid, char * cover_file);
s32 Cover_Delete(u8 * discid);

#endif
