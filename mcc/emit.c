/*	Programmer : Richard Boccuzzi	*/
/*	Filename   : emit.c				*/

#include <ctype.h>
#include <unistd.h>

#include "mcc.h"
#include "emit.h"
#include "errors.h"

static int word[32];

/*----------------------------------------------------------------------*/

static FILE *outfile;
void initemit()
{
	int i;
	outfile = stdout;

/*****
	outfile = fopen("./a.m","w+");
	if (!outfile)
	{
		fprintf(stderr, "Error, could not open a.m for output\n");
		exit(1);
	}
*****/
	for(i = 0; i < 32; i++)
		word[i] = -1;
}

/*----------------------------------------------------------------------*/

void unlinkoutfile()
{
	extern int fclose(FILE *__stream);

	fclose(outfile);
	unlink("./a.s");
}

/*----------------------------------------------------------------------*/

void dumpword()
{
	int i;
	if (panicmode) return;
	for (i=0; i < 32; i++)
	{
		if (word[i] < 1)
			putc('0', outfile);
		else
			putc('1', outfile);
		word[i] = -1;
	}
	putc('\n', outfile);
}

/*----------------------------------------------------------------------*/

void genamux(bool set)
{
	if (panicmode) return;
	if (word[0] < 0)
		word[0] = set;
	else
		printerr(semanticerr, reset, (int) "amux");
	word[20] = false;
}

/*----------------------------------------------------------------------*/

void gencond(int c)
{
	if (panicmode) return;
	if (word[1] < 0)
		switch (c)
		{
			case 0:	word[1] = word[2] = 0;		break;
			case 1:	word[1] = 0; word[2] = 1;	break;
			case 2:	word[1] = 1; word[2] = 0; 	break;
			case 3:	word[1] = word[2] = 1;		break;
		}
	else
		printerr(semanticerr, reset, (int) "jump condition");
}

/*----------------------------------------------------------------------*/

void genalu(int f)
{
	if (panicmode) return;
	if (word[3] < 0)
		switch (f)
		{
			case 0:	word[3] = word[4] = 0;		break;
			case 1:	word[3] = 0; word[4] = 1;	break;
			case 2:	word[3] = 1; word[4] = 0; 	break;
			case 3:	word[3] = word[4] = 1;		break;
		}
	else
		printerr(semanticerr, reset, (int) "alu function");
}

/*----------------------------------------------------------------------*/

void genshift(int f)
{
	if (panicmode) return;
	if (word[5] < 0)
		switch (f)
		{
			case 0:	word[5] = word[6] = 0;		break;
			case 1:	word[5] = 0; word[6] = 1;	break;
			case 2:	word[5] = 1; word[6] = 0; 	break;
			case 3:	word[5] = word[6] = 1;		break;
		}
	else
		printerr(semanticerr, reset, (int) "shift function");
}

/*----------------------------------------------------------------------*/

void genmbr(bool set)
{
	if (panicmode) return;
	if (word[7] < 0)
		word[7] = set;
	else
		printerr(semanticerr, reset, (int) "mbr");
}

/*----------------------------------------------------------------------*/

void genmar(bool set)
{
	if (panicmode) return;
	if (word[8] < 0)
		word[8] = set;
	else
		printerr(semanticerr, reset, (int) "write");
}

/*----------------------------------------------------------------------*/

void genread(bool set)
{
	if (panicmode) return;
	if (word[9] < 0)
		word[9] = set;
	else
		printerr(semanticerr, reset, (int) "read");
}

/*----------------------------------------------------------------------*/

void genwrite(bool set)
{
	if (panicmode) return;
	if (word[10] < 0)
		word[10] = set;
	else
		printerr(semanticerr, reset, (int) "write");
}

/*----------------------------------------------------------------------*/

void genenc(bool set)
{
	if (panicmode) return;
	if (word[11] < 0)
		word[11] = set;
	else
		printerr(semanticerr, reset, (int) "enc");
}

/*----------------------------------------------------------------------*/

void gencreg(enreg dreg)
{
	int temparray[4];
	if (panicmode) return;
	if (dreg == r_mbr)
		genmbr(true);
	else
	{
		int i;
		itob(temparray, 4, dreg);
		if (word[12] == -1)
			for (i = 0; i < 4; i++)
				word[12+i] = temparray[i];
		else
			if (!isequalb(temparray, word+12, 4))
				printerr(semanticerr, reset, (int) "C bus");
	}
}

/*----------------------------------------------------------------------*/

void genbreg(enreg dreg)
{
	int temparray[4];
	if (panicmode) return;
	if (dreg == r_mbr)
		printerr(semanticerr, wrongmbr, 0);
	else
	{
		int i;
		itob(temparray, 4, dreg);
		if (word[16] == -1)
			for (i = 0; i < 4; i++)
				word[16+i] = temparray[i];
		else
			if (!isequalb(temparray, word+16, 4))
				printerr(semanticerr, reset, (int) "B bus");
	}
}

/*----------------------------------------------------------------------*/

void genareg(enreg dreg)
{
	int temparray[4];
	if (panicmode) return;
	if (dreg == r_mbr)
		genamux(true);	
	else
	{
		int i;
		itob(temparray, 4, dreg);
		if (word[20] == -1)
			for (i = 0; i < 4; i++)
				word[20+i] = temparray[i];
		else
			if (!isequalb(temparray, word+20, 4))
				printerr(semanticerr, reset, (int) "A bus");
	}
}

/*----------------------------------------------------------------------*/

void genaddr(int addr)
{
	int temparray[8];
	if (panicmode) return;
	{
		int i;
		itob(temparray, 8, addr);
		if (word[24] == -1)
			for (i = 0; i < 8; i++)
				word[24+i] = temparray[i];
		else
			if (!isequalb(temparray, word+24, 8))
				printerr(semanticerr, reset, (int) "jump address");
	}
}

/*----------------------------------------------------------------------*/

void genhalt()
{
	if (panicmode) return;
	genread(true);
	genwrite(true);
}

/*----------------------------------------------------------------------*/

void genabreg(enreg dreg)
{
	int temparray[4];
	if (panicmode) return;
	if (dreg == r_mbr)
		genamux(true);	
	else
	{
		itob(temparray, 4, dreg);
		if (isequalb(temparray, word+20, 4))
			genareg(dreg);
		else if (isequalb(temparray, word+16, 4))
			genbreg(dreg);
		else if (word[20] == -1)
			genareg(dreg);
		else if (word[16] == -1)
			genbreg(dreg);
		else
			printerr(semanticerr, reset, (int) "A/B bus");
	}
}

/*----------------------------------------------------------------------*/

void itob(int *a, int size, int num)
{
	for (size--; size >= 0; size--)
	{
		a[size] = num % 2;
		num = num / 2;
	}
}

/*----------------------------------------------------------------------*/

bool isequalb(int *a, int *b, int size)
{
	for (; size; size--)
		if (a[size-1] != b[size-1])
			return false;
	return true;
}

/*----------------------------------------------------------------------*/
