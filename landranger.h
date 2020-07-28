/* > landranger.h
 * 1.00 arb
 */

#ifndef __LANDRANGER_H
#define __LANDRANGER_H

#include "osmap.h"

/*
 * Landranger maps are 40km square with 1km grid squares.
 */
#define MAX_LANDRANGER 204   /* Last landranger number */
#define UNMAPPED -999        /* Maps not yet entered */
#define MAPSQUARE 40000      /* Maps are 40km wide and high */

/*
 * Top left corner in metres offset from SV 0000 (or UNMAPPED if not known)
 */
extern int Landranger_East[MAX_LANDRANGER+1];
extern int Landranger_North[MAX_LANDRANGER+1];
extern char *Landranger_Name[MAX_LANDRANGER+1];


#ifdef __cplusplus
extern "C" {
#endif


/** landranger.c **/
/*
 * Landranger coordinates are in m from false origin, ie. SV 0000
 */
extern int Landranger_Init();
extern int Landranger_FromGrid(int E, int N, int *maps);
extern int Landranger_BinOutline(ellipsoid_t, int mapnum, gridpoint_t *points);
extern int Landranger_Bin(int mapnum, gridpoint_t *points);

/** mapzone.c **/
/*
 * Convert (map,e,n) to (zone).
 * e,n are in m as an offset from an unknown zone (to be returned), but are
 * known to on the given map.  Use gridref2en to change gridref string to e,n
 * for input to this function.
 */
extern int Landranger_Mapzone(char *zone, int map, int e, int n);


#ifdef __cplusplus
}
#endif

#endif /* !__LANDRANGER_H */
