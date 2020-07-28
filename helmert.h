/* > helmert.h
 * 1.00 arb 
 * Helmert transforms
 */

#ifndef HELMERT_H
#define HELMERT_H

#ifdef __cplusplus
extern "C" {
#endif


extern double helmert_etrs89_to_osgb36_dx;    /* m */
extern double helmert_etrs89_to_osgb36_dy;    /* m */
extern double helmert_etrs89_to_osgb36_dz;    /* m */
extern double helmert_etrs89_to_osgb36_scale; /* parts per million */
extern double helmert_etrs89_to_osgb36_xrot;  /* seconds of arc */
extern double helmert_etrs89_to_osgb36_yrot;  /* seconds of arc */
extern double helmert_etrs89_to_osgb36_zrot;  /* seconds of arc */


extern double helmert_x(double x, double y, double z, double dx, double roty, double rotz, double scale);
extern double helmert_y(double x, double y, double z, double dy, double rotx, double rotz, double scale);
extern double helmert_z(double x, double y, double z, double dz, double rotx, double roty, double scale);


extern void helmert_wgs84_to_osgb36(double *x, double *y, double *z);
extern void helmert_osgb36_to_wgs84(double *x, double *y, double *z);


#ifdef __cplusplus
}
#endif

#endif
