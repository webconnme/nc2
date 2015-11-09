//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		:
//	File		:
//	Description	:
//	Author		: Goofy
//	Export		:
//	History		:
//------------------------------------------------------------------------------
#ifndef __MEMINIT_H__
#define	__MEMINIT_H__

#include <nx_type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_BOOTINFO
{
	int	iDownloadSize;
	U32	dwDownloadAddr;
	U32	dwJumpAddr;
} BOOTINFO;

CBOOL	ProcessBootHeader( U32 *pBootHeader, BOOTINFO *pBootInfo );
void	MemoryInitWithRstCfg( unsigned int ResetConfig );

#ifdef __cplusplus
}
#endif

#endif	//	__MEMINIT_H__
