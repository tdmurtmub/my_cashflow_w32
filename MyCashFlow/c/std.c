/***********************************************************************

Module:		STD.C

				Copyright (c) 1991 by Wesley Steiner

Purpose:		My standard extensions to C.

$Header:   K:/lib/std.c_v   1.0   28 Jan 1995 10:32:30   WES  $

$Log:   K:/lib/std.c_v  $
 * 
 *    Rev 1.0   28 Jan 1995 10:32:30   WES
 * Initial revision.

***********************************************************************/

#include <dos.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "std.h"

/**********************************************************************/

int round(double x)

	{
	double y;
	int result;

	result = (modf(x, &y) >= 0.50 ? y + 1.0 : y);
	return result;
	}

long int GetFileSize(const char *path)

	{
	int hfile;
	long L;
	
	if (!FileExists(path)) return -1L;
	
	hfile = _open(path, O_RDONLY + O_BINARY);
	L = filelength(hfile);
	_close(hfile);

	return L;
	}
                                                                       