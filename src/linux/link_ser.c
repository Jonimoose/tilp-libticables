/* Hey EMACS -*- linux-c -*- */
/* $Id: link_ser2.c 1033 2005-05-06 18:17:02Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

/* "Home-made serial" link & "Black TIGraphLink" link unit (low-level I/O routines) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "detect.h"
#include "ioports.h"

#define dev_fd  ((int)(h->priv))

static int ser_prepare(CableHandle *h)
{
    switch(h->port)
    {
    case PORT_1: h->address = 0x3f8; h->device = strdup("/dev/ttyS0");
	break;
    case PORT_2: h->address = 0x2f8; h->device = strdup("/dev/ttyS1");
	break;
    case PORT_3: h->address = 0x3e8; h->device = strdup("/dev/ttyS2");
	break;
    case PORT_4: h->address = 0x3e8; h->device = strdup("/dev/ttyS3");
	break;
    default: return ERR_ILLEGAL_ARG;
    }

    // detect stuffs
    //TRYC(check_for_tty(h->device));


    return 0;
}

static int ser_open(CableHandle *h)
{
    TRYC(ser_io_open(h->device, (int *)&(h->priv)));
    return 0;
}

static int ser_close(CableHandle *h)
{
    TRYC(ser_io_close(dev_fd));
    return 0;
}

static int ser_reset(CableHandle *h)
{
    ser_io_wr(dev_fd, 3);
    return 0;
}

static int ser_put(CableHandle *h, uint8_t *data, uint16_t len)
{
    int bit;
    int i, j;
    tiTIME clk;
    
    for(j = 0; j < len; j++)
    {
	uint8_t byte = data[j];
	
	for (bit = 0; bit < 8; bit++) 
	{
	    if (byte & 1) 
	    {
		ser_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10))
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    
		ser_io_wr(dev_fd, 3);
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10) == 0x00);
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_WRITE_TIMEOUT;
		};
	    }
	    else
	    {
		ser_io_wr(dev_fd, 1);
                TO_START(clk);
		while (ser_io_rd(dev_fd) & 0x20)
		{  
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
		};
		
                ser_io_wr(dev_fd, 3);
                TO_START(clk);
                while ((ser_io_rd(dev_fd) & 0x20) == 0x00)
		{
		    if (TO_ELAPSED(clk, h->timeout))
                        return ERR_WRITE_TIMEOUT;
                };
	    }
	    
	    byte >>= 1;
	    for (i = 0; i < h->delay; i++)
		ser_io_rd(dev_fd);
	}
    }
    
    return 0;
}

static int ser_get(CableHandle *h, uint8_t *data, uint16_t len)
{
    int bit;
    int i, j;
    tiTIME clk;
    
    for(j = 0; j < len; j++)
    {
	uint8_t v, byte = 0;
  	
	for (bit = 0; bit < 8; bit++) 
	{
	    TO_START(clk);
	    while ((v = ser_io_rd(dev_fd) & 0x30) == 0x30) 
	    {
		if (TO_ELAPSED(clk, h->timeout))
		    return ERR_READ_TIMEOUT;
	    }
	    
	    if (v == 0x10) 
	    {
		byte = (byte >> 1) | 0x80;
		ser_io_wr(dev_fd, 1);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x20) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		ser_io_wr(dev_fd, 3);
	    } 
	    else 
	    {
		byte = (byte >> 1) & 0x7F;
		ser_io_wr(dev_fd, 2);
		
		TO_START(clk);
		while ((ser_io_rd(dev_fd) & 0x10) == 0x00) 
		{
		    if (TO_ELAPSED(clk, h->timeout))
			return ERR_READ_TIMEOUT;
		}
		ser_io_wr(dev_fd, 3);
	    }
	    
	    for (i = 0; i < h->delay; i++)
		ser_io_rd(dev_fd);
	}
	
	data[j] = byte;
    }
    
    return 0;
}

static int ser_probe(CableHandle *h)
{
    int timeout = 1;
    tiTIME clk;
    
    // 1
    ser_io_wr(dev_fd, 2);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x10))
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    ser_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x10) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    // 0
    ser_io_wr(dev_fd, 1);
    TO_START(clk);
    while (ser_io_rd(dev_fd) & 0x20)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    ser_io_wr(dev_fd, 3);
    TO_START(clk);
    while ((ser_io_rd(dev_fd) & 0x20) == 0x00)
    {
	if (TO_ELAPSED(clk, timeout))
	    return ERR_WRITE_TIMEOUT;
    };
    
    return 0;
}

static int ser_check(CableHandle *h, int *status)
{
	*status = STATUS_NONE;

  	if (!((ser_io_rd(dev_fd) & 0x30) == 0x30)) 
    		*status = (STATUS_RX | STATUS_TX);

	return 0;
}

#define swap_bits(a) (((a&2)>>1) | ((a&1)<<1))	// swap the 2 lowest bits

static int ser_set_red_wire(CableHandle *h, int b)
{
	int v = swap_bits(ser_io_rd(dev_fd) >> 4);

  	if (b)
    		ser_io_wr(dev_fd, v | 0x02);
  	else
    		ser_io_wr(dev_fd, v & ~0x02);

	return 0;
}

static int ser_set_white_wire(CableHandle *h, int b)
{
	int v = swap_bits(ser_io_rd(dev_fd) >> 4);

  	if (b)
    		ser_io_wr(dev_fd, v | 0x01);
  	else
    		ser_io_wr(dev_fd, v & ~0x01);

	return 0;
}

static int ser_get_red_wire(CableHandle *h)
{
	return ((0x10 & ser_io_rd(dev_fd)) ? 1 : 0);
}

static int ser_get_white_wire(CableHandle *h)
{
	return ((0x20 & ser_io_rd(dev_fd)) ? 1 : 0);
}

const CableFncts cable_ser = 
{
	CABLE_BLK,
	"BLK",
	N_("BlackLink"),
	N_("BlackLink or home-made serial cable"),
	!0,
	&ser_prepare,
	&ser_open, &ser_close, &ser_reset, &ser_probe,
	&ser_put, &ser_get, &ser_check,
	&ser_set_red_wire, &ser_set_white_wire,
	&ser_get_red_wire, &ser_get_white_wire,
};
