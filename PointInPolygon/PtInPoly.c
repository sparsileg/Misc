static char *sccsid = "%W%	%G%";

/*  This file contains the source code for the TAMPS Strip Chart Generator
    Point in Polygon facility.

    This file consists of the following routines which are accessible to
    functions external to this file:

    bsa_IsPointInPolygon             Check to see if a point is inside
                                     an arbitrary convex or concave
                                     polygon

    This file also consists of the following routines which are local to
    this file:

    bsa_Intersect                    Check to see if an edge of the 
                                     polygon intersects a horizontal
                                     line extending to the right of 
                                     the point being investigated
*/



#include <stdio.h>

#include "bsa_Main.h"
#include "bsa_Error.h"

/*  Define the threshold to use for slope division */

#define bsa_SLOPE_THRESHOLD 0.001

/*  Declare functions which are local to this file. */

#ifndef bsa_NO_PROTO
static tbsa_Error bsa_Intersect(
                      double,          /* X coord of first point in line */ 
                      double,          /* Y coord of first point in line */ 
                      double,          /* X coord of second point in line */ 
                      double,          /* Y coord of second point in line */ 
                      double,          /* X coord of point to investigate */
                      double,          /* Y coord of point to investigate */
                      unsigned char *);/* t/f flag for intersection */
#endif
#ifdef bsa_NO_PROTO
static tbsa_Error bsa_Intersect();     /* check for intersection */
#endif



#ifndef bsa_NO_PROTO
tbsa_Error bsa_IsPointInPolygon(double         dXPoint,
                                double         dYPoint,
                                long           kNumberOfVertices,
                                double        *dXVertices,
                                double        *dYVertices,
                                unsigned char *lInside)
#endif
#ifdef bsa_NO_PROTO
tbsa_Error bsa_IsPointInPolygon(dXPoint,
                                dYPoint,
                                kNumberOfVertices,
                                dXVertices,
                                dYVertices,
                                lInside)

/*  Routine to see if a point is inside an arbitrary, convex or
    concave polygon.  Both the polygon and the point are expressed
    as doubles.

    If the point is inside the polygon, lInside is set to bsa_TRUE.
    If the point is on the polygon boundary, lInside is set to bsa_TRUE.
    If the point is outside the polygon, lInside is set to bsa_FALSE.

    It is assumed that the list of polygon vertices has only one 
    entry for the starting vertex.  In other words, it is assumed
    that the polygon is not closed.  This routine will automatically
    generate a polygon edge from the last vertex in the polygon
    vertext list to the frst vertext in the polygon vertex list.

    If successful, this routine returns bsa_OK.  Currently,
    this routine does not return any other error codes.

    Written by:  Steven A. Walker           Date:  January 3, 1993
    Modified by:                            Date: */

/*  Input Parameters */

    double         dXPoint;            /* x coord of point */
    double         dYPoint;            /* y coord of point */
    long           kNumberOfVertices;  /* # of vertices in polygon */
    double        *dXVertices;         /* list of polygon X vertices */
    double        *dYVertices;         /* list of polygon Y vertices */

/*  Output Parameters */

    unsigned char *lInside;            /* t/f flag for point inside poly */

#endif
{
/*  Local Variables */

    tbsa_Error    ReturnValue;
    long          k1;                      /* loop counter */
    unsigned char lInsideFlag;             /* 0 if pt is outside, 1 if inside */
    unsigned char lIntersect;              /* t/f flag for intersection */
    double        dXMax;                   /* max X vertex in polygon list */
    double        dXMin;                   /* min X vertex in polygon list */
    double        dYMax;                   /* max Y vertex in polygon list */
    double        dYMin;                   /* min Y vertex in polygon list */
    double        dX1;                     /* X coord of first pt in line */
    double        dY1;                     /* Y coord of first pt in line */
    double        dX2;                     /* X coord of second pt in line */
    double        dY2;                     /* Y coord of second pt in line */

/*  First, compute a bounding hull for the polygon. */

    dXMax = dXVertices[0];
    dYMax = dYVertices[0];
    dXMin = dXVertices[0];
    dYMin = dYVertices[0];
    for (k1 = 1; k1 < kNumberOfVertices; k1++)
    {
      dXMax = bsa_MAX(dXMax,dXVertices[k1]);
      dYMax = bsa_MAX(dYMax,dYVertices[k1]);
      dXMin = bsa_MIN(dXMin,dXVertices[k1]);
      dYMin = bsa_MIN(dYMin,dYVertices[k1]);
    }

/*  See if the point is outside the bouding hull.  If so, there
    can be no intersection. */

    if ((dXPoint > dXMax) ||
        (dYPoint > dYMax) ||
        (dXPoint < dXMin) ||
        (dYPoint < dYMin))
    {
      *lInside = bsa_FALSE;
      return(bsa_OK);
    }

/*  The point is inside the bouding hull of the polygon.  Therefore,
    we need to see if it is inside or outside the polygon.  The
    approach we use is to extend a horizontal line from the point to
    the right to "infinity".  We will then count the number or times
    the sides of the polygon intersect the horizontal line extended
    from the point.  If the number of intersections is odd, the point
    is inside the polygon.  If the number of intersections is even,
    the point is outside the polygon.  We will track even vs odd
    by flipping the value of lInsideFlag, which is initialized to
    zero (outside).

    By convention, if the horizontal line extended from the point
    lies along one of the polygon edges, we will not count it as
    an intersection. */

    lInsideFlag = 0;
    dX1 = dXVertices[0];
    dY1 = dYVertices[0];
    for (k1 = 1; k1 < kNumberOfVertices; k1++)
    {
      dX2 = dXVertices[k1];
      dY2 = dYVertices[k1];
      ReturnValue = bsa_Intersect(dX1,dY1,dX2,dY2,dXPoint,dYPoint,&lIntersect);
      if (ReturnValue != bsa_OK)
      {
        *lInside = bsa_FALSE;
        return(ReturnValue);
      }
      if (lIntersect == bsa_TRUE) lInsideFlag = !lInsideFlag;
      dX1 = dX2;
      dY1 = dY2;
    }
    dX2 = dXVertices[0];
    dY2 = dYVertices[0];
    ReturnValue = bsa_Intersect(dX1,dY1,dX2,dY2,dXPoint,dYPoint,&lIntersect);
    if (ReturnValue != bsa_OK)
    {
      *lInside = bsa_FALSE;
      return(ReturnValue);
    }
    if (lIntersect == bsa_TRUE) lInsideFlag = !lInsideFlag;

/*  Now, see if the number of intersections is even. */

    *lInside = bsa_TRUE;
    if (lInsideFlag == 0) *lInside = bsa_FALSE;
    return(bsa_OK);
}

#ifndef bsa_NO_PROTO
static tbsa_Error bsa_Intersect(double         dX1,
                                double         dY1,
                                double         dX2,
                                double         dY2,
                                double         dXPoint,
                                double         dYPoint,
                                unsigned char *lIntersect)
#endif
#ifdef bsa_NO_PROTO
static tbsa_Error bsa_Intersect(dX1,
                                dY1,
                                dX2,
                                dY2,
                                dXPoint,
                                dYPoint,
                                lIntersect)

/*  Routine to determine if an arbitrary line segement defined by
    X1, Y1 to X2, Y2 intersects with a horizontal line extending from
    point XPoint, YPoint to the right to "inifinity".

    This routine does not compute the intersection of the lines.  
    It only determines if they intersect.

    By convention, if the line from X1, Y1 to X2, Y2 is horizontal,
    there is no intersection, even if the line has the same y value
    as the dYpoint.

    Also by convention, we expect the lines to be passed in with
    the vertices ordered as they are in the polygon vertex list.
    Thus, the convention we use is that if the line's first point
    is on the horizontal line emenating from the point being
    investiaged, we have an intersection.  If the line's second
    point is on the horizontal line emenating from the point
    being investigated, we do not have an intersection.
    

    If an intersection is found, lIntersect is set to bsa_TRUE.
    Otherwise, it is set to bsa_FALSE.

    If successful, this routine returns bsa_OK.  Currently, this
    routine does not return any other error codes.
 
    Written by:  Steven A. Walker           Date:  January 3, 1993
    Modified by:                            Date: */

/*  Input Parameters */

    double         dX1;        /* X coord of first point in line */
    double         dY1;        /* & coord of first point in line */
    double         dX2;        /* X coord of second point in line */
    double         dY2;        /* Y coord of second point in line */
    double         dXPoint;    /* X coord of first pt in horizontal line */
    double         dYPoint;    /* X coord of second pt in horizontal line */

/*  Output Parameters */

    unsigned char *lIntersect; /* t/f flag for intersection */

#endif
{
/*  Local Variables */

    double dCompare;    /* result of comparing slopes */
    double dDeltaY;     /* numerator of slope for line dX1,dY1 to dX2,dY2 */

/*  Initialize the intersection to false. */

    *lIntersect = bsa_FALSE;

/*  If the line is horizontal, then by convention there is no
    intersection. */

    if (dY1 == dY2) return(bsa_OK);

/*  We can eleminate cases where the line is completely to the
    left of the point. */

    if ((dX1 < dXPoint) && (dX2 < dXPoint)) return(bsa_OK);

/*  We can also eleminate cases where the line is completely
    above the point. */

    if ((dY1 > dYPoint) && (dY2 > dYPoint)) return(bsa_OK);

/*  Likewise, we can eleminate cases where the line is completely
    below the point. */

    if ((dY1 < dYPoint) && (dY2 <= dYPoint)) return(bsa_OK);

/*  Now, if the line is completely to the right of the point,
    there must be an intersection, because we have eleminated
    cases where the line does not cross the horizontal line
    extending to the right of the point. */

    if ((dX1 >= dXPoint) && (dX2 >= dXPoint)) 
    {
      *lIntersect = bsa_TRUE;
      return(bsa_OK);
    }

/*  If we got this far, then we need to do some more caclulations
    to see if the lines intersect.

    To do this, we simply compare the slopes of two lines.
    The first line is the one with end points at dX1, dY1 and 
    dX2, dY2.  The other is a line created by dXPoint, dYPoint
    and dX2, dY2.

    Note that if 

      (dY2 - dYPoint) / (dX2 - dXPoint) <= (dY2 - dY1) / (dX2 - dX1)

    is true, then there is an intersection.  To simplify, we will
    invert the slopes:

      (dX2 - dXPoint) / (dY2 - dYPoint) >= (dX2 - dX1) / (dY2 - dY1)
      (dX2 - dXPoint) >= (dY2 - dYPoint) * ((dX2 - dX1) / (dY2 - dY1))
      0 >= -(dX2 - dXPoint) + ((dY2 - dYPoint) * ((dX2 - dX1) / (dY2 - dY1)))
      0 <=  (dX2 - dXPoint) - ((dY2 - dYPoint) * ((dX2 - dX1) / (dY2 - dY1)))

    The above illustrates the case where both slopes are positive.
    For the case where both slopes are negative, the inequality
    flips when we multiply both sides by (dY2 - dYPoint).  So,
    the final equation works for all cases.

    Note that since we have already eleminated vertical lines, it
    is not possible to have a divide by zero case.  However, it is
    possible that dY2 and dY1 are very close together.  Therefore,
    we need to be careful that we don't have numerical problems in
    the divide. */

    dDeltaY = dY2 - dY1;
    if ((dDeltaY > 0) && (dDeltaY < bsa_SLOPE_THRESHOLD))
    {
      dDeltaY = bsa_SLOPE_THRESHOLD;
    }
    else if ((dDeltaY < 0) && (dDeltaY > bsa_SLOPE_THRESHOLD))
    {
      dDeltaY = - bsa_SLOPE_THRESHOLD;
    }

    dCompare = (dX2 - dXPoint) - ((dY2 - dYPoint) * ((dX2 - dX1) / dDeltaY));

    if (0 <= dCompare) *lIntersect = bsa_TRUE;

    return(bsa_OK);
}
