/*
 * http://www.movable-type.co.uk/scripts/latlong.html
 */

/* -------------------------------------------------------------------------
 * Great-circle distance between two points, i.e. shortest distance
 * over the earth's surface.
 */

/* 
 * Haversine forumula should be better than the spherical law of cosines
 * (below) alhough is you use doubles then the latter should be ok.
 */
#define MEANRAD 6371
remember lat,lon must be radians
dLon = lon2-lon1;
dLat = lat2-lat1;
a = sin(dLat/2) * sin(dLat/2) +
	cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);
c = 2 * atan2(sqrt(a), sqrt(1-a);
distance = MEANRAD * c;

/*
 * Spherical law of cosines
 */
c = acos(sin(lat1) * sin(lat2) +
	cos(lat1) * cos(lat2) *
	cos(lon2-lon1));
distance = MEANRAD * c;

/*
 * Initial bearing
 * (Final bearing = initial bearing from end to start, reversed)
 */
y = sin(dLon) * cos(lat2);
x = cos(lat1) * sin(lat2) -
	sin(lat1) * cos(lat2) * cos(dLon);
bearing = atan2(y, x)
except result is in the range -180 to +180 so to get 0..360 then
convert to degrees and use (degrees+360)mod360
to get final just use bearing = (bearing+180)mod360

/*
 * rhumb line
 * path of constant bearing which crosses all meridians at same angle
 * longer than great circle but is a straight line on a mercator map.
 */
var dPhi = Math.log(Math.tan(lat2/2+Math.PI/4)/Math.tan(lat1/2+Math.PI/4));
var q = (Math.abs(dLat) > 1e-10) ? dLat/dPhi : Math.cos(lat1);
// if dLon over 180° take shorter rhumb across 180° meridian:
if (Math.abs(dLon) > Math.PI) {
  dLon = dLon>0 ? -(2*Math.PI-dLon) : (2*Math.PI+dLon);
}
var d = Math.sqrt(dLat*dLat + q*q*dLon*dLon) * R;
var brng = Math.atan2(dLon, dPhi);
