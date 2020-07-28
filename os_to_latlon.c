/* > os_to_latlon.c
 * 1.02 arb Mon Feb  9 12:33:45 GMT 2015 - can accept absolute
 *      easting and northing coords (space or comma separated).
 * 1.01 arb Sat Sep 11 23:10:46 BST 2010 - added input from stdin
 *      and support for multiple command line coords.
 * 1.00 arb 
 */

static char SCCSid[] = "@(#)os_to_latlon.c1.02 (C) 2009 arb mapLib: British National Grid to Lat,Lon";


/*
 * Usage:
 * os_to_latlon ZoneGridref [...]
 * or
 * os_to_latlon < fileContainingZoneGridref
 *
 * Converts a British National Grid coordinate of the form
 * ZoneGridref, eg. NH022765 (may also contain higher precision gridref)
 * into latitude,longitude (on WGS84)
 * Will also accept coordinates (easting northing).
 */

#include <stdio.h>
#include "osmap.h"
#ifndef NINT /* it's in satlib */
//#define DOUBLE_TMP double macro_dtmp
//#define NINT(a)     ((int)((macro_dtmp=(a)) > 0 ? macro_dtmp+0.5 : macro_dtmp-0.5))
#define INLINE inline
static INLINE int       NINT(double  a) { return (int)(a > 0 ? a+0.5 : a-0.5); }
#endif

int
convert_EN(int E, int N, double *lat, double *lon, double *height)
{
	double x,y,z;

	OSMap_InitConstants(E_Airy);
	en_to_latlon(E, N, lat, lon);
	latlon_to_cartesian(*lat, *lon, 0, &x, &y, &z);
	helmert_osgb36_to_wgs84(&x, &y, &z);
	OSMap_InitConstants(E_GRS80);
	cartesian_to_latlon(x, y, z, lat, lon, height);
	return(0);
}


int
convert_zonegridref(const char *zonegrid, double *lat, double *lon, double *height)
{
	int E, N;

	zonegridref2en(zonegrid, &E, &N);
	return convert_EN(E, N, lat, lon, height);
}


int
convert_string(const char *string, double *lat, double *lon, double *height)
{
	float E, N;

	/* If there's a zone present */
	if (isalpha(string[0]))
		return convert_zonegridref(string, lat, lon, height);

	/* otherwise split into E and N */
	if (sscanf(string, "%f %f", &E, &N) == 2)
		return convert_EN(NINT(E), NINT(N), lat, lon, height);
	else if (sscanf(string, "%f,%f", &E, &N) == 2)
		return convert_EN(NINT(E), NINT(N), lat, lon, height);

	return(-1);
}


int
main(int argc, char *argv[])
{
	double lat, lon, height;

	if (argc > 1)
	{
		int i;
		for (i=1; i<argc; i++)
		{
			convert_string(argv[i], &lat, &lon, &height);
			printf("%f,%f\n", lat, lon);
		}
	}
	else
	{
		char line[256];
		while (fscanf(stdin, "%s", line) == 1)
		{
			convert_string(line, &lat, &lon, &height);
			printf("%f,%f\n", lat, lon);
		}
	}

	return(0);
}
