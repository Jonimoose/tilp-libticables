/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

/* Resources mapping wrapper */

/*
  This unit compiles cable type and resources to determine the best way to use
  the link cable:
  - I/O method
  - cable access.  
  It also permforms link cable (un)registering.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(__LINUX__)
#include "linux/linux_mapping.c"
#elif defined(__MACOSX__)
#include "macos/macos_mapping.c"
#elif defined(__BSD__)
#include "bsd/bsd_mapping.c"
#elif defined(__WIN32__)
#include "win32/win32_mapping.c"
#else
#include "none.h"
#endif


int mapping_get_method(TicableType type, int resources, TicableMethod *method)
{
	int ret;
	
#if defined(__LINUX__)
	ret = linux_get_method(type, resources, method);
#elif defined(__BSD__)
	ret = bsd_get_method(type, resources, method);
#elif defined(__WIN32__)
	ret = win32_get_method(type, resources, method);
#elif defined(__MACOSX__)
	ret = macos_get_method(type, resources, method);
#else
	ret = 0;
#endif
  	return ret;
}

int mapping_register_cable(TicableType type, TicableLinkCable *lc)
{
	int ret;
	
#if defined(__LINUX__)
	ret = linux_register_cable(type, lc);
#elif defined(__BSD__)
	ret = bsd_register_cable(type, lc);
#elif defined(__WIN32__)
	ret = win32_register_cable(type, lc);
#elif defined(__MACOSX__)
	ret = macos_register_cable(type, lc);
#else
	ret = 0;
#endif
  	return ret;
}

int nul_register_cable(TicableLinkCable * lc);

int mapping_unregister_cable(TicableLinkCable *lc)
{
	return nul_register_cable(lc);	
}
