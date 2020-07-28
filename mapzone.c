/* > mapzone.c
 * 1.00 arb Mon Sep  2 17:53:07 BST 1996
 * Convert input of 'mapnum gridref' to output of 'zone gridref'.
 * This involves looking up map coords in landranger.c database
 * and converting to a zone.
 */

static char SCCSid[] = "@(#)mapzone.c     1.00 (C) 1996 arb mapLib: Map+Ref -> Zone";

#include <stdio.h>
#include <string.h>
#include "arb/utils.h"
#include "satlib/dundee.h"
#include "osmap.h"
#include "landranger.h"

static int verbose = 0;


/*
 * Given (map, e, n) fill in zone array (two chars),
 * Landranger_Init must be called first.
 */
int
Landranger_Mapzone(char *zone, int map, int e, int n)
{
	static int sv00e=0, sv00n=0;
	int mape, mapn, mapzonee, mapzonen;

	Landranger_Init();
	if (sv00e==0)
		zone2en("SV", &sv00e, &sv00n);
	/* Get bottom left map corner as offset from AA00 */
	mape = Landranger_East[map]+sv00e;
	mapn = Landranger_North[map]+sv00n-MAPSQUARE;
	/* Find which zone the map corner is in */
	en2zone(zone, mape, mapn);
	/* Find the offset from AA00 to the zone corner */
	zone2en(zone, &mapzonee, &mapzonen);
	/* Assume gridref given is from this zone */
	e += mapzonee;
	n += mapzonen;
	/* If so then full coord will be on map, else it's in next zone. */
	if (verbose) fprintf(stderr,"Map starts at %d,%d and ends at %d,%d\nThat is in zone %s\nZone starts at %d,%d and ends at %d,%d\nTrying point in that zone, at %d,%d\n", mape,mapn, mape+MAPSQUARE, mapn+MAPSQUARE, zone, mapzonee,mapzonen, mapzonee+ZONESQUARE,mapzonen+ZONESQUARE, e,n);
	if (e < mape || e > mape+MAPSQUARE) e+=ZONESQUARE;
	if (n < mapn || n > mapn+MAPSQUARE) n+=ZONESQUARE;
	/* Convert grid ref into a zone string */
	en2zone(zone, e, n);
	return(0);
}


#ifdef MAIN
#include <ctype.h>
static char *usage = "usage: %s < MapGridref > ZoneGridref\n"
	"ie. stdin is lines containing two space-separated fields\n"
	"MapNumber and GridRef, eg. 50 123456\n"
	"and stdout is lines containing two space-separated fields\n"
	"ZoneLetter and GridRef, eg. NN 123456\n";
static char *prog;
int
main(int argc, char *argv[])
{
	FILE *fpin=stdin, *fpout=stdout;
	char line[256], *ref, zone[8];
	int map;
	int e, n;

	prog = argv[0];
	if (argc>1) { fprintf(stderr,usage,prog); exit(1); }

	Landranger_Init();
	while (fgets(line, 256, fpin))
	{
		line[strlen(line)-1] = '\0';
		map = atoi(line);
		ref=line; while (!isspace(*ref)) ref++;
		while (isspace(*ref)) ref++;
		if (Landranger_East[map] == UNMAPPED)
		{
			fprintf(stderr,"%s: don't know coordinates of map %d\n",prog,map);
			continue;
		}
		gridref2en(ref, &e, &n);      /* get ref into metres */
		Landranger_Mapzone(zone, map, e, n);
		fprintf(fpout, "%s\t%s\n", zone, ref);
	}
	return(0);
}
#endif /* MAIN */
