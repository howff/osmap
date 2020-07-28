/* > helmert.c
 * 1.00 arb
 */

static char SCCSid[] = "@(#)helmert.c     1.00 (C) 2007 arb mapLib: Helmert transforms";

#include "satlib/dundee.h"      /* for PI */
#include <math.h>
#include "helmert.h"


/* ---------------------------------------------------------------------------
 * Constants used when converting between the two datums
 * ETRS89 (WGS84) and OSGB36.
 * Reverse the signs when going in the other direction.
 */
double helmert_etrs89_to_osgb36_dx    = -446.448;  /* m */
double helmert_etrs89_to_osgb36_dy    =  125.157;  /* m */
double helmert_etrs89_to_osgb36_dz    = -542.060;  /* m */
double helmert_etrs89_to_osgb36_scale =   20.4894; /* parts per million */
double helmert_etrs89_to_osgb36_xrot  =   -0.1502; /* seconds of arc */
double helmert_etrs89_to_osgb36_yrot  =   -0.2470; /* seconds of arc */
double helmert_etrs89_to_osgb36_zrot  =   -0.8421; /* seconds of arc */


/* ---------------------------------------------------------------------------
 * Compute Helmert-transformed X coordinate.
 * x,y,z - cartesian X,Y,Z coordinates (m)
 * dx    - X translation (m)
 * yrot  - Y rotation (seconds of arc)
 * zrot  - Z rotation (seconds of arc)
 * scale - (ppm)
 * Returns: transformed X
 */
double
helmert_x(double x, double y, double z, double dx, double roty, double rotz, double scale)
{
	double scalefactor = scale * 0.000001;
	double roty_rad = roty / 3600 * PI/180;
	double rotz_rad = rotz / 3600 * PI/180;
	double helmertx = x + (x * scalefactor) - (y * rotz_rad) + (z * roty_rad) + dx;
	return helmertx;
}


/*
 * Compute Helmert-transformed Y coordinate.
 * x,y,z - cartesian X,Y,Z coordinates (m)
 * dY    - Y translation (m)
 * xrot  - X rotation (seconds of arc)
 * zrot  - Z rotation (seconds of arc)
 * scale - (ppm)
 * Returns: transformed Y
 */
double
helmert_y(double x, double y, double z, double dy, double rotx, double rotz, double scale)
{
	double scalefactor = scale * 0.000001;
	double rotx_rad = rotx / 3600 * PI/180;
	double rotz_rad = rotz / 3600 * PI/180;
	double helmerty = (x * rotz_rad) + y + (y * scalefactor) - (z * rotx_rad) + dy;
	return helmerty;
}


/*
 * Compute Helmert-transformed Z coordinate.
 * x,y,z - cartesian X,Y,Z coordinates (m)
 * dz    - Z translation (m)
 * xrot  - X rotation (seconds of arc)
 * xrot  - Y rotation (seconds of arc)
 * scale - (ppm)
 * Returns: transformed Z
 */
double
helmert_z(double x, double y, double z, double dz, double rotx, double roty, double scale)
{
	double scalefactor = scale * 0.000001;
	double rotx_rad = rotx / 3600 * PI/180;
	double roty_rad = roty / 3600 * PI/180;
	double helmertz = (-1 * x * roty_rad) + (y * rotx_rad) + z + (z * scalefactor) + dz;
	return helmertz;
}


/* ---------------------------------------------------------------------------
 * Convert a cartesian coordinate X,Y,Z (m) in ECEF between the WGS84 datum
 * (ETRS89) and the Ordnance Survey datum (OSGB36).
 */
void
helmert_wgs84_to_osgb36(double *x, double *y, double *z)
{
	double xx, yy, zz;
	xx = helmert_x(*x, *y, *z, helmert_etrs89_to_osgb36_dx,
		helmert_etrs89_to_osgb36_yrot, helmert_etrs89_to_osgb36_zrot,
		helmert_etrs89_to_osgb36_scale);
	yy = helmert_y(*x, *y, *z, helmert_etrs89_to_osgb36_dy,
		helmert_etrs89_to_osgb36_xrot, helmert_etrs89_to_osgb36_zrot,
		helmert_etrs89_to_osgb36_scale);
	zz = helmert_z(*x, *y, *z, helmert_etrs89_to_osgb36_dz,
		helmert_etrs89_to_osgb36_xrot, helmert_etrs89_to_osgb36_yrot,
		helmert_etrs89_to_osgb36_scale);
	*x = xx;
	*y = yy;
	*z = zz;
}


void
helmert_osgb36_to_wgs84(double *x, double *y, double *z)
{
	double xx, yy, zz;
	xx = helmert_x(*x, *y, *z, -helmert_etrs89_to_osgb36_dx,
		-helmert_etrs89_to_osgb36_yrot, -helmert_etrs89_to_osgb36_zrot,
		-helmert_etrs89_to_osgb36_scale);
	yy = helmert_y(*x, *y, *z, -helmert_etrs89_to_osgb36_dy,
		-helmert_etrs89_to_osgb36_xrot, -helmert_etrs89_to_osgb36_zrot,
		-helmert_etrs89_to_osgb36_scale);
	zz = helmert_z(*x, *y, *z, -helmert_etrs89_to_osgb36_dz,
		-helmert_etrs89_to_osgb36_xrot, -helmert_etrs89_to_osgb36_yrot,
		-helmert_etrs89_to_osgb36_scale);
	*x = xx;
	*y = yy;
	*z = zz;
}


/* ---------------------------------------------------------------------------
 */
#ifdef MAIN
int main(int argc, char*argv[])
{
	double x,y,z;

	x = 3909833.018;
	y = -147097.1376;
	z = 5020322.478;

	helmert_wgs84_to_osgb36(&x, &y, &z);

	/* Should be 3909460.067807 -146987.299607 5019888.069969 */
	printf("%f %f %f\n", x, y, z);

	return(0);
}
#endif /* MAIN */
