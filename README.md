# Ordnance Survey mapping library

(c) 2000 Andrew Brooks <arb@sat.dundee.ac.uk>

landranger - array of maps names, locations, and func for finding maps of area

munro - array of munro names, heights, locations, map numbers, ...

mapzone - find zone letters for given map number and gridref

map - convert between zone letters and grid refs

llen - convert between lat,lon and grid refs


| Datum  | Spheroid  | Used by
| ------ | --------- | -------
| OSGB36 | Airy      | OS National Grid
| WGS84  | GRS80     | GPS
| ETRF89 | update to WGS84


# Testing

As seen on MemoryMap (v5, explorer 380 1:25000) corner of Ewing building is:
NO 39670 30000; 
Lat,Lon = 56.45824, -2.98053

According to the Grid InQuest program (from qgsl via ordsvy):
Lat,Lon = 56.4582614948, -2.9805352438


% ./map NO 3967030000  (equivalent to NO 39673000, roughly NO 397300)
From SV is (  339670,  730000)

% ./llen 339670 730000
latitude: 56.458386
longitude: -2.979058
easting: 339670.000000
northing: 729999.999978
latitude (WGS84): 56.458271
longitude (WGS84): -2.980532

% ./os_to_latlon NO396300
56.458262,-2.981668

% ./os_to_latlon NO3967030000
56.458271,-2.980532


# API

To convert National Grid E,N (m) into WGS84 lat,lon (degrees)

OSMap_InitConstants(E_Airy);
en_to_latlon(easting, northing, &lat, &lon);
latlon_to_cartesian(lat, lon, 0, &x, &y, &z);
helmert_osgb36_to_wgs84(&x, &y, &z);
OSMap_InitConstants(E_GRS80);
cartesian_to_latlon(x, y, z, &lat, &lon, &height);

To convert WGS84 lat,lon (degrees) into National Grid E,N (m)

OSMap_InitConstants(E_GRS80);
latlon_to_cartesian();
helmert_osgb36_to_wgs84();
OSMap_InitConstants(E_Airy);
cartesian_to_latlon(x, y, z, &lat, &lon, &height);
latlon_to_en(lat, lon, &easting, &northing);


# Using GDAL

echo "380150 402109" | gdaltransform -s_srs EPSG:27700 -t_srs EPSG:4326
-2.30083027968938 53.5152719980087 48.8662291513756
Please note that behind the scenes, these datum shift parameters are applied: +towgs84=375,-111,431,0,0,0,0 (which might be what you want .. or not).
Indeed, these are the default values for a 3-parameter transformation (I'm using GDAL 1.9.2).
On page 33 of [1], OSGB gives the values for a 7-parameter transformation which should be more accurate in most cases. You have to revert the signs of the given values and use them like this: +towgs84=446.448,-125.157,542.06,0.1502,0.247,0.8421,-20.4894
And there is OSGB's grid shift file at [2] which leads to even more accurate results.
[1] http://www.ordnancesurvey.co.uk/oswebsite/gps/docs/A_Guide_to_Coordinate_Systems_in_Great_Britain.pdf 
[2] http://www.ordnancesurvey.co.uk/oswebsite/gps/osnetfreeservices/furtherinfo/ostn02_ntv2.html


 Resources

Information http://www.bangor.ac.uk/is/iss025/osgbfaq.htm

Coordinate convertor http://www.osi.ie/gps/login.asp
Also http://www.killetsoft.de/

Coastline data from GEOnet  http://earth-info.nga.mil/gns/html/cntry_files.html
