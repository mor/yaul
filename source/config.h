#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Application Constants */
#define CONSOLE_XCOORD 258
#define CONSOLE_YCOORD 112
#define CONSOLE_WIDTH 354
#define CONSOLE_HEIGHT 304

#define COVER_XCOORD 24
#define COVER_YCOORD 104
#define COVER_WIDTH 176
#define COVER_HEIGHT 248

#define BACKGROUND_WIDTH 640
#define BACKGROUND_HEIGHT 480

#define DATA_PATH "sdhc:/usb-loader"
#define COVERS_PATH DATA_PATH "/covers"

#define COVER_HOST_1 "awiibit.com"
#define COVER_HOST_1_2D_PATH "/BoxArt160x224/%s.png" // %s = header->id
#define COVER_HOST_1_3D_PATH "/3dBoxArt176x248/%s.png" // %s = header->id

#define COVER_HOST_2 "saratoga.ath.cx"
#define COVER_HOST_2_2D_PATH "/wii_2D_cover_images/%s.png"
#define COVER_HOST_2_3D_PATH "/wii_3D_box_images/%s.png"

#endif
