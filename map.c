/* > map.c
 * 1.03 arb Thu May 28 14:12:34 BST 2009 - Added zonegridref2en.
 * 1.02 arb Wed Feb 25 17:14:36 GMT 2004 - Accept lowercase zone letters.
 * 1.01 arb Thu May  7 15:22:19 BST 1998 - solaris has no exp10() so replaced
 *      with pow(10,)
 * 1.00 arb Tue May 28 10:48:09 BST 1996
 * Converts map references (eg. NN 630412) to eastings and northings in m.
 * usage: map [Zone Ref]
 *  Zone is two letters, for example SP,
 *  Ref is 2, 4 or 6 digits
 *  output is various forms of (e,n)
 * else:
 * give lines of 'zone gridref' on stdin and have grid ref in metres on stdout
 * coordinates are full references from real origin.
 */

static char SCCSid[] = "@(#)map.c         1.03 (C) 1996 arb mapLib: GridRef->metres";

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "osmap.h"
#define exp10(Y) (pow(10,Y))

/*
 * zone2en converts a two letter zone string into meters (e,n)
 */
int
zone2en(const char *zone, int *e, int *n)
{
	int a, b;

	if ((a = toupper(zone[0]) - 'A') > 7) a--;
	if ((b = toupper(zone[1]) - 'A') > 7) b--;
	*e = 500*(a%5) + 100*(b%5);
	*n = 500*(4-a/5) + 100*(4-b/5);
	*e *= 1000;
	*n *= 1000;
	return(0);
}


/*
 * en2zone extracts a zone string from (e,n) in m.
 * zone string must be a buffer of size >=3, as a nul char is added.
 */
int
en2zone(char *zone, int e, int n)
{
	int a, b;

	e /= ZONESQUARE; n /= ZONESQUARE;
	a = e/5; b = e-5*a;
	zone[0] = 'A'+a;
	zone[1] = 'A'+b;

	a = n/5; b = n-a*5;
	zone[0] += (4-a)*5;
	zone[1] += (4-b)*5;
	if (zone[0] >= 'I') zone[0]++;
	if (zone[1] >= 'I') zone[1]++;
	zone[2] = '\0';
	return(0);
}


/*
 * gridref2en converts a 2, 4, 6, 8 or 10 digit grid reference into metres (e,n)
 * by convention 2 digits is 10km, 4 digits is km, 6 digits is 100m,
 * 8 digits is 10m, 10 digits is 1m.  12 digits (or more) is always metres but
 * note that most uses of this function will be using a different offset than
 * you get with 12 digit references.
 * Note: no error checking is performed.
 */
int
gridref2en(const char *ref, int *e, int *n)
{
	const char *nptr;
	int digits, i;

	*e = *n = 0;
	digits = i = strlen(ref) / 2;
	nptr = ref + digits;
	while (i--)
	{
		*e = *e * 10 + *ref++ - '0';
		*n = *n * 10 + *nptr++ - '0';
	}
	/* 6 digits each for N,E (or more) are assumed to be in metres */
	/* but less than 6 each is converted to metres. */
	if (digits < 6)
	{
		*e *= exp10(5-digits);
		*n *= exp10(5-digits);
	}
	return(0);
}


/*
 * Convert for example "NH123456" or "NH 123456" to east,north.
 */
int
zonegridref2en(const char *zonegridref, int *e, int *n)
{
	int E,N, e0,n0, e1,n1;
	char zone[3];

	/* Skip any spaces before the zone */
	while (*zonegridref && !isalpha(*zonegridref)) zonegridref++;
	/* Get offset to start of SV zone */
	zone2en("SV", &E, &N);
	/* Convert zone to easting,northing */
	zone[0] = zonegridref[0];
	zone[1] = zonegridref[1];
	zone[2] = '\0';
	zone2en(zone, &e0, &n0);
	/* Skip zone then any spaces between zone and grid reference */
	zonegridref+=2;
	while(*zonegridref && !isdigit(*zonegridref)) zonegridref++;
	/* Convert gridref to relative easting,northing */
	gridref2en(zonegridref, &e1, &n1);
	/*printf("%s %s\n%7d %7d\n%7d %7d\n%7d %7d\n",zone,zonegridref,E,N, e0,n0, e1,n1);*/
	/* Return easting,northing relative to SV */
	*e = e0+e1-E;
	*n = n0+n1-N;
	return(0);
}


#ifdef MAIN
int main(int argc, char *argv[])
{
	int e0, n0, e, n, E, N;
	char *zoneletters, *gridref;
	char newzone[8];

	zone2en("SV", &E, &N);
	if (argc > 1)
	{
		zoneletters = argv[1];
		gridref = argv[2];

		zone2en(zoneletters, &e0, &n0);
		printf("Zone %s is (%8d,%8d) m\n", zoneletters, e0, n0);
		gridref2en(gridref, &e, &n);
		printf("Coord   is (%8d,%8d)\n", e, n);
		printf("Final   is (%8d,%8d)\n", e+e0, n+n0);
		printf("From SV is (%8d,%8d)\n", e+e0-E, n+n0-N);
		en2zone(newzone, e+e0, n+n0);
		printf("Zone %s\n", newzone);
	}
	else
	{
		char zone[16], grid[16], line[256];

		while (fgets(line, 255, stdin))
		{
			sscanf(line, "%s %s", zone, grid);
			zone2en(zone, &e0, &n0);
			gridref2en(grid, &e, &n);
			/*printf("%d\t%d\n", e0+e, n0+n);*/
			printf("%d\t%d\n", e0+e-E, n0+n-N);
		}
	}
	return(0);
}
#endif /* MAIN */
