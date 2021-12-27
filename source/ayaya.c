#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <mp3player.h>
#include "ayaya_mp3.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

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

	printf("ayaya hyperclap\n");

	MP3Player_PlayBuffer(ayaya_mp3, ayaya_mp3_size, NULL);

	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
