/*
	QLAY - Sinclair QL emulator
	Copyright Jan Venema 1998
	QLAY TOOLS mdv
    Refactored and optimised by Roger Boesch
*/

#include "qlayt.h"

#define NOSECTS 255
#define SECTLEN (14+14+512+26+120)
#define FNO	0x28
#define BNO	0x29
#define SNO	0x0d
#define SOF	0x34

extern char temppath[256];

U8 mdv[NOSECTS][SECTLEN];
char filenames[256][37];
int filehist[256];
int filemaxn[256];
U32 filelen[256];
int	cdirs, cdirf, cdblock, cfile, cblock, csect, noif;
U32	dirlength;

// MARK: - Helper

/* note! qxl parts use fnum[]! */
static void output_file(int fnum) {
    FILE *of;
    char ofname[256];
    int	i, blk, number, n;
    U32	flen, wsect, headoff;

	ofname[255]='\0';
    strcpy(ofname, temppath);
    strcat(ofname, filenames[fnum]);

	if ((of = fopen(ofname,"wb")) == NULL) {
		sprintf(ofname,"file_%03d", fnum);
		
        if ((of = fopen(ofname,"wb")) == NULL) {
			printf("Cannot open '%s'\n", ofname);
			return;
		}
	}

	flen = filelen[fnum];

    for (blk = 0; blk < filemaxn[fnum]+1; blk++) {
		for (i = 0; i < NOSECTS; i++) {
			if ((mdv[i][FNO] == fnum) && (mdv[i][BNO] == blk)) {
				wsect = 512;

                if (flen < wsect) {
                    wsect = flen;
                }

                if (blk == 0) {
					wsect -= 64;
                    headoff = 64;
                    
					n = (int)fwrite(&mdv[i][0x34], sizeof(U8), 64, qxldf);
				
                    if (n!=64) {
						printf("Cannot write directory file %s\n", dirfname);
						return;
					}
				}
                else {
                    headoff = 0;
                }
                
				number = (int)fwrite(&mdv[i][0x34+headoff],sizeof(char),wsect, of);

                if (number != (int)wsect) {
                    /* kch! */
					printf("Cannot write %s\n", ofname);
					fclose(of);
					return;
				}

                flen -= 512;
				break;
			}
		}
	}
    
	fclose(of);
}

static int print_map(int create) {
    int	i, s;
    U8 *p;
    int	sflag, sno, fno, bno, fno0, bno0;
    int	numfiles, validf, num;

	for (i = 0; i < 256; i++) {
		filehist[i] = 0;
		filemaxn[i] = 0;
		filenames[i][36]='\0';
	}

	for (s = 0; s < NOSECTS; s++) {
		p = mdv[s];
		sflag = p[0x0c];
		sno = p[SNO];
		fno = p[FNO];
		bno = p[BNO];
		fno0 = mdv[0][0x34+sno*2];
		bno0 = mdv[0][0x34+sno*2+1];

        if ((sflag == 0xff) && (fno0 != 0xfd) && (fno0 != 0xff)) {
			filehist[fno]++;

            if (filemaxn[fno] < bno) {
                filemaxn[fno] = bno;
            }
            
			if ((fno > 0) && (fno < 0xf8) && (bno == 0)) {
				i = p[0x34+15];
				strncpy(filenames[fno], &p[0x34+16], i);
				filelen[fno] = getlong(&p[0x34+0]);
			}
		}
	}

	numfiles = 0;
	for (i = 0; i < 256; i++) {
		validf = 0;

        if (filehist[i] != 0) {
            if ((i > 0) && (i < 0xf8)) {
				validf = 1;
			}

			if ((i != 0xfd) && (i != 0xf9)) {
				num = filehist[i];

                if (filehist[i] != filemaxn[i]+1) {
					validf = 0;
				}
			}

            numfiles++;
			
            if (validf) {
				if (filelen[i] < 256*512) {
                    /* = mdv max */
                    if (create) {
                        output_file(i);
                    }
                    else {
                        printf(">%s\n", filenames[i]);
                    }
				}
			}
		}
	}
    
    return numfiles;
}

static void write_dir_len(void) {
	putlong(&mdv[1][0]+SOF, dirlength);
}

static void put_dir(void) {
    int i;
	
    if ((cdirf > 0)&&((cdirf%8)==0)) {
		cdblock++;
		cdirs++;
		cdirf = 0;
	}
	for (i = 0; i < 64; i++) {
		mdv[cdirs][SOF+i+cdirf*64] = sector[i];
	}

    /* update block header */
	mdv[cdirs][0x28] = 0;
	mdv[cdirs][0x29] = cdblock;

	/* update map */
	mdv[0][SOF+2*cdirs] = 0;
	mdv[0][SOF+2*cdirs+1] = cdblock;

	dirlength += 64;
	cdirf++;
	cblock = 0;
}

static void init_mdv(void) {
    int s, b, rand;

	for (s = 0; s < NOSECTS; s++) {
		for (b = 0; b < SECTLEN; b++) {
			mdv[s][b] = 0;
		}
	}
	cdirs = 1;	    /* sector 1, after map */
	cdirf = 0;	    /* first file in dir */
	cdblock = 0;	/* dir block number */
	cfile = 1;	    /* fileNR 1 */
	csect = 2;	    /* sector 2, after dir */
    
	/* noif is the number of files to be placed in the directory */
	if (noif > 7) {
		csect += noif/8;
	}
    
	dirlength = 0;

	/* create map */
	for (b = 0;b<512;b+=2) {
		mdv[0][SOF+b]=0xfd;	/* all vacant */
		mdv[0][SOF+b+1]=0x00;
	}

    mdv[0][0x28]=0xf8;	/* block head */
	mdv[0][SOF+0]=0xf8;	/* mdv map */
	mdv[0][SOF+2*255]=0xff;	/* not available */

    /* init sector header: mdv name, random */
	if (randmdv == -1) {
		rand = random()&0xffff;
	}
    else {
		/* convert endian style */
		rand = ((randmdv>>8)&0xff) | ((randmdv<<8)&0xff00);
	}

    for (s = 0; s < NOSECTS; s++) {
		mdv[s][0x0c] = 0xff;
		mdv[s][0x0d] = s;

        for (b = 0; b < 10; b++) {
			mdv[s][0x0e +b]=' ';
        }
        
		b = strlen(outfname);
        
        if (b > 10) {
            b = 10;
        }
		
        strncpy(&mdv[s][0x0e], outfname, b);
		mdv[s][0x18] = rand&0xff;
		mdv[s][0x19] = rand>>8;
		
        /* block header */
		mdv[s][0x28]=0xfd;
	}

    for (b = 0; b < 512; b++) {
        sector[b] = 0;
    }

    put_dir();	/* create first empty dir header */
}

static void put_sect(int sectlen) {
    int i;
    
	if (csect >= NOSECTS) {
		printf("MDV full\n");
        return;
	}
	
    for(i = 0; i <sectlen; i++) {
		mdv[csect][SOF+i] = sector[i];
	}
	
    /* update block header */
	mdv[csect][0x28] = cfile;
	mdv[csect][0x29] = cblock;

	/* update map */
	mdv[0][SOF+2*csect] = cfile;
	mdv[0][SOF+2*csect+1] = cblock;

	cblock++;
	csect++;
}

static void write_mdv(U8 *buffer, FILE *outfile) {
    int i;
    unsigned int sum;

	/* sect head pll */
    for (i = 0;i<10;i++) {
        putc(0, outfile);
    }
    
	putc(0xff, outfile);
	putc(0xff, outfile);
	
    /* sect head */
	sum = 0x0f0f;
	
    for (i = 0; i < 14; i++) {
		putc(buffer[i+0x0c], outfile);
		sum += buffer[i+0x0c];
	}

    putc(sum&0xff, outfile);
	putc((sum>>8)&0xff, outfile);

	/* block head pll */
    for (i = 0;i<10;i++) {
        putc(0, outfile);
    }
    
	putc(0xff, outfile);
	putc(0xff, outfile);

    /* block head */
	sum = 0x0f0f;

    for (i = 0; i< 2; i++) {
		putc(buffer[i+0x28], outfile);
		sum += buffer[i+0x28];
	}
    
	putc(sum&0xff, outfile);
	putc((sum>>8)&0xff, outfile);

	/* sector pll */
    for (i = 0; i < 6; i++) {
        putc(0, outfile);
    }
    
	putc(0xff, outfile);
	putc(0xff, outfile);

    /* sector */
	sum = 0x0f0f;
	for(i = 0; i < 512; i++) {
		putc(buffer[i+SOF], outfile);
		sum+=buffer[i+SOF];
	}

    putc(sum&0xff, outfile);
	putc((sum>>8)&0xff, outfile);

	/* filler */
	for (i = 0; i < 0x78; i++) {
		putc(0x5a, outfile);
	}
}

/* generate a file header for file 'ifname' and store in head, at least 64 chars*/
/* return -1 if error, 0 OK */
static int genheader(char *ifname, U8 *head, int dz, U32 datasize) {
    FILE *infile;
    int	i, fnlen;
    struct stat s;

    char path[256];
    path[255]='\0';
    strcpy(path, temppath);
    strcat(path, ifname);

	/* is 'ifname' readable */
	if ((infile = fopen(path,"rb")) == NULL) {
		printf("cannot open %s, skipping\n", ifname);
		return -1;
	}

	/* prepare the header */
    for (i = 0; i < 64; i++) {
        head[i]=0;
    }
    
	/* get file length */
	fseek(infile, 0L, 2);
	putlong(head, ftell(infile)+64);	/* +64 ! */

	fnlen = (int)strlen(ifname);

    if (fnlen > 36) {
		printf("Error, filename too long %s, %d\n", ifname, fnlen);
		return -1;
	}

    putword(&head[14],fnlen);
	strncpy(&head[16],ifname, 36);

    if (dz == 2) {
        if (getxtcc(infile, &datasize) != 0) {
            return -1;
        }
    }
    
	if (dz) {
		putlong(&head[6], datasize);
		head[5] = 1;
	}

	/* find the file's modification time */
	if (stat(path, &s) != 0) {
		printf("error: cannot stat %s, skipping\n", ifname);
		fclose(infile);

        return -1;
	}

	putlong(&head[0x34], s.st_mtime+QDOSTIME);

	printf("%-36s\n", ifname);
    
	fclose(infile);
	
    return 0;
}

// MARK: - MDV functions

int fil2mdv(char *fname, char *ofname) {
    int	number, s, ai, no_ascii_file, i;
    FILE *lstfile, *infile, *outfile;
    int	dz = 0, noif, noof;
    char *p, *q;
    U32	datasize;

	if ((lstfile = fopen(fname, "r")) == NULL) {
		printf("Error: cannot open %s\n", fname);
		return -1;
	}

	if ((outfile = fopen(ofname, "wb")) == NULL) {
		printf("cannot open %s\n", ofname);
		return -1;
	}

	noif = 0;

    /* how many lines? */
	while (fgets(ifname, LINESIZE-1, lstfile) != NULL) {
		noif++;
		no_ascii_file = 0;

        for (i = 0; i < (int)strlen(ifname); i++) {
			if (!(isascii(ifname[i]))) {
				no_ascii_file = 1;
				break;
			}
		}

        if (no_ascii_file) {
			printf("Error: %s is not a valid listfile\n", lstfname);
            return -1;
		}
	}

	fseek(lstfile, 0L, 0);
	init_mdv();

	/* now loop through input files */
	ai = 0;
    noof = 0;

    while (noif != 0) {
		noif--;
		ai++;
		dz = 0;

        fgets(lstline, LINESIZE-1, lstfile);
		strcpy(lstline2, lstline);
		
        /* get first item */
		p = strtok(lstline2, " \t\n");
		
        if (p == NULL) {
            continue;
        }
        
		i = sscanf(p, "%s", ifname);
		q = p+strlen(p) + 1;
		p = strtok(q, " \t\n");

        if (p != NULL) {
			i = sscanf(p,"%ld",&datasize);

            if (i==1) {
				dz = 1;
			}
            else {
				printf("Error: incorrect format datasize: %s\n", p);
				printf(">>%s<<\n", lstline);
				printf("Skipping\n");

                continue;
			}
		}

        if (genheader(ifname, sector, dz, datasize)<0) {
			printf("Quitting\n");
            return -1;
		}

        /* simply open it again */
        char path[256];
        path[255]='\0';
        strcpy(path, temppath);
        strcat(path, ifname);

        if ((infile = fopen(path, "rb")) == NULL) {
			printf("cannot open %s, skipping\n", ifname);
			continue;
		}
        
		number = (int)fread(&sector[64],sizeof(U8),512-64, infile);
		number += 64;

        put_dir();
		put_sect(number);
		
        while ((number = fread(sector, sizeof(U8), 512, infile)) != NULL) {
			put_sect(number);
		}
        
		fclose(infile);
		noof++;
		cfile++;
		ai++;
	}

	write_dir_len();
    for (s = 0; s < NOSECTS; s++) {
		write_mdv(mdv[s],outfile);
    }

	fclose(outfile);
	printf("Created %s: %d/%d sectors\n", ofname, 255-csect, 255);

    return 255-csect;
}

/* extract files from a MDV file. If 'create'== 0 then only list contents */
int mdv2fil(char *fname, int create) {
    int number;
    int sector; /* also exists as sector[] !!!! */
    FILE *infile;

    if ((infile = fopen(fname,"rb")) == NULL) {
        printf("Error: cannot open %s\n", fname);
        return -1;
    }

    if (create) {
        if ((qxldf = fopen(dirfname,"rb")) != NULL) {
            printf("%s already exists, exiting\n", dirfname);
            fclose(qxldf);
            
            return -1;
        }
        fclose(qxldf);

        if ((qxldf = fopen(dirfname,"wb+")) == NULL) {
            printf("cannot open %s for write\n", dirfname);
            return -1;
        }
    }
    
    fseek(infile, 0L, 2);

    if (ftell(infile)!=174930) {
        printf("Filelength not correct, exiting\n");
        return -1;
    }

    fseek(infile, 0L, 0);

    sector = 0;
    while ((number = fread(mdv[sector],sizeof(char), SECTLEN, infile))!=0) {
        if (number < SECTLEN) {
            printf("error: block < SECTLEN: %d\n", number);
            return -1;
        }

        sector++;

        if (sector == NOSECTS) {
            break;
        }
    }

    int num = print_map(create);
    fclose(infile);
    
    return num;
}
