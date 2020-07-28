/* > osmap.h
 * 1.00 arb Thu May 15 13:00:45 BST 2003
 */

#ifndef OSMAP_H
#define OSMAP_H

/*
 * Choose your preferred ellipsoid: Airy, UTM or OSGRS80.
 * The latter is similar enough, by the ETRF, to WGS84 as used by GPS.
 */
typedef enum { E_Airy, E_UTM, E_GRS80 } ellipsoid_t;

/*
 * Map zones, eg. "NC" are 100km square.
 * Define ZONESQUARE as metres square.
 */
#define ZONESQUARE 100000


#ifdef __cplusplus
extern "C" {
#endif


/** map.c **/
/*
 * zone2en - convert zone to (e,n) in m from true origin
 * en2zone - convert (e,n) in m from true origin into zone string
 * gridref2en - convert 2,4 or 6 digits reference into m
 */
extern int zone2en(const char *zone, int *e, int *n);
extern int en2zone(char *zone, int e, int n);
extern int gridref2en(const char *ref, int *e, int *n);
extern int zonegridref2en(const char *zonegridref, int *e, int *n);


/** llen.c **/
/*
 * Conversion functions.
 * To map between OSGRS80 Grid and National Grid use
 *  osgrs80_to_en   parameters are in m from false origin
 *  en_to_osgrs80
 *   * safe to use same vars for both input and output
 * To map between latitude,longitude and grid coordinates use
 *  init_constants  to initialise for your preferred ellipsoid, then
 *  en_to_latlon    parameters are in m from false origin, ie. SV 0000
 *  latlon_to_en
 */
extern void OSMap_InitConstants(ellipsoid_t ellipsoid);
extern int latlon_to_cartesian(double lat, double lon, double height, double *x, double *y, double *z);
extern int cartesian_to_latlon(double x, double y, double z, double *lat, double *lon, double *height);
extern int osgrs80_to_en(double osgrs80e, double osgrs80n, double *e, double *n);
extern int en_to_osgrs80(double e, double n, double *osgrs80e, double *osgrs80n);
extern int en_to_latlon(double E, double N, double *lat, double *lon);
extern int latlon_to_en(double lat, double lon, double *E, double *N);


/** helmert.c **/
extern void helmert_wgs84_to_osgb36(double *x, double *y, double *z);
extern void helmert_osgb36_to_wgs84(double *x, double *y, double *z);


typedef struct
{
	double lat, lon;    /* degrees, WGS84, satlib-format positive W 0-360 */
	char zone[3];       /* OS NGR two=letter zone */
	double east, north; /* full coordinate in metres */
	int landranger;     /* map number */
} OSMap_Coord_t;

extern void OSMap_SetLatLon(OSMap_Coord_t *, double lat, double lon);
extern void OSMap_SetEastNorth(OSMap_Coord_t *, double east, double north);
extern void OSMap_SetGridRef(OSMap_Coord_t *, char *zone, char *ref);
extern void OSMap_SetMapRef(OSMap_Coord_t *, int map, char *ref);


#ifdef __cplusplus
}
#endif

#endif
