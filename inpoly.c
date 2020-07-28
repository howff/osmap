static const char SCCSid[] = "@(#)inpoly.c      1.00 (C) 2010 Galacticomm In Polygon test";

/***************************************************************************
 *                                                                         *
 *   INPOLY.C                                                              *
 *                                                                         *
 *   Copyright (c) 1995-1996 Galacticomm, Inc.  Freeware source code.      *
 *                                                                         *
 *   Please feel free to use this source code for any purpose, commercial  *
 *   or otherwise, as long as you don't restrict anyone else's use of      *
 *   this source code.  Please give credit where credit is due.            *
 *                                                                         *
 *   Point-in-polygon algorithm, created especially for World-Wide Web     *
 *   servers to process image maps with mouse-clickable regions.           *
 *                                                                         *
 *   http://www.visibone.com/inpoly/inpoly.c                               *
 *                                                                         *
 *                                       6/19/95 - Bob Stein & Craig Yap   *
 *                                       stein@visibone.com                *
 *                                       craig@cse.fau.edu                 *
 *                                                                         *
 ***************************************************************************/

int                                /*   1=inside, 0=outside                */
inpoly(                            /* is target point inside a 2D polygon? */
unsigned int poly[][2],            /*   polygon points, [0]=x, [1]=y       */
int npoints,                       /*   number of points in polygon        */
unsigned int xt,                   /*   x (horizontal) of target point     */
unsigned int yt)                   /*   y (vertical) of target point       */
{
	unsigned int xnew,ynew;
	unsigned int xold,yold;
	unsigned int x1,y1;
	unsigned int x2,y2;
	int i;
	int inside=0;

	if (npoints < 3)
	{
		return(0);
	}

	xold=poly[npoints-1][0];
	yold=poly[npoints-1][1];
	for (i=0 ; i < npoints ; i++)
	{
		xnew=poly[i][0];
		ynew=poly[i][1];
		if (xnew > xold)
		{
			x1=xold;
			x2=xnew;
			y1=yold;
			y2=ynew;
		}
		else
		{
			x1=xnew;
			x2=xold;
			y1=ynew;
			y2=yold;
		}
		if ((xnew < xt) == (xt <= xold)         /* edge "open" at left end */
			&& ((long)yt-(long)y1)*(long)(x2-x1)
			< ((long)y2-(long)y1)*(long)(xt-x1))
		{
			inside=!inside;
		}
		xold=xnew;
		yold=ynew;
	}
	return(inside);
}

