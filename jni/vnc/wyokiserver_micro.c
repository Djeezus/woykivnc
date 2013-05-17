/*
Wyoki VNC pusher
Copyright (C) 2013 Gert Vandelaer <gert.vandelaer@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "common.h"
#include "framebuffer.h"

#include "gui.h"
#include "input.h"

#include "libvncserver/scale.h"
#include "rfb/rfb.h"
#include "rfb/keysym.h"
#include "suinput.h"

static const int bpp = 4 ;
static int mX = 128, mY = 160 ;

/* Create structure to give pointer to each client */
typedef struct ClientData
{
    rfbBool oldButton;
    int oldx, oldy;
} ClientData;

static void clientgone (rfbClientPtr cl)
{
    free(cl->clientData);
}

static enum rfbNewClientAction newclient (rfbClientPtr cl)
{
    cl->clientData = (void*)calloc(sizeof(ClientData), 1);
    cl->clientGoneHook = clientgone;
    return RFB_CLIENT_ACCEPT;
}

/* Switch to new framebuffer content */

static void newframebuffer (rfbScreenInfoPtr screen, int width, int height)
{
    unsigned char *oldfb, *newfb;

    mX = width;
    mY = height;
    oldfb = (unsigned char*)screen->frameBuffer;
    newfb = (unsigned char*)malloc(mX * mY * bpp);
    rfbNewFramebuffer(screen, (char*)newfb, mX, mY, 8, 3, bpp);
    free(oldfb);
}

void close_app()
{
  fprintf(stderr, "Cleaning up...\n");
  closeFB();
  cleanupInput();
  unbindIPCserver();
  exit(0); /* normal exit status */
}

/* Initialization */
int runonce = 0;

int main (int argc, char** argv)
{
	
initFB() ;
fprintf(stderr, "Init FB ...\n");

initInput();
fprintf(stderr, "Init Input ...\n");

bindIPCserver();
fprintf(stderr, "Bind IPC server ... \n");

    rfbScreenInfoPtr rfbScreen = rfbGetScreen(&argc, argv, mX, mY, 8, 3, bpp);
    if (!rfbScreen)
        return 0;
    rfbScreen->desktopName = "Wyoki daemon";
    rfbScreen->frameBuffer = (char*)malloc(mX * mY * bpp);
    rfbScreen->alwaysShared = TRUE;
    rfbScreen->newClientHook = newclient;
    rfbScreen->port = 6789 ;

    /* initialize the server */
    rfbInitServer(rfbScreen);

    /* this is the non-blocking event loop; a background thread is started */
    rfbRunEventLoop(rfbScreen, -1, TRUE);
    fprintf(stderr, "Running background loop...\n");

    const char * filename = "/sdcard/tmp/pixelbuffer.bin";
    unsigned char pixelbuffer[ mX * mY * bpp ];
    while (1)
    {
        sleep(5);
	fprintf(stderr, "slept 5, Looping ... \n");
        FILE * ft = fopen(filename, "rb");
        if (ft)
        {
		fprintf(stderr,"fread  pixelbuffer ...\n");
            fread(pixelbuffer, 1, mX * mY * bpp, ft);

            if (!runonce)
            {
		fprintf(stderr,"if !runonce ...\n");
                runonce = 1;
                memcpy(rfbScreen->frameBuffer, pixelbuffer, mX * mY * bpp);
            }

            int modx0 = -1, modx1 = -1, mody0 = -1, mody1 = -1;
            int j, i;

            // find the smallest (single) rectangle that covers all the modified pixels
#if 1
            for (j = 0; j < mY; j++)
            {
                for (i = 0; i < mX; i++)
                {
                    if (pixelbuffer[ j * mX * bpp + i * bpp + 0 ] != (unsigned char)rfbScreen->frameBuffer[ j * mX * bpp + i * bpp + 0 ]
                     || pixelbuffer[ j * mX * bpp + i * bpp + 1 ] != (unsigned char)rfbScreen->frameBuffer[ j * mX * bpp + i * bpp + 1 ]
                     || pixelbuffer[ j * mX * bpp + i * bpp + 2 ] != (unsigned char)rfbScreen->frameBuffer[ j * mX * bpp + i * bpp + 2 ])
                    {
                        if (modx0 < 0 || i < modx0)
                            modx0 = i;
                        if (i > modx1)
                            modx1 = i + 2;
                        if (mody0 < 0 || j < mody0)
                            mody0 = j;
                        if (j > mody1)
                            mody1 = j + 2;

                    }
                }
            }
#endif
            // if there were any modified pixels, mark the rectangle
            if (modx0 >= 0)
            {
		fprintf(stderr, "Modified pixels found ... mark the rectangle...\n");
                memcpy(rfbScreen->frameBuffer, pixelbuffer, mX * mY * bpp);

                rfbMarkRectAsModified(rfbScreen, modx0, mody0, modx1, mody1);
            }
            fclose(ft);
        }
        else
        {
            printf("couldn't open file");
            return -1;
        }
    }

    free(rfbScreen->frameBuffer);
    rfbScreenCleanup(rfbScreen);

    return (0);
}
