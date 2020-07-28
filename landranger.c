/* > landranger.c
 * 1.10 arb Thu May 15 13:33:38 BST 2003 - added BinOutline with ellipsoid
 *      parameter, change Bin to assume E_GRS80, fix GRID_START code (was
 *      at end), added error checks, added re-initialisation check, tidied.
 * 1.00 arb Tue May 28 10:46:14 BST 1996
 */

static char SCCSid[] = "@(#)landranger.c  1.10 (C) 1996 arb mapLib: LandRanger maps";

#include <stdio.h>
#include "satlib/dundee.h"
#include "osmap.h"
#include "landranger.h"

/*
 * Configuration:
 * Coordinates are given as km from SV00 to top left corner of map,
 * and stored in array as metres.
 * Maps are 40km square.
 *  NOTE  _top_left_corner_
 * Define MAPDATA_MAPPER to use map coord from Psion "Mapper" program,
 * otherwise arb's manually entered coords will be used.
 * Define GRID_START,_POINT as grid .bin file codes.
 */

#define SCALE 1000           /* Scale read value to m */
#define TRANSLATE 0          /* Translate read value to m */
#define MAPDATA_MAPPER
#define GRID_START 101
#define GRID_POINT  10

/*
 * To do:
 */

/*
 * Bugs:
 * mapping from coord to map number would not work for odd islands
 */

/*
 * Public exported arrays hold coordinate of top left corner of map
 * in m from false origin (ie. SV 0000).
 */
int Landranger_East[MAX_LANDRANGER+1];
int Landranger_North[MAX_LANDRANGER+1];

/* Flag set non-zero after initialisation */
static int Landranger_InitState = 0;


/* ----------------------------------------------------------------------------
 * Data
 */

/*
 * Private data arrays prior to conversion during Init into metres.
 * Initial entry.  Column 4 contains XXX if entry had an ambiguity.
 */
typedef struct landy_data_s
{
	int map;        /* Landranger map number 1 to 204 */
	int east;       /* Top left in km from false origin (see SCALE) */
	int north;      /* Top left in km from false origin (see SCALE) */
} landy_data_t;

static landy_data_t landy_data[] =
{
#ifdef MAPDATA_MAPPER
	{ 1, 429, 1220 },
	{ 2, 433, 1196 },
	{ 3, 414, 1187 },
	{ 4, 420, 1147 },
	{ 5, 340, 1060 },
	{ 6, 321, 1036 },
	{ 7, 315, 1010 },
	{ 8, 118,  966 },
	{ 9, 212,  980 },
	{ 10, 252, 980 },
	{ 11, 292, 969 },
	{ 12, 300, 979 },
	{ 13,  95, 943 },
	{ 14, 106, 927 },
	{ 15, 196, 940 },
	{ 16, 236, 940 },
	{ 17, 276, 940 },
	/* 18 returns UNMAPPED */
	{ 19, 174, 900 },
	{ 20, 214, 900 },
	{ 21, 254, 900 },
	{ 22,  57, 863 },
	{ 23, 113, 876 },
	{ 24, 150, 870 },
	{ 25, 190, 860 },
	{ 26, 230, 860 },
	{ 27, 270, 870 },
	{ 28, 310, 873 },
	{ 29, 345, 870 },
	{ 30, 377, 870 },
	{ 31,  50, 823 },
	{ 32, 130, 840 },
	{ 33, 170, 830 },
	{ 34, 210, 820 },
	{ 35, 250, 830 },
	{ 36, 285, 833 },
	{ 37, 325, 833 },
	{ 38, 365, 830 },
	{ 39, 120, 810 },
	{ 40, 160, 800 },
	{ 41, 200, 790 },
	{ 42, 240, 790 },
	{ 43, 280, 800 },
	{ 44, 320, 800 },
	{ 45, 360, 800 },
	{ 46,  92, 773 },
	{ 47, 120, 773 },
	{ 48, 120, 750 },
	{ 49, 160, 760 },
	{ 50, 200, 750 },
	{ 51, 240, 760 },
	{ 52, 270, 760 },
	{ 53, 294, 760 },
	{ 54, 334, 760 },
	{ 55, 164, 720 },
	{ 56, 204, 722 },
	{ 57, 244, 722 },
	{ 58, 284, 730 },
	{ 59, 324, 730 },
	{ 60, 110, 680 },
	{ 61, 132, 702 },
	{ 62, 160, 680 },
	{ 63, 200, 682 },
	{ 64, 240, 685 },
	{ 65, 280, 690 },
	{ 66, 316, 690 },
	{ 67, 356, 690 },
	{ 68, 157, 640 },
	{ 69, 175, 653 },
	{ 70, 215, 645 },
	{ 71, 255, 645 },
	{ 72, 280, 660 },
	{ 73, 320, 660 },
	{ 74, 357, 660 },
	{ 75, 390, 660 },
	{ 76, 195, 610 },
	{ 77, 235, 610 },
	{ 78, 275, 620 },
	{ 79, 315, 620 },
	{ 80, 355, 620 },
	{ 81, 395, 620 },
	{ 82, 195, 570 },
	{ 83, 235, 570 },
	{ 84, 265, 580 },
	{ 85, 305, 580 },
	{ 86, 345, 580 },
	{ 87, 367, 580 },
	{ 88, 407, 580 },
	{ 89, 290, 540 },
	{ 90, 317, 540 },
	{ 91, 357, 540 },
	{ 92, 380, 540 },
	{ 93, 420, 540 },
	{ 94, 460, 525 },
	{ 95, 213, 505 },
	{ 96, 303, 500 },
	{ 97, 326, 500 },
	{ 98, 366, 500 },
	{ 99, 406, 500 },
	{ 100, 446, 500 },
	{ 101, 486, 500 },
	{ 102, 326, 460 },
	{ 103, 360, 460 },
	{ 104, 400, 460 },
	{ 105, 440, 460 },
	{ 106, 480, 460 },
	{ 107, 500, 460 },
	{ 108, 320, 420 },
	{ 109, 360, 420 },
	{ 110, 400, 420 },
	{ 111, 440, 420 },
	{ 112, 470, 425 },
	{ 113, 510, 426 },
	{ 114, 220, 400 },
	{ 115, 240, 385 },
	{ 116, 280, 385 },
	{ 117, 320, 380 },
	{ 118, 360, 380 },
	{ 119, 400, 380 },
	{ 120, 440, 390 },
	{ 121, 478, 390 },
	{ 122, 518, 390 },
	{ 123, 210, 360 },
	{ 124, 250, 345 },
	{ 125, 280, 345 },
	{ 126, 320, 340 },
	{ 127, 360, 340 },
	{ 128, 400, 348 },
	{ 129, 440, 350 },
	{ 130, 480, 350 },
	{ 131, 520, 350 },
	{ 132, 560, 350 },
	{ 133, 600, 350 },
	{ 134, 617, 330 },
	{ 135, 250, 305 },
	{ 136, 280, 305 },
	{ 137, 320, 300 },
	{ 138, 345, 300 },
	{ 139, 385, 308 },
	{ 140, 425, 310 },
	{ 141, 465, 310 },
	{ 142, 505, 314 },
	{ 143, 537, 314 },
	{ 144, 577, 310 },
	{ 145, 200, 260 },
	{ 146, 240, 265 },
	{ 147, 270, 280 },
	{ 148, 310, 280 },
	{ 149, 333, 268 },
	{ 150, 373, 268 },
	{ 151, 413, 270 },
	{ 152, 453, 270 },
	{ 153, 493, 274 },
	{ 154, 533, 274 },
	{ 155, 573, 274 },
	{ 156, 613, 290 },
	{ 157, 165, 241 },
	{ 158, 189, 230 },
	{ 159, 229, 225 },
	{ 160, 269, 245 },
	{ 161, 309, 245 },
	{ 162, 349, 228 },
	{ 163, 389, 230 },
	{ 164, 429, 230 },
	{ 165, 460, 235 },
	{ 166, 500, 234 },
	{ 167, 540, 234 },
	{ 168, 580, 234 },
	{ 169, 607, 250 },
	{ 170, 269, 205 },
	{ 171, 309, 205 },
	{ 172, 340, 195 },
	{ 173, 380, 195 },
	{ 174, 420, 195 },
	{ 175, 460, 195 },
	{ 176, 495, 200 },
	{ 177, 530, 200 },
	{ 178, 565, 195 },
	{ 179, 603, 173 },
	{ 180, 240, 152 },
	{ 181, 280, 152 },
	{ 182, 320, 170 },
	{ 183, 349, 155 },
	{ 184, 389, 155 },
	{ 185, 429, 156 },
	{ 186, 465, 165 },
	{ 187, 505, 165 },
	{ 188, 545, 165 },
	{ 189, 585, 155 },
	{ 190, 207, 127 },
	{ 191, 247, 112 },
	{ 192, 287, 112 },
	{ 193, 310, 130 },
	{ 194, 349, 115 },
	{ 195, 389, 115 },
	{ 196, 429, 116 },
	{ 197, 469, 130 },
	{ 198, 509, 130 },
	{ 199, 549, 130 },
	{ 200, 175,  90 },
	{ 201, 215,  87 },
	{ 202, 255,  72 },
	/* 203 returns UNMAPPED */
	{ 204, 172,  54 }
#else /* ARB's data below */
	{  9, 212, 980 },
	{ 10, 252, 980 },
	{ 11, 292, 969 },
	{ 12, 300, 979 },
	{ 15, 196, 940 },
	{ 16, 236, 940 },
	{ 17, 276, 940 },
	{ 19, 174, 900 },
	{ 20, 214, 900 },
	{ 21, 254, 900 },
	{ 24, 150, 870 },
	{ 25, 190, 860 },
	{ 26, 230, 860 },
	{ 27, 270, 870 },
	{ 28, 310, 873 },
	{ 29, 345, 870 }, /* XXX */
	{ 30, 377, 870 },
	{ 32, 130, 840 },
	{ 33, 170, 830 },
	{ 34, 210, 820 },
	{ 35, 250, 830 },
	{ 36, 285, 833 }, /* XXX */
	{ 37, 325, 833 }, /* XXX */
	{ 38, 365, 830 }, /* XXX */
	{ 40, 160, 800 },
	{ 41, 200, 790 },
	{ 42, 240, 790 },
	{ 43, 280, 800 },
	{ 44, 320, 800 },
	{ 45, 360, 800 },
	{ 47, 120, 773 },
	{ 48, 120, 750 },
	{ 49, 160, 760 },
	{ 50, 200, 750 },
	{ 51, 240, 760 },
	{ 52, 270, 760 },
	{ 53, 294, 760 },
	{ 54, 334, 760 },
	{ 55, 164, 720 },
	{ 56, 204, 722 },
	{ 57, 244, 722 },
	{ 58, 284, 730 },
	{ 59, 324, 730 }
#endif /* !MAPDATA_MAPPER */
};

char *Landranger_Name[] =
{
	"Old Mother Goose", /* No Landranger map zero */
	"Shetland - Yell & Unst",
	"Shetland - Whalsay",
	"Shetland - North Mainland",
	"Shetland - South Mainland",
	"Orkney - Northern Isles",
	"Orkney - Mainland",
	"Orkney - Southern Isles",
	"Stornoway & North Lewis",
	"Cape Wrath",
	"Strathnaver",
	"Thurso & Dunbeath",
	"Thurso, Wick & surrounding area",
	"West Lewis & North Harris",
	"Tarbert & Loch Seaforth",
	"Loch Assynt & surrounding area",
	"Lairg, Loch Shin & surrounding area",
	"Helmsdale & Strath of Kildonan",
	"Sound of Harris & St Kilda",
	"Gairloch & Ullapool area",
	"Beinn Dearg & surrounding area",
	"Dornoch, Alness & Invergordon area",
	"Benbecula",
	"North Skye",
	"Raasay, Applecross & Loch Torridon",
	"Glen Carron & surrounding area",
	"Inverness & Strathglass area",
	"Nairn, Forres & surrounding area",
	"Elgin, Dufftown & surrounding area",
	"Banff & surrounding area",
	"Fraserburgh, Peterhead & District",
	"Barra & surrounding Islands",
	"South Skye",
	"Loch Alsh, Glen Shiel & surrounding area",
	"Fort Augustus & Glen Albyn area",
	"Kingussie & Monadhliath Mountains",
	"Grantown, Aviemore & Cairngorm area",
	"Strathdon & surrounding area",
	"Aberdeen & surrounding area",
	"Rùm & Eigg",
	"Loch Shiel",
	"Ben Nevis, Fort William & surrounding area",
	"Glen Garry & Loch Rannoch area",
	"Braemar to Blair Atholl",
	"Ballater, Glen Clova & surrounding area",
	"Stonehaven & surrounding area",
	"Coll & Tiree",
	"Tobermory & North Mull",
	"Iona, Ulva & West Mull",
	"Oban & East Mull",
	"Glen Orchy & surrounding area",
	"Loch Tay & surrounding area",
	"Pitlochry to Crieff",
	"Blairgowrie & surrounding area",
	"Dundee to Montrose",
	"Lochgilphead & surrounding area",
	"Loch Lomond & Inveraray area",
	"Stirling & The Trossachs area",
	"Perth to Alloa",
	"St Andrews & Kirkcaldy area",
	"Islay",
	"Jura & Colonsay",
	"North Kintyre area",
	"Firth of Clyde area",
	"Glasgow & surrounding area",
	"Falkirk & West Lothian",
	"Edinburgh & Midlothian area",
	"Duns, Dunbar & Eyemouth area",
	"South Kintyre",
	"Isle of Arran",
	"Ayr, Kilmarnock & surrounding area",
	"Lanark & Upper Nithsdale area",
	"Upper Clyde Valley",
	"Peebles, Galashiels & surrounding area",
	"Kelso & surrounding area",
	"Berwick-upon-Tweed & surrounding area",
	"Girvan & surrounding area",
	"Dalmellington to New Galloway",
	"Nithsdale & Annandale area",
	"Hawick & Eskdale area",
	"Cheviot Hills & Kielder Forest area",
	"Alnwick, Morpeth & surrounding area",
	"Stranraer, Glenluce & surrounding area",
	"Newton Stewart & Kirkcudbright area",
	"Dumfries, Castle Douglas & surrounding area",
	"Carlisle & The Solway Firth",
	"Haltwhistle, Bewcastle & Alston area",
	"Hexham, Haltwhistle & surrounding area",
	"Tyneside & Durham area",
	"West Cumbria",
	"Penrith, Keswick & Ambleside area",
	"Appleby-in-Westmorland",
	"Barnard Castle & surrounding area",
	"Middlesbrough & Darlington area",
	"Whitby & surrounding area",
	"Isle of Man",
	"Barrow-in-Furness & South Lakeland area",
	"Kendal & Morecambe",
	"Wensleydale & Upper Wharfedale",
	"Northallerton, Ripon & surrounding area",
	"Malton, Pickering & surrounding area",
	"Scarborough & Bridlington area",
	"Preston, Blackpool & surrounding area",
	"Blackburn, Burnley & surrounding area",
	"Leeds, Bradford & Harrogate area",
	"York & surrounding area",
	"Market Weighton & surrounding area",
	"Kingston upon Hull & surrounding area",
	"Liverpool & surrounding area",
	"Manchester & surrounding area",
	"Sheffield & Huddersfield area",
	"Sheffield & Doncaster area",
	"Scunthorpe & surrounding area",
	"Grimsby & surrounding area",
	"Anglesey",
	"Snowdon & surrounding area",
	"Denbigh & Colwyn Bay area",
	"Chester, Wrexham & surrounding area",
	"Stoke-on-Trent & Macclesfield area",
	"Buxton, Matlock & Dove Dale area",
	"Mansfield, Worksop & surrounding area",
	"Lincoln & surrounding area",
	"Skegness area",
	"Lleyn Peninsula",
	"Dolgellau & surrounding area (Special Edition)",
	"Bala & Lake Vyrnwy & surrounding area",
	"Shrewsbury & surrounding area",
	"Stafford, Telford & surrounding area",
	"Derby & Burton upon Trent area",
	"Nottingham & Loughborough area",
	"Grantham & surrounding area",
	"Boston & Spalding area",
	"North West Norfolk",
	"North East Norfolk",
	"Norwich & The Broads",
	"Aberystwyth & surrounding area",
	"Newtown, Llanidloes & surrounding area",
	"Ludlow, Wenlock Edge & surrounding area",
	"Kidderminster & Wyre Forest area",
	"Birmingham & surrounding area",
	"Leicester & Coventry area",
	"Kettering, Corby & surrounding area",
	"Peterborough & surrounding area",
	"Ely, Wisbech & surrounding area",
	"Thetford, Breckland & surrounding area",
	"Cardigan & surrounding area",
	"Lampeter, Llandovery & surrounding area",
	"Elan Valley & Builth Wells area",
	"Presteigne & Hay-on-Wye area",
	"Hereford, Leominster & surrounding area",
	"Worcester, The Malverns & surrounding area",
	"Stratford-upon-Avon & surrounding area",
	"Northampton & Milton Keynes area",
	"Bedford, Huntingdon & surrounding area",
	"Cambridge, Newmarket & surrounding area",
	"Bury St Edmunds & Sudbury area",
	"Saxmundham, Aldeburgh & surrounding area",
	"St David's & Haverfordwest area",
	"Tenby & surrounding area",
	"Swansea, Gower & surrounding area",
	"Brecon Beacons",
	"Abergavenny & The Black Mountains",
	"Gloucester & Forest of Dean area",
	"Cheltenham & Cirencester area",
	"Oxford & surrounding area",
	"Aylesbury & Leighton Buzzard area",
	"Luton, Hertford & surrounding area",
	"Chelmsford, Harlow & surrounding area",
	"Colchester & The Blackwater area",
	"Ipswich & The Naze area",
	"Vale of Glamorgan & Rhondda area",
	"Cardiff, Newport & surrounding area",
	"Bristol, Bath & surrounding area",
	"Swindon, Devizes & surrounding area",
	"Newbury, Wantage & surrounding area",
	"Reading, Windsor & surrounding area",
	"West London area",
	"East London area",
	"The Thames Estuary",
	"Canterbury & East Kent area",
	"Barnstaple & Ilfracombe area",
	"Minehead & Brendon Hills area",
	"Weston-super-Mare & Bridgwater area",
	"Yeovil & Frome",
	"Salisbury & The Plain",
	"Winchester & Basingstoke area",
	"Aldershot, Guildford & surrounding area",
	"Dorking, Reigate & Crawley area",
	"Maidstone & The Weald of Kent",
	"Ashford & Romney Marsh area",
	"Bude, Clovelly & surrounding area",
	"Okehampton & North Dartmoor area",
	"Exeter, Sidmouth & surrounding area",
	"Taunton & Lyme Regis",
	"Dorchester, Weymouth & surrounding area",
	"Bournemouth, Purbeck & surrounding area",
	"Solent & The Isle of Wight",
	"Chichester & The Downs",
	"Brighton & The Downs",
	"Eastbourne, Hastings & surrounding area",
	"Newquay, Bodmin & surrounding area",
	"Plymouth & Launceston area",
	"Torbay & South Dartmoor area",
	"Land's End, The Lizard & Isles of Scilly",
	"Truro, Falmouth & surrounding area"
};


/* ----------------------------------------------------------------------------
 * Landranger_Init - must be called prior to using any Landranger functions.
 * May be called multiple times.
 */
int
Landranger_Init()
{
	int i, j, k;

	if (Landranger_InitState)
		return(0);

#if 0
	FILE *fp;
	char *datafile = "/users/local/arb/C/Map/landranger.c";
	char line[256];

	for (i=1; i<=MAX_LANDRANGER; i++)
		Landranger_East[i] = Landranger_North[i] = UNMAPPED;

	fp = fopen(datafile, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open %s\n", datafile);
		return(-1);
	}
	while (fgets(line, 256, fp))
	{
		if (sscanf(line, "%d\t%d\t%d\t", &i, &j, &k) == 3)
		{
			Landranger_East[i] = j * SCALE + TRANSLATE;
			Landranger_North[i] = k * SCALE + TRANSLATE;
		}
	}
	fclose(fp);
#endif /* 0 */

	for (i=0; i<sizeof(landy_data)/sizeof(landy_data_t); i++)
	{
		Landranger_East[landy_data[i].map] = landy_data[i].east * SCALE + TRANSLATE;
		Landranger_North[landy_data[i].map] = landy_data[i].north * SCALE + TRANSLATE;
	}

	Landranger_InitState = 1;

	return(0);
}


/* ----------------------------------------------------------------------------
 * Landranger_FromGrid
 *   Find out which maps contain the given coordinate.
 * Parameters:
 *   E,N - input metres offset from false origin SV 0000.
 *   maps - array returning map numbers (must be at least size of 4 integers)
 * Returns:
 *   Returns number of maps which contain the given coordinate.
 */
int
Landranger_FromGrid(int E, int N, int *maps)
{
	int i, num=0;

	/* Initialise if not already done */
	Landranger_Init();

	for (i=0; i<=MAX_LANDRANGER; i++)
	{
		if (Landranger_East[i] == UNMAPPED)
			continue;
		if ((E>=Landranger_East[i]) &&
			(E<Landranger_East[i]+MAPSQUARE) &&
			(N<=Landranger_North[i]) &&
			(N>Landranger_North[i]-MAPSQUARE))
		{
			maps[num++] = i;
		}
	}
	return(num);
}


/* ----------------------------------------------------------------------------
 * Landranger_BinOutline
 *   Return an array of points marking the outline of the map, suitable
 * for plotting using satlib or saving to a .bin grid file in $HGRIDSDIR.
 * Usually just plots the four corners.
 * Note:
 *   You must call OSMap_InitConstants appropriately first to initialise the
 * ellipsoid to GRS80, and you must call Landranger_Init too.
 * Parameters:
 *   ellipsoid - the desired ellipsoid
 *   mapnum - Landranger map number
 *   points - returns 5 points in the array which must be size 5 at least.
 * Returns:
 *   0 for success, -1 if map has no known coordinates.
 */
int
Landranger_BinOutline(ellipsoid_t ellipsoid, int mapnum, gridpoint_t *points)
{
	double E, N, e, n;
	int p;
	double lat, lon;
	DOUBLE_TMP;

	/* Initialise if not already done */
	Landranger_Init();

	if (Landranger_East[mapnum] == UNMAPPED ||
		Landranger_North[mapnum] == UNMAPPED)
		return(-1);

	/* Set line types */
	for (p=0; p<5; p++)
		points[p].code = GRID_POINT;
	points[0].code = GRID_START;

	/* Find top left corner of map */
	E = e = Landranger_East[mapnum];
	N = n = Landranger_North[mapnum];
	p = 0;

	/* Create square using lat,lon */
	if (ellipsoid == E_GRS80) en_to_osgrs80(e, n, &e, &n);
	en_to_latlon(e, n, &lat, &lon);
	points[p].lat = lat; points[p].lon = MOD360(-lon); p++;

	e = E+MAPSQUARE; n = N;
	if (ellipsoid == E_GRS80) en_to_osgrs80(e, n, &e, &n);
	en_to_latlon(e, n, &lat, &lon);
	points[p].lat = lat; points[p].lon = MOD360(-lon); p++;

	e = E+MAPSQUARE; n = N-MAPSQUARE;
	if (ellipsoid == E_GRS80) en_to_osgrs80(e, n, &e, &n);
	en_to_latlon(e, n, &lat, &lon);
	points[p].lat = lat; points[p].lon = MOD360(-lon); p++;

	e = E; n = N-MAPSQUARE;
	if (ellipsoid == E_GRS80) en_to_osgrs80(e, n, &e, &n);
	en_to_latlon(e, n, &lat, &lon);
	points[p].lat = lat; points[p].lon = MOD360(-lon); p++;

	e = E; n = N;
	if (ellipsoid == E_GRS80) en_to_osgrs80(e, n, &e, &n);
	en_to_latlon(e, n, &lat, &lon);
	points[p].lat = lat; points[p].lon = MOD360(-lon); p++;

	return(0);
}

/*
 * Assumes ellipsoid GRS80
 */
int
Landranger_Bin(int mapnum, gridpoint_t *points)
{
	return Landranger_BinOutline(E_GRS80, mapnum, points);
}


/* ----------------------------------------------------------------------------
 * Main program.
 * Write gridfile of landranger maps to stdout.
 */
#ifdef MAIN
#include "llen.h"
int
main(int argc, char *argv[])
{
	FILE *fpout = stdout;
	gridpoint_t points[5];
	int i;

	OSMap_InitConstants(E_GRS80);
	Landranger_Init();
	for (i=1; i<=MAX_LANDRANGER; i++)
	{
		if (Landranger_BinOutline(E_GRS80, i, points) == 0)
			fwrite(points, sizeof(points), 1, fpout);
	}
	return(0);
}
#endif /* MAIN */
