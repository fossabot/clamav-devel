/*
 *  Copyright (C) 2002 - 2004 Tomasz Kojm <tkojm@clamav.net>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if HAVE_CONFIG_H
#include "clamav-config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "options.h"
#include "others.h"
#include "shared.h"
#include "defaults.h"
#include "client.h"


void help(void);

/* this local macro takes care about freeing memory at exit */
/*
#define mexit(i)    if(opt) free_opt(opt);			    \
		    mprintf("*Memory freed. Exit code: %d\n", i);   \
		    exit(i);
*/
#define mexit(i)    exit(i)

void clamscan(struct optstruct *opt)
{
	int ds, dms, ret;
	struct timeval t1, t2;
	struct timezone tz;
	time_t starttime;


    /* initialize some important variables */

    mprintf_disabled = 0;

    if(optc(opt, 'v')) mprintf_verbose = 1;
    else mprintf_verbose = 0;

    if(optl(opt, "quiet")) mprintf_quiet = 1;
    else mprintf_quiet = 0;

    if(optl(opt, "stdout")) mprintf_stdout = 1;
    else mprintf_stdout = 0;

    if(optc(opt, 'V')) {
	mprintf("clamdscan / ClamAV version "VERSION"\n");
	mexit(0);
    }

    if(optc(opt, 'h')) {
	free_opt(opt);
    	help();
    }

    if(optc(opt, 'i')) printinfected = 1;
    else printinfected = 0;

    /* initialize logger */

    if(optl(opt, "log-verbose")) logverbose = 1;
    else logverbose = 0;

    if(optc(opt, 'l')) {
	logfile = getargc(opt, 'l');
	if(logg("--------------------------------------\n")) {
	    mprintf("!Problem with internal logger.\n");
	    mexit(2);
	}
    } else 
	logfile = NULL;


    time(&starttime);
    /* ctime() does \n, but I need it once more */
    logg("Scan started: %s\n", ctime(&starttime));

    memset(&claminfo, 0, sizeof(struct s_info));

    gettimeofday(&t1, &tz);
    ret = client(opt);

//Implement STATUS in clamd
    if(!optl(opt, "disable-summary")) {
	gettimeofday(&t2, &tz);
	ds = t2.tv_sec - t1.tv_sec;
	dms = t2.tv_usec - t1.tv_usec;
	ds -= (dms < 0) ? (1):(0);
	dms += (dms < 0) ? (1000000):(0);
	mprintf("\n----------- SCAN SUMMARY -----------\n");
	    logg("\n-- summary --\n");
	mprintf("Infected files: %d\n", claminfo.ifiles);
	    logg("Infected files: %d\n", claminfo.ifiles);
	mprintf("Time: %d.%3.3d sec (%d m %d s)\n", ds, dms/1000, ds/60, ds%60);
	    logg("Time: %d.%3.3d sec (%d m %d s)\n", ds, dms/1000, ds/60, ds%60);
    }

    mexit(ret);
}

void help(void)
{

    mprintf_stdout = 1;

    mprintf("\n");
    mprintf("                          ClamAV Daemon Client "VERSION"\n");
    mprintf("                (c) 2002, 2003 Tomasz Kojm <tkojm@clamav.net>\n\n");

    mprintf("    --help              -h             Show help\n");
    mprintf("    --version           -V             Print version number and exit\n");
    mprintf("    --verbose           -v             Be verbose\n");
    mprintf("    --quiet                            Be quiet, only output error messages\n");
    mprintf("    --stdout                           Write to stdout instead of stderr\n");
    mprintf("                                       (this help is always written to stdout)\n");
    mprintf("    --log=FILE          -l FILE        Save scan report in FILE\n");
    mprintf("    --log-verbose                      Log additional messages\n");
    mprintf("    --config-file=FILE                 Read configuration from FILE.\n");
    mprintf("    --disable-summary                  Disable summary at end of scanning\n");
    mprintf("\n");

    exit(0);
}
