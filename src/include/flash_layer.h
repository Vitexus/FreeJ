/////////////////////////////////////////////////////////////
// Flash Plugin and Player
// Copyright (C) 1998 Olivier Debon
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
///////////////////////////////////////////////////////////////
//  Author : Olivier Debon  <odebon@club-internet.fr>
//  Ported to FreeJ by jaromil
//
//  "$Id$"

#ifndef __FLASH_LAYER_H__
#define __FLASH_LAYER_H__

#include <stdio.h>
#include <layer.h>
#include <flash.h>

class FlashLayer : public Layer {

 public:
  FlashLayer();
  ~FlashLayer();
  
  bool init(int width, int height);
  bool open(char *file);
  void *feed();
  bool keypress(int key);
  void close();
  
  
 private:
  
  void *procbuf;
  void *render;

  FlashHandle fh;

  FlashDisplay fd;

  struct FlashInfo fi;

  FlashEvent fe;

  long flag;



  struct timeval *wakeDate;

  FILE *filedesc;
  char *filename;  
  long *size;
};

#endif
