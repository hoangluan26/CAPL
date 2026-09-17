/*****************************************************
 *
 * test.c
 *
 * Description : Hello World in C, ANSI-style
 *
 */

#include <stdio.h>
#include "HKMC_ASK_Server.h"


int main(int argc, char *argv[])
{
uint8 seed[11][8] = { { 0 },
		{ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 },
		{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 },
		{ 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 },
		{ 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA },
		{ 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB },
		{ 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC },
		{ 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD },
		{ 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE },
		{ 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF },
		{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
		};
	    uint8 key[8];
	    //unsigned int dist[256] = {0};
	    SEEDKEY_RT ret;
	    VERSION_INFO ver;
	    int i, j;

        printf(" KEY generate...\r\n");

	    for (i=1; i<11; i++) {
	        ret =  ASK_KeyGenerate(seed[i], key);
	        if (ret){
	            printf("KEY generate fail\r\n");
	        }
	        else {
	            for (j=0; j<8; j++){
	                printf("%02x ", key[j]);
	            }
	         }
	        putchar('\n');
	    }
	    putchar('\n');

	    #ifdef VERSION_INFO_AVAILABLE
	    vGetVersionInfo(&ver);
	    printf("vendor: %x, module: %x, version: %d.%d.%d\n",
		    ver.vendorID, ver.moduleID, ver.majorVersion, ver.minorVersion, ver.patchVersion);
	#endif
	#if 0
	    j=0;
	    while (j++ < 10000)   {
	        ret = ASK_Rand8ByteGenerate(seed);
	        for (i=0; i<8; i++){
	          if (j%100 == 0) printf("%x ", seed[i]);
	          dist[seed[i]]++;
	        }
	        if (j%100 ==0) printf("\r\n");
	    }
	    for (i=0; i<256; i++)       {
	      printf("[%x]: [%d]", i, dist[i]);
	      (i%4==3)? printf("\r\n"):printf("\t");
	    }
	#endif
	return 0;
}
