/*
 *	utils.c is part of cxmb
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

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <psploadcore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "utils.h"

#define DRIVER_OFFSET 0x00002A4C

int isCtfFile(const char *str)
{
	if (str)
	{
		int len = strlen(str);
		if (len > 4) {
			return
				str[len - 4] == '.' &&
				str[len - 3] == 'c' &&
				str[len - 2] == 't' &&
				str[len - 1] == 'f';
		}
	}
	return 0;
}

int cmpistr(const char *str1, const char *str2)
{
	for (; *str1 && *str2; str1++, str2++)
	{
		if (tolower(*str1) != *str2)
			return 1;
	}
	if (*str1 != *str2)
		return 1;
	return 0;
}

void tolowerstr(char *str)
{
	for (; *str; ++str)
	{
		*str = tolower(*str);
	}
}

int truncpath(char *str1, const char *str2)
{
	char *ostr = strstr(str1, str2);
	if (ostr)
	{
		ostr[strlen(str2)] = 0;
		return 1;
	}
	return -1;
}

int readLine(int fd, char *buf, int max_len)
{
	int i = 0, bytes = 0;
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	while (i < max_len && (bytes = sceIoRead(fd, buf + i, 1)) == 1)
	{
		if (buf[i] == -1 || buf[i] == '\n')
			break;
		i++;
	}
	buf[i] = 0;
	if (bytes != 1 && i == 0)
		return -1;
	return i;
}

unsigned int getFindDriverAddr(void)
{
	tSceModule *pMod = (tSceModule *)sceKernelFindModuleByName("sceIOFileManager");
	return pMod ? pMod->text_addr + DRIVER_OFFSET : 0;
}

PspIoDrv *findDriver(char *drvname)
{
	unsigned int *(*getDevice)(char *) = (void *)getFindDriverAddr();
	if (!getDevice)
		return NULL;
	unsigned int *u;
	u = getDevice(drvname);
	if (!u)
		return NULL;
	log("%s found!\nu0: %08x\nu1: %08x\nu2: %08x\nu3: %08x\n",
		drvname, u[0], u[1], u[2], u[3]);
	return (PspIoDrv *)u[1];
}

unsigned int *findExport(const char *szMod, const char *szLib, unsigned int nid)
{
	SceLibraryEntryTable *entry;
	tSceModule *pMod;
	void *entTab;
	int entLen;
	pMod = (tSceModule *)sceKernelFindModuleByName(szMod);
	if (!pMod)
	{
		return 0;
	}
	int i = 0;
	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
	while (i < entLen)
	{
		int count;
		int total;
		unsigned int *vars;

		entry = (SceLibraryEntryTable *)(entTab + i);

		if (entry->libname && (!szLib || !strcmp(entry->libname, szLib)))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			for (count = 0; count < entry->stubcount; count++)
			{
				if (vars[count] == nid)
				{
					return &vars[count + total];
				}
			}
		}

		i += (entry->len * 4);
	}
	return NULL;
}

unsigned int findProc(const char *szMod, const char *szLib, unsigned int nid)
{
	unsigned int *export = findExport(szMod, szLib, nid);
	if (export)
	{
		log("func %08x in %s of %s found:\n%08x\n",
			nid, szLib, szMod, *export);
		return *export;
	}
	return 0;
}

StartModuleHandler (*setStartModuleHandler)(StartModuleHandler);
int (*rebootPsp)(void);

void initUtils(void)
{
	setStartModuleHandler = (void *)findProc("SystemControl", "SystemCtrlForKernel", 0x1C90BECB);
	rebootPsp = (void *)findProc("scePower_Service", "scePower", 0x0442D852);
}
