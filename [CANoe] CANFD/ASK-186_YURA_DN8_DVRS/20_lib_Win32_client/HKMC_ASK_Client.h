µÎ¼Ò¤Å\/***************************************************************************/
/*!

   \file        HKMC_ASK_Client.h

   \title       Advanced Seedkey Client Header File

   \version		v1.2

 */
/***************************************************************************/


#ifndef __ADV_SEED_KEY_CLIENT_H__
#define __ADV_SEED_KEY_CLIENT_H__

#undef _T
#ifdef _UNICODE
#define _T(x)      L##x
#else
#define _T(x)      x
#endif 

typedef unsigned char BYTE;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef enum{
	SEEDKEY_SUCCESS = 0,
	SEEDKEY_FAIL = 1,
} SEEDKEY_RT;

#ifndef VERSION_INFO_AVAILABLE
#define VERSION_INFO_AVAILABLE
#endif

typedef struct {
	uint16 vendorID;
	uint16 moduleID;
	uint8 majorVersion;
	uint8 minorVersion;
	uint8 patchVersion;
} VERSION_INFO;

typedef __declspec(dllimport) SEEDKEY_RT(*KeyGenerateAPI)(const BYTE *seed, BYTE *key);
typedef __declspec(dllimport) void(*VersionAPI)(VERSION_INFO *pVersionInfo);

#endif