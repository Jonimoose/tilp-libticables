/*  libticables - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* 
 * General purpose virtual link cable unit (generic)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "cabl_def.h"
#include "export.h"

#if defined(__LINUX__) || defined(__MACOSX__)

/**************/
/* Linux part */
/**************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include "str.h"

#include "typedefs.h"
#include "export.h"
#include "cabl_err.h"
#include "cabl_def.h"
#include "cabl_ext.h"
#include "logging.h"

static int p;
static int ref_cnt = 0; // Counter of library instances

static int rd[2] = { 0, 0 }; // Pipe 0 <- 1 or 1 <- 0
static int wr[2] = { 0, 0 }; // Pipe 0 -> 1 or 1 -> 0

#define HIGH 666 // 2 of 3
#define LOW  333 // 1 of 3

static const char fifo_names[4][MAXCHARS] = 
{ 
  "/tmp/.vlc_1_0", "/tmp/.vlc_0_1", 
  "/tmp/.vlc_0_1", "/tmp/.vlc_1_0" 
};

static struct cs
{
  byte data;
  int available;
} cs;

int vtl_init()
{
  /* Init some internal variables */
  cs.available = 0; 
  cs.data = 0;

  /*
    if(io_addr > 2)
    return ERR_ILLEGAL_OP;
    else
    p = io_addr;
  */
  
  p = io_address;
  //fprintf(stderr, "io_address: %i\n", io_address);

  /* Check if the pipes already exist else create them */
  if(access(fifo_names[0], F_OK) | access(fifo_names[1], F_OK))
     {
       mkfifo(fifo_names[0], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
       mkfifo(fifo_names[1], O_RDONLY | O_WRONLY | O_NONBLOCK | S_IRWXU);
     }

  /* Open the pipes */
  // Open the 1->0 pipe in reading
  if((rd[p-1] = open(fifo_names[2*(p-1)+0], 
		     O_RDONLY | O_NONBLOCK)) == -1)
    {
      fprintf(stderr, "error: %s\n", strerror(errno));
      return ERR_OPEN_PIPE;
    }
  
  // Open the 0->1 pipe in writing (in reading at first)
  if((wr[p-1] = open(fifo_names[2*(p-1)+1],
		     O_RDONLY | O_NONBLOCK)) == -1)
    {
      return ERR_OPEN_PIPE;
    }
  if((wr[p-1] = open(fifo_names[2*(p-1)+1],
		     O_WRONLY | O_NONBLOCK)) == -1)
    {
      return ERR_OPEN_PIPE;
    }
  ref_cnt++;

  return 0;
}

int vtl_open()
{
  byte d;
  int n;

  /* Flush the pipe */
  do
    {
      n=read(rd[p-1], (void *)(&d), 1);
    }
  while(n > 0);

  return 0;
}

int vtl_put(byte data)
{
  int n = 0;
  clock_t clk;
  int b;
  struct stat s;

  /* Check if the other pipe is used */
  /*
  if(ref_cnt < 2)
    return ERR_OPP_NOT_AVAIL;
  */

  /* Transfer rate modulation */
  clk=clock();
  do
    {
      b = (clock()-clk) < time_out/10.0*CLOCKS_PER_SEC;
      if(!b) return ERR_SND_BYT_TIMEOUT;
      fstat(wr[p-1], &s);
      if(s.st_size > HIGH)
	n = 0;
      else if(s.st_size < LOW)
	n=1;
    }
  while(n <= 0);

  /* Write the data in a defined delay */
  clk=clock();
  do
    {
      b = (clock()-clk) < time_out/10.0*CLOCKS_PER_SEC;
      if(!b) return ERR_SND_BYT_TIMEOUT;
      n = write(wr[p-1], (void *)(&data), 1);
    }
  while(n <= 0);

  return 0;
}

int vtl_get(byte *data)
{
  static int n=0;
  clock_t clk;
  int b;

  if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

  // Read the byte in a defined delay
  clk=clock();
  do
    {
      b = (clock()-clk) < time_out/10.0*CLOCKS_PER_SEC;
		if(!b) return ERR_RCV_BYT_TIMEOUT;
      n = read(rd[p-1], (void *)data, 1);
    }
  while(n <= 0);

  if(n == -1)
    {
      return ERR_RCV_BYT;
    }

  return 0;
}

int vtl_probe()
{
  return 0;
}

int vtl_close()
{
  return 0;
}

int vtl_exit()
{
  if(rd[p-1])
    {
      /* Close the pipe */
      if(close(rd[p-1]) == -1)
	{
	  return ERR_CLOSE_PIPE;
	}
      rd[p-1] = 0;
    }
  if(wr[p-1])
    {
      /* Close the pipe */
      if(close(wr[p-1]) == -1)
	{
	  return ERR_CLOSE_PIPE;
	}
      wr[p-1] = 0;
    }
  ref_cnt--;
  
  return 0;
}

int vtl_check(int *status)
{
  int n = 0;
  
  /* Since the select function does not work, I do it myself ! */
  *status = STATUS_NONE;
  if(rd)
    {
      n = read(rd[p-1], (void *) (&cs.data), 1);
      if(n > 0)
        {
          if(cs.available == 1)
            return ERR_BYTE_LOST;
	  
          cs.available = 1;
          *status = STATUS_RX;
          return 0;
        }
      else
	{
          *status = STATUS_NONE;
          return 0;
        }
    }

  return 0;
}

int vtl_supported()
{
  return SUPPORT_OFF;
}

#elif defined(__WIN32__)

/************************/
/* Windows 32 bits part */
/************************/

#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "cabl_def.h"
#include "cabl_err.h"
#include "export.h"
#include "plerror.h"

extern int time_out; // Timeout value for cables in 0.10 seconds
extern int delay;    // Time between 2 bits (home-made cables only)
static int p;
static int ref_cnt = 0; // Counter of library instances

static const char *pipeName = "\\\\.\\pipe\\vtl";	// string: \\.\pipe\pipename

HANDLE hPipe;

static struct cs
{
  byte data;
  int available;
} cs;

int vtl_init(uint io_addr, char *dev)
{
	/* Check if valid argument */
	if(io_addr > 2)
		return ERR_ILLEGAL_OP;
	else
		p = io_addr;

	/* Create the pipe (in non-blocking mode) */
	hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_NOWAIT, 2, 256, 256, 
			0 * time_out, NULL);	// 100 * time_out
	if(hPipe == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "CreateNamedPipe\n");
		print_last_error();
		return ERR_OPEN_PIPE;
	}

  return 0;
}

int vtl_open()
{
	BOOL fSuccess;
	DWORD i;
	byte data;

	/* Flush the pipe */
	do
    {
	  fSuccess = ReadFile(hPipe, &data, 1, &i, NULL);
    }
	while(i > 0);

  return 0;
}

int vtl_put(byte data)
{
	DWORD i;
	BOOL fSuccess;

	// Write the data
	fSuccess=WriteFile(hPipe, &data, 1, &i, NULL);
	if(!fSuccess)
	{
		fprintf(stderr, "WriteFile\n");
		print_last_error();
		return ERR_SND_BYT;
	}
	else if(i == 0)
	{
		return ERR_SND_BYT_TIMEOUT;
	}

	return 0;
}

int vtl_get(byte *data)
{
	DWORD i;
	BOOL fSuccess;
	clock_t clk;
	int b;

	/* If the tig_check function was previously called, retrieve the byte */
	if(cs.available)
    {
      *data = cs.data;
      cs.available = 0;
      return 0;
    }

	clk=clock();
	do
    {
      b=(clock()-clk) < time_out/10.0*CLOCKS_PER_SEC;
      if(!b) return ERR_RCV_BYT_TIMEOUT;
	  fSuccess = ReadFile(hPipe,data,1,&i,NULL);
    }
	while(i != 1);

	return 0;
}

int vtl_close()
{
  return 0;
}

int vtl_exit()
{
	if(hPipe)
	{
		CloseHandle(hPipe);
		hPipe = 0;
	}
  
	return 0;
}

int vtl_probe()
{
	return 0;
}

int vtl_check(int *status)
{
	int n = 0;
	DWORD i;
	BOOL fSuccess;

	*status = STATUS_NONE;
	if(hPipe)
    {
	    // Read the data: return 0 if error and i contains 1 or 0 (timeout)
		fSuccess = ReadFile(hPipe, (&cs.data), 1, &i, NULL);
		if(fSuccess && (i==1))
		{
			if(cs.available == 1)
				return ERR_BYTE_LOST;

			cs.available = 1;
			*status = STATUS_RX;
			return 0;
		}
		else
		{
			*status = STATUS_NONE;
			return 0;
		}
    }  

	return 0;
}

int vtl_supported()
{
  return SUPPORT_ON;
}

#else

/************************/
/* Unsupported platform */
/************************/

int vtl_init()
{
  return 0;
}

int vtl_open()
{
  return 0;
}

int vtl_put(byte data)
{
  return 0;
}

int vtl_get(byte *d)
{
  return 0;
}

int vtl_probe()
{
  return 0;
}

int vtl_close()
{
  return 0;
}

int vtl_exit()
{
  return 0;
}

int vtl_check(int *status)
{
  return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1)) // swap the 2 lowest bits

int vtl_set_red_wire(int b)
{
  return 0;
}

int vtl_set_white_wire(int b)
{
  return 0;
}

int vtl_get_red_wire()
{
  return 0;
}

int vtl_get_white_wire()
{
  return 0;
}

int vtl_supported()
{
  return SUPPORT_OFF;
}

#endif
