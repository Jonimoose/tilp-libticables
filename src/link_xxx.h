/* Hey EMACS -*- linux-c -*- */
/* $Id: cables.h 370 2004-03-22 18:47:32Z roms $ */

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

#ifndef LINKXXX_H
#define LINKXXX_H

#include "cabl_def.h"

int dfl_register_cable(TicableLinkCable * lc, TicableMethod method);
int dfl_unregister_cable(TicableLinkCable * lc);

int avr_register_cable(TicableLinkCable * lc, TicableMethod method);
int avr_unregister_cable(TicableLinkCable * lc);

int dev_register_cable(TicableLinkCable * lc, TicableMethod method);
int dev_unregister_cable(TicableLinkCable * lc);

int par_register_cable(TicableLinkCable * lc, TicableMethod method);
int par_unregister_cable(TicableLinkCable * lc);

int ser_register_cable_1(TicableLinkCable * lc, TicableMethod method);
int ser_unregister_cable_1(TicableLinkCable * lc);

int ser_register_cable_2(TicableLinkCable * lc, TicableMethod method);
int ser_unregister_cable_2(TicableLinkCable * lc);

int slv_register_cable(TicableLinkCable * lc, TicableMethod method);
int slv_unregister_cable(TicableLinkCable * lc);

int tie_register_cable(TicableLinkCable * lc, TicableMethod method);
int tie_unregister_cable(TicableLinkCable * lc);

int tig_register_cable(TicableLinkCable * lc, TicableMethod method);
int tig_unregister_cable(TicableLinkCable * lc);

int vti_register_cable(TicableLinkCable * lc, TicableMethod method);
int vti_unregister_cable(TicableLinkCable * lc);

int vtl_register_cable(TicableLinkCable * lc, TicableMethod method);
int vtl_unregister_cable(TicableLinkCable * lc);

#endif