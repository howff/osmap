/* > osmap.c
 * 1.00 arb Sat May 24 18:58:11 BST 2003
 */

/*
 * zone2en - convert zone to (e,n) in m from true origin
 * en2zone - convert (e,n) in m from true origin into zone string
 * gridref2en - convert 2,4 or 6 digits reference into m
 */
extern int zone2en(char *zone, int *e, int *n);
extern int en2zone(char *zone, int e, int n);
extern int gridref2en(char *ref, int *e, int *n);

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
extern int osgrs80_to_en(double osgrs80e, double osgrs80n, double *e, double *n);
extern int en_to_osgrs80(double e, double n, double *osgrs80e, double *osgrs80n);
extern int en_to_latlon(double E, double N, double *lat, double *lon);
extern int latlon_to_en(double lat, double lon, double *E, double *N);


typedef struct
{
	double lat, lon;    /* degrees, WGS84, satlib-format positive W 0-360 */
	char zone[3];       /* OS NGR two=letter zone */
	double east, north; /* full coordinate in metres */
	int landranger;     /* map number */
} OSMap_Coord_t;

void
OSMap_SetLatLon(OSMap_Coord_t *coord, double lat, double lon)
{
	coord.lat = lat;
	coord.lon = MOD360(lon);
}

void
OSMap_SetEastNorth(OSMap_Coord_t *coord, double east, double north)
{
	coord.east = east;
	coord.north = north;
}

void
OSMap_SetGridRef(OSMap_Coord_t *coord, char *zone, char *ref)
{
	/* XXX improvement: if zone is NULL then zone is included in ref */
	coord.zone[0] = toupper(zone[0]);
	coord.zone[1] = toupper(zone[1]);
	coord.zone[2] = '\0';
	gridref2en(ref, &e, &n);
	coord.east = ;
	coord.north = ;
}

void
OSMap_SetMapRef(OSMap_Coord_t *coord, int map, char *ref)
{
	Landranger_Init();
	Landranger_MapZone();
	coord.landranger = map;
	coord.east = ;
	coord.north = ;
}

