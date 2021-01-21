/*
 *	utils.h is part of cxmb
 *	Copyright (C) 2008  Poison
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *	Description:	
 *	Author:			Poison <hbpoison@gmail.com>
 *	Date Created:	2008-07-01
 */

//patpat mod for 3.71 - 6.37 <2011-02-19>
//neur0n mod for 6.39
//frostegater mod for 6.60
//LMAN mod for 6.61
//Yoti mod for INFINITY

#pragma once

#include <pspkernel.h>
#include <psploadexec_kernel.h>

typedef struct tSceModule
{
	struct tSceModule *next;
	unsigned short attribute;
	unsigned char version[2];
	char modname[27];
	char terminal;
	unsigned int unknown1;
	unsigned int unknown2;
	int modid;
	unsigned int unknown3[4];
	void *ent_top;
	unsigned int ent_size;
	void *stub_top;
	unsigned int stub_size;
	unsigned int unknown4[5];
	unsigned int entry_addr;
	unsigned int gp_value;
	unsigned int text_addr;
	unsigned int text_size;
	unsigned int data_size;
	unsigned int bss_size;
	unsigned int nsegment;
	unsigned int segmentaddr[4];
	unsigned int segmentsize[4];
} tSceModule;

typedef int (*StartModuleHandler)(tSceModule *);

extern StartModuleHandler (*setStartModuleHandler)(StartModuleHandler);

extern int (*rebootPsp)(void);

extern int isCtfFile(const char *str);

extern int cmpistr(const char *str1, const char *str2);

extern int truncpath(char *str1, const char *str2);

extern int readLine(int fd, char *buf, int max_len);

extern PspIoDrv *findDriver(char *drvname);

extern void initUtils(void);
