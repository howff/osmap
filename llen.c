/* > llen.c
 * 1.10 arb Fri Jul 13 15:10:33 BST 2007 - Added helmert transforms.
 * 1.01 arb Thu May 30 17:39:54 BST 1996 - bug fix a2 in init_constants
 * 1.00 arb Wed Apr 10 11:04:01 BST 1996
 * Converts (easting,northing) to (latitude,longitude) and back again.
 * Main program (optional):
 * Input is (e,n) offsets from SV00,
 * output is in degrees on specified ellipsoid.
 * usage: llen [-v] [-e ellipsoid] [east north]
 * ellipsoid is a, u or g: airy, utm or grs80.
 */

static char SCCSid[] = "@(#)llen.c        1.10 (C) 1996 arb mapLib: L,L <-> E,N";

/*
 * Convert eastings,northings to latitude,longitude on given ellipsoid
 * and back again. Also calculate real distances.
 */

/*
 * Bugs:
 * Have not considered local scale factors, convergence, thus true
 * azimuth or true distance cannot be computed.
 */

/*
 * Notes:
 * Longitude is positive east of Greenwich
 */

/*
 * To do:
 * Encapsulate ellipsoid parameters into one structure
 * Put first order calculations into another containing nu,rho,eta2
 */

/*
 * Configuration:
 * Define ODD_TAN to get unusual tan behaviour in equations VI and VIII.
 */
#undef ODD_TAN

#include <stdio.h>
#include <math.h>
#include "arb/utils.h"
#include "osmap.h"
#include "helmert.h"

#if !defined PI && defined M_PI
#define PI M_PI
#elif !defined PI
#define PI 3.1415926536
#endif

#ifndef RAD
#define RAD(X) ((X)*PI/180.0)
#define DEG(X) ((X)*180.0/PI)
#endif

static double N0 = -100000;          /* grid coords of true origin */
static double E0 =  400000;
static double F0 = 0.9996012717;     /* scale on central meridian */
static double a = 6377563.396;       /* semi-major axis */
static double b = 6356256.910;       /* semi-minor axis */
static double phi0 = RAD(49);        /* true origin */
static double lambda0 = RAD(-2);
static double aF0;                   /* pre-scaled axes  = a*F0*/
static double bF0;                   /* = b*F0 */
static double e2;                    /* eccentricity = (a*a-b*b)/(a*a) */
static double n;                     /* = (a-b)/(a+b) */

static double east_shift[5][3] =
{
	{ 92,  96, 102 },
	{ 89,  96, 105 },
	{ 85,  97, 108 },
	{ 93,  99, 106 },
	{ 99, 104, 107 }
};
static double north_shift[5][3] =
{
	{ -82, -80, -82 },
	{ -75, -75, -78 },
	{ -58, -62, -62 },
	{ -47, -52, -54 },
	{ -44, -49, -52 }
};

static int verbose = 0;


/* ---------------------------------------------------------------------------
 * Initialise constants for given ellipsoid
 */
void
OSMap_InitConstants(ellipsoid_t ellipsoid)
{
	double a2;

	switch (ellipsoid)
	{
		case E_Airy:
			N0 = -100000;
			E0 =  400000;
			F0 = 0.9996012717;
			a = 6377563.396;
			b = 6356256.910;
			phi0 = RAD(49);
			lambda0 = RAD(-2);
			break;
		case E_UTM:
			N0 = 0;
			E0 =  500000;
			F0 = 0.9996;
			a = 6378388.000;
			b = 6356911.946;
			phi0 = RAD(0);
			lambda0 = RAD(3);
			break;
		case E_GRS80:
			N0 = -100000;
			E0 =  400000;
			F0 = 0.9996012717;
			a = 6378137.000;
			b = 6356752.3141; /* or .3142 if using GRS80 with WGS84 */
			phi0 = RAD(49);
			lambda0 = RAD(-2);
			break;
		default:
			break;
	}

	a2 = a*a;
	aF0 = a*F0;
	bF0 = b*F0;
	e2 = (a2-b*b)/a2;
	n = (a-b)/(a+b);
}


/* ---------------------------------------------------------------------------
 * Convert between Cartesian coordinates in ECEF reference frame
 * and latitude/longitude in the ellipsoid already initialised.
 * Lat,lon - degrees N,E
 * height  - ellipsoid height in meters
 * x,y,z   - meters ECEF
 */
int
latlon_to_cartesian(double lat, double lon, double height, double *x, double *y, double *z)
{
	double nu;
	double sinlat, coslat;
	double sinlon, coslon;

	sincos(RAD(lat), &sinlat, &coslat);
	sincos(RAD(lon), &sinlon, &coslon);

	nu = a / sqrt(1 - e2 * sinlat * sinlat);     /* equation B2 */

	*x = (nu + height) * coslat * coslon;        /* equation B3 */
	*y = (nu + height) * coslat * sinlon;        /* equation B4 */
	*z = ((1 - e2) * nu + height) * sinlat;      /* equation B5 */

	return(0);
}


int
cartesian_to_latlon(double x, double y, double z, double *lat, double *lon, double *height)
{
	double p, nu, sinlat, newlat;
	int count = 0;
#define LATITUDE_PRECISION (1e-5) /* in radians */

	p = sqrt(x * x + y * y);

	*lon = DEG(atan(y / x));                    /* equation B6 */

	/* first guess at latitude */
	newlat = atan(z / p * (1 - e2));            /* equation B7 */

	/* Iterate to get a better latitude */
	do
	{
		*lat = newlat;
		sinlat = sin(*lat);
		nu = a / sqrt(1 - e2 * sinlat * sinlat);    /* from B2 */
		newlat = atan((z + e2 * nu * sinlat) / p);
		count++;
	} while (fabs(newlat - *lat) > LATITUDE_PRECISION && count < 9999);
	*lat = DEG(newlat);

	/* Finally height from latest latitude */
	*height = p / cos(RAD(*lat)) - nu;

	return(0);
}


/* ---------------------------------------------------------------------------
 * Convert GRS80 to E,N
 */
int
osgrs80_to_en(double osgrs80e, double osgrs80n, double *e, double *n)
{
	int e_index, n_index;
	double s0, s1, s2, s3;
	double t, u;
	double e_offset, n_offset;

	e_index = osgrs80e / 350000;
	n_index = osgrs80n / 350000;
	t = (osgrs80e - e_index * 350000 ) / 350000;
	u = (osgrs80n - n_index * 350000 ) / 350000;
	s0 = east_shift[n_index][e_index];
	s1 = east_shift[n_index][e_index+1];
	s2 = east_shift[n_index+1][e_index+1];
	s3 = east_shift[n_index+1][e_index];
	e_offset = (1-t)*(1-u)*s0 + t*(1-u)*s1+t*u*s2+(1-t)*u*s3;
	s0 = north_shift[n_index][e_index];
	s1 = north_shift[n_index][e_index+1];
	s2 = north_shift[n_index+1][e_index+1];
	s3 = north_shift[n_index+1][e_index];
	n_offset = (1-t)*(1-u)*s0 + t*(1-u)*s1+t*u*s2+(1-t)*u*s3;
	*e = osgrs80e + e_offset;
	*n = osgrs80n + n_offset;
	return(0);
}


/* ---------------------------------------------------------------------------
 * Convert E,N to GRS80
 */
int
en_to_osgrs80(double e, double n, double *osgrs80e, double *osgrs80n)
{
	int e_index, n_index;
	double s0, s1, s2, s3;
	double t, u;
	double e_offset, n_offset;

	e_index = e / 350000;
	n_index = n / 350000;
	t = (e - e_index * 350000 ) / 350000;
	u = (n - n_index * 350000 ) / 350000;
	s0 = east_shift[n_index][e_index];
	s1 = east_shift[n_index][e_index+1];
	s2 = east_shift[n_index+1][e_index+1];
	s3 = east_shift[n_index+1][e_index];
	e_offset = (1-t)*(1-u)*s0 + t*(1-u)*s1+t*u*s2+(1-t)*u*s3;
	s0 = north_shift[n_index][e_index];
	s1 = north_shift[n_index][e_index+1];
	s2 = north_shift[n_index+1][e_index+1];
	s3 = north_shift[n_index+1][e_index];
	n_offset = (1-t)*(1-u)*s0 + t*(1-u)*s1+t*u*s2+(1-t)*u*s3;
	*osgrs80e = e - e_offset;
	*osgrs80n = n - n_offset;
	return(0);
}


/* ---------------------------------------------------------------------------
 * Calculate the arc of meridian
 */
static int
arc_of_meridian(double phi1, double phi2, double *M)
{
	register double n2 = n*n;
	register double n3 = n2*n;
	register double phiplus = phi2 + phi1;
	register double phiminus = phi2 - phi1;

	*M = bF0 * (((1+n+n2*5/4+n3*5/4)*phiminus) -
		((3*n+3*n2+n3*21/8)*sin(phiminus)*cos(phiplus)) +
		((n2*15/8+n3*15/8)*sin(2*phiminus)*cos(2*phiplus)) -
		(n3*35/24*sin(3*phiminus)*cos(3*phiplus)));
	return(0);
}


/* ---------------------------------------------------------------------------
 * Convert E,N to Lat,lon
 */
int
en_to_latlon(double E, double N, double *lat, double *lon)
{
	register double phi_guess = phi0;
	double M = 0;
	int count = 0;
	double sin2phi;
	double nu;        /* radius of curvature at lat. phi perp. to a meridian */
	double rho;       /* radius of curvature of a meridian at lat. phi */
	double eta2;
	double eVII, eVIII, eIX, eX, eXI, eXII, eXIIA;
	double tanphi, tan2phi, tan4phi, tan6phi;
	double secphi;
	double y, y2, y3, y4, y5, y6, y7;
	double nu3, nu5, nu7;

	do
	{
		phi_guess = (N - N0 - M) / aF0 + phi_guess;
		if (verbose) printf("phi_guess is %lf\n", DEG(phi_guess));
		arc_of_meridian(phi0, phi_guess, &M);
		count++;
	}
	while (fabs(N - N0 - M) > 0.001);
	if (verbose) printf("M was calculated %d times\n", count);

	sin2phi = sin(phi_guess);
	sin2phi *= sin2phi;

	nu = aF0 / sqrt(1 - e2*sin2phi);
	rho = nu*(1-e2)/(1-e2*sin2phi);
	eta2 = nu/rho - 1;

	secphi = 1/cos(phi_guess);
	tanphi = tan(phi_guess);
	tan2phi = tanphi * tanphi;
	tan4phi = tan2phi * tan2phi;
	tan6phi = tan4phi * tan2phi;

	y = E - E0;
	y2 = y*y;
	y3 = y2*y;
	y4 = y3*y;
	y5 = y4*y;
	y6 = y5*y;
	y7 = y6*y;

	nu3 = nu*nu*nu;
	nu5 = nu3*nu*nu;
	nu7 = nu3*nu3*nu;

	eVII = tanphi / (2*rho*nu);
	eVIII = tanphi / (24*rho*nu3) * (5+3*tan2phi+eta2-9*tan2phi*eta2);
#ifdef ODD_TAN
	eVIII = tanphi / (24*rho*nu3) * (5+3*tan2phi+eta2-9*tan(phi_guess*eta2)*tan(phi_guess*eta2));
#endif
	eIX = tanphi / (720*rho*nu5) * (61+90*tan2phi+45*tan4phi);
	*lat = phi_guess - y2*eVII + y4*eVIII - y6*eIX;

	eX = secphi / nu;
	eXI = secphi / (6*nu3) * (nu/rho+2*tan2phi);
	eXII = secphi / (120*nu5) * (5+28*tan2phi+24*tan4phi);
	eXIIA = secphi / (5040*nu7) * (61+662*tan2phi+1320*tan4phi+720*tan6phi);
	*lon = lambda0 + y*eX - y3*eXI + y5*eXII - y7*eXIIA;

	if (verbose)
	{
		printf("nu   %.8lg\n", nu);
		printf("rho  %.8lg\n", rho);
		printf("eta2 %.8lg\n", eta2);
		printf("M    %.8lg\n", M);
		printf("y    %.8lg\n", y);
		printf("VII  %.8lg\n", eVII);
		printf("VIII %.8lg\n", eVIII);
		printf("IX   %.8lg\n", eIX);
		printf("X    %.8lg\n", eX);
		printf("XI   %.8lg\n", eXI);
		printf("XII  %.8lg\n", eXII);
		printf("XIIA %.8lg\n", eXIIA);
	}

	*lat = DEG(*lat);
	*lon = DEG(*lon);
	return(0);
}


/* ---------------------------------------------------------------------------
 * Convert lat,lon to E,N
 */
int
latlon_to_en(double lat, double lon, double *E, double *N)
{
	double M;
	double phi = RAD(lat);
	double sinphi, sin2phi;
	double cosphi, cos3phi, cos5phi;
	double tan2phi, tan4phi;
	double nu;        /* radius of curvature at lat. phi perp. to a meridian */
	double rho;       /* radius of curvature of a meridian at lat. phi */
	double eta2;
	double P, P2, P3, P4, P5, P6;
	double eI, eII, eIII, eIIIA, eIV, eV, eVI;

	arc_of_meridian(phi0, phi, &M);

	sinphi = sin(phi);
	sin2phi = sinphi * sinphi;

	nu = aF0 / sqrt(1 - e2*sin2phi);
	rho = nu*(1-e2)/(1-e2*sin2phi);
	eta2 = nu/rho - 1;

	P = RAD(lon) - lambda0;
	P2 = P * P;
	P3 = P2 * P;
	P4 = P3 * P;
	P5 = P4 * P;
	P6 = P5 * P;

	cosphi = cos(phi);
	cos3phi = cosphi * cosphi * cosphi;
	cos5phi = cos3phi * cosphi * cosphi;
	tan2phi = tan(phi);
	tan2phi = tan2phi * tan2phi;
	tan4phi = tan2phi * tan2phi;

	eI = M + N0;
	eII = nu / 2 * sinphi * cosphi;
	eIII = nu / 24 * sinphi * cos3phi * (5-tan2phi+9*eta2);
	eIIIA = nu / 720 * sinphi * cos5phi * (61-58*tan2phi+tan4phi);
	*N = eI + P2*eII + P4*eIII + P6*eIIIA;

	eIV = nu*cosphi;
	eV = nu/6*cos3phi*(nu/rho-tan2phi);
	eVI = nu/120*cos5phi*(5-18*tan2phi+tan4phi+14*eta2-58*tan2phi*eta2);
#ifdef ODD_TAN
	eVI = nu/120*cos5phi*(5-18*tan2phi+tan4phi+14*eta2-58*tan(phi*eta2)*tan(phi*eta2));
#endif
	*E = E0 + P*eIV + P3*eV + P5*eVI;

	if (verbose)
	{
		printf("nu   %.8lg\n", nu);
		printf("rho  %.8lg\n", rho);
		printf("eta2 %.8lg\n", eta2);
		printf("M    %.8lg\n", M);
		printf("P    %.8lg\n", P);
		printf("I    %.8lg\n", eI);
		printf("II   %.8lg\n", eII);
		printf("III  %.8lg\n", eIII);
		printf("IIIA %.8lg\n", eIIIA);
		printf("IV   %.8lg\n", eIV);
		printf("V    %.8lg\n", eV);
		printf("VI   %.8lg\n", eVI);
	}

	return(0);
}


/* ---------------------------------------------------------------------------
 * Test program
 */
#ifdef MAIN
static char *prog;
static char *usage = "usage: %s [-v] [-e ellipsoid] [east north]\n"
	"-e\tellipsoid, value should be airy, utm or grs80\n"
	"Note: GPS uses WGS84 datum with GRS80 ellipsoid,\n"
	"The OS National Grid uses the OSGB36 datum with Airy ellipsoid.\n";
static char *options = "ve:";
int
main(int argc, char *argv[])
{
	int c, n=0;
	char *strp;
	double easting = 651409.903;  /* should convert to 1.7179215833 */
	double northing = 313177.271; /* should convert to 52.6575703055 */
	double lat, lon, height, x, y, z;
	ellipsoid_t ellipsoid = E_Airy;

	prog = argv[0];

	GETOPT(c, options)
	{
		case 'e':
			switch (*optarg)
			{
				case 'a': case 'A': ellipsoid = E_Airy; break;
				case 'u': case 'U': ellipsoid = E_UTM; break;
				case 'g': case 'G': ellipsoid = E_GRS80; break;
				default: break;
			}
			break;
		case 'v': verbose = 1; break;
		case '?': fprintf(stderr, usage, prog);
	}

	GETOPT_LOOP_REST(strp)
	{
		if (n==0) easting = atof(strp);
		else if (n==1) northing = atof(strp);
		else fprintf(stderr,usage,prog);
		n++;
	}

	OSMap_InitConstants(ellipsoid);

	if (getenv("LL_TEST1"))
	{
		double lat = 52 + 39/60.0 + 27.2531/3600;
		double lon =  1 + 43/60.0 +  4.5177/3600;
		double H = 24.7;
		double x, y, z;
		OSMap_InitConstants(E_Airy);
		latlon_to_cartesian(lat, lon, H, &x, &y, &z);
		printf("%f %f %f -> %f %f %f\n", lat, lon, H, x, y, z);
		OSMap_InitConstants(E_GRS80);
		cartesian_to_latlon(x, y, z, &lat, &lon, &H);
		printf("%f %f %f <- %f %f %f\n", lat, lon, H, x, y, z);
	}

	if (getenv("LL_TEST2"))
	{
		latlon_to_en(56.458, -2.978, &easting, &northing);
		printf("easting: %lf\n", easting);
	    printf("northing: %lf\n", northing);
		exit(0);
	}

	if (ellipsoid == E_GRS80)
	{
		/* Not entirely sure if this this is correct to call here. */
		/*
		en_to_osgrs80(easting, northing, &easting, &northing);
		*/
	}

	/* Convert E,N to Lon,Lat */
	en_to_latlon(easting, northing, &lat, &lon);
	printf("latitude: %lf\n", lat);
	printf("longitude: %lf\n", lon);

	/* Convert back again to verify */
	latlon_to_en(lat, lon, &easting, &northing);
	printf("easting: %lf\n", easting);
	printf("northing: %lf\n", northing);

	/* Convert to WGS84 lat,lon */
	/* 1. to cartesian */
	OSMap_InitConstants(E_Airy);
	latlon_to_cartesian(lat, lon, 0, &x, &y, &z);
	if (verbose) printf("AIRY  x=%f y=%f z=%f\n", x, y, z);
	/* 2. convert datum */
	helmert_osgb36_to_wgs84(&x, &y, &z);
	if (verbose) printf("GRS80 x=%f y=%f z=%f\n", x, y, z);
	/* 3. from cartesian */
	OSMap_InitConstants(E_GRS80);
	cartesian_to_latlon(x, y, z, &lat, &lon, &height);
	printf("latitude (WGS84): %lf\n", lat);
	printf("longitude (WGS84): %lf\n", lon);

	return(0);
}
#endif /* MAIN */
