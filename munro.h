/* > munro.h
 * 2.00 arb Thu May 15 13:43:09 BST 2003 - added post-1997 munros
 *      and updated from new munro database but had to remove reascent
 * 1.00 arb
 */

#ifndef __MUNRO_H
#define __MUNRO_H

#include "osmap.h"

#ifdef MUNROS_PRE1997
#define NUM_MUNROS 277
typedef struct munro_s
{
	char *region;
	int munronum;
	int topnum;
	int height;
	int reascent;
	char *maps;
	char *zone;
	int gridref;
	char *name;
} munro_t;

#else

#define NUM_MUNROS 284

typedef struct munro_s
{
	char *region;
	int munronum;
	int height;
	char *maps;
	char *zone;
	int gridref;
	char *name;
} munro_t;

#endif

extern munro_t munroTable[NUM_MUNROS];

#endif /* !__MUNRO_H */
