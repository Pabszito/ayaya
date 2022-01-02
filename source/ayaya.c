#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <asndlib.h>
#include <mp3player.h>
#include <wiiuse/wpad.h>
#include <jpeg/jpgogc.h>

#include "ayaya_mp3.h"

static u32 *xfb;
static GXRModeObj *rmode = NULL;

JPEGIMG pointer;
extern char picData[];
extern int picLength;
ir_t ir;

void InitPointer(){
    memset(&pointer, 0, sizeof(JPEGIMG));

    pointer.inbuffer = picData;
    pointer.inbufferlength = picLength;

    JPEG_Decompress(&pointer);
}

void DisplayImage(JPEGIMG jpeg, int x, int y) {
    unsigned int *jpegout = (unsigned int *) jpeg.outbuffer;

    int i,j;
    int height = jpeg.height;
    int width = jpeg.width / 2;
    for(i = 0; i <= width; i++)
        for(j = 0; j <= height - 2; j++)
            xfb[(i + x) + 320 * (j + 16 + y)] = jpegout[i + width * j];

    free(jpeg.outbuffer);
}

void DrawAt(int posX, int posY, JPEGIMG image){
    DisplayImage(image, posX, posY);
}

int main(int argc, char **argv) {
	VIDEO_Init();
	WPAD_Init();
	ASND_Init();
	MP3Player_Init();

	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb, 20, 20, rmode -> fbWidth, rmode -> xfbHeight, rmode -> fbWidth * VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);

	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();

	VIDEO_WaitVSync();

	if(rmode -> viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	printf("\x1b[2;0H");

	MP3Player_PlayBuffer(ayaya_mp3, ayaya_mp3_size, NULL);

    int pointerPosX = 300;
    int pointerPosY = 250;

	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();
        WPAD_SetVRes(0, 640, 480);
        WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

        WPAD_IR(0, &ir);
        if( ir.valid ) {
            pointerPosX = ir.x;
            pointerPosY = ir.y;
        }

        VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);
        DrawAt(pointerPosX, pointerPosY, pointer);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
