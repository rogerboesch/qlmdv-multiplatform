/*
	QLAY - Sinclair QL emulator
	Copyright Jan Venema 1998
	QLAY TOOLS include and define
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define QDOSTIME ((9*365+2)*86400) /* 0.82c forget about GMT&DST offsets */
#define	LINESIZE 256
#define	QDOSSIZE 37
#define	DOSSIZE 14
#define	SECTLENQXL 0x800	/* sector length in QXL */

typedef unsigned long U32;
typedef unsigned short U16;
typedef unsigned char U8;

/* shared usage among options */
extern char  ifname[LINESIZE],lstline[LINESIZE],qdosname[QDOSSIZE],dosname[DOSSIZE];
extern char  lstline2[LINESIZE];
extern U8	 sector[SECTLENQXL];
extern FILE* qxldf;
extern int	 randmdv;
extern char	 lstfname[256];
extern char	 outfname[256];
extern char	 dirfname[256];

extern char  filenames[256][37];
extern char  temppath[256];

extern void putlong(U8*,U32);
extern void putword(U8*,U16);
extern U32	getlong(U8*);
extern U16	getword(U8*);
extern void usage(void);
extern int	getxtcc(FILE*,U32*);

extern void resetglobals(void);
extern int fil2mdv(char*,char*);
extern int mdv2fil(char*,int);
