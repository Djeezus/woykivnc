/*
WyokiVNC
Copyright (C) 2011 Jose Pereira <onaips@gmail.com>
Added Wyoki functionality 2013 Gert Vandelaer <djeezus@godsells.org>

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

#ifndef KEYMANIP_H
#define KEYMANIP_H

#include "common.h"
#include "gui.h"

#include "rfb/rfb.h"
#include "suinput.h"

#define BUS_VIRTUAL 0x06

void initInput();
int keysym2scancode(rfbBool down, rfbKeySym c, rfbClientPtr cl, int *sh, int *alt);
void transformTouchCoordinates(int *x, int *y,int,int);
void ptrEvent(int buttonMask, int x, int y, rfbClientPtr cl);
void keyEvent(rfbBool down, rfbKeySym key, rfbClientPtr cl);
void cleanupInput();

#endif
