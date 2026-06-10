/*  Test program to test the TAMPS Strip Chart Generation
    Utility routine to see if a point is inside a polygon.

    Written by:  Steven A. Walker         Date:  January 3, 1993
    Modified by:                          Date: */

#include <stdio.h>
#include <math.h>

#include "bsa_Main.h"
#include "bsa_Error.h"

/*  Define PI */

#define bsa_PI 3.14159

main()
{
/*  Local Variables */

    tbsa_Error    ReturnValue;
    long          kNumberOfVertices = 1024;
    long          k1;
    double        dTheta;
    double        dDeltaTheta;
    double        dXPoint;
    double        dYPoint;
    double        dXVertices[kNumberOfVertices];
    double        dYVertices[kNumberOfVertices];
    unsigned char lInside;



/*  Test Cases 1, 2, and 3:  Test with a circle. */

    dTheta = 0;
    dDeltaTheta = (2.0 * bsa_PI) / (double) kNumberOfVertices;
    for (k1 = 0; k1 < kNumberOfVertices; k1++)
    {
      dXVertices[k1] = cos(dTheta);
      dYVertices[k1] = sin(dTheta);
      dTheta += dDeltaTheta;
    }

/*  Test Case 1A.  Point at center */

    dXPoint = 0.0;
    dYPoint = 0.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 1A -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Cases 2:  Points inside the circle */

/*  Test Case 2A.  Point at 3 oclock */

    dTheta = 0.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2A -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2B.  Point at 1:30 oclock */

    dTheta = 1.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2B -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2C.  Point at 12 oclock */

    dTheta = 2.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2C -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2D.  Point at 10:30 oclock */

    dTheta = 3.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2D -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2E.  Point at 9 oclock */

    dTheta = 4.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2E -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2F.  Point at 7:30 oclock */

    dTheta = 5.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2F -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2G.  Point at 6 oclock */

    dTheta = 6.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2G -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 2H.  Point at 4:30 oclock */

    dTheta = 7.0 * (bsa_PI/4.0);
    dXPoint = 0.5 * cos(dTheta);
    dYPoint = 0.5 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 2H -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Cases 3:  Points outside the circle */

/*  Test Case 3A.  Point at 3 oclock */

    dTheta = 0.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3A -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3B.  Point at 1:30 oclock */

    dTheta = 1.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3B -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3C.  Point at 12 oclock */

    dTheta = 2.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3C -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3D.  Point at 10:30 oclock */

    dTheta = 3.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3D -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3E.  Point at 9 oclock */

    dTheta = 4.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3E -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3F.  Point at 7:30 oclock */

    dTheta = 5.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3F -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3G.  Point at 6 oclock */

    dTheta = 6.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3G -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 3H.  Point at 4:30 oclock */

    dTheta = 7.0 * (bsa_PI/4.0);
    dXPoint = 2.0 * cos(dTheta);
    dYPoint = 2.0 * sin(dTheta);
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 3H -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Cases 4 and 5:  Test with a concave polygon. */

    dXVertices[0] = 10.0;
    dYVertices[0] = 10.0;
    dXVertices[1] = 11.0;
    dYVertices[1] =  5.0;
    dXVertices[2] = 12.0;
    dYVertices[2] =  7.0;
    dXVertices[3] = 13.0;
    dYVertices[3] =  5.0;
    dXVertices[4] = 14.0;
    dYVertices[4] =  7.0;
    dXVertices[5] = 15.0;
    dYVertices[5] =  5.0;
    dXVertices[6] = 16.0;
    dYVertices[6] =  7.0;
    dXVertices[7] = 17.0;
    dYVertices[7] =  5.0;
    dXVertices[8] = 18.0;
    dYVertices[8] =  7.0;
    dXVertices[9] = 19.0;
    dYVertices[9] =  5.0;
    dXVertices[10] = 20.0;
    dYVertices[10] = 10.0;
    kNumberOfVertices = 11;

/*  Test Case 4:   Points inside teeth. */

/*  Test Case 4A. */

    dXPoint = 11.0;
    dYPoint =  6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 4A -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 4B. */

    dXPoint = 11.0;
    dYPoint =  6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 4B -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 4C. */

    dXPoint = 15.0;
    dYPoint =  6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 4C -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 4D. */

    dXPoint = 17.0;
    dYPoint =  6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 4D -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 4E. */

    dXPoint = 19.0;
    dYPoint =  6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 4E -- Expected Inside.   Got ");
    if (lInside == bsa_TRUE)  printf("Inside.\n");
    if (lInside == bsa_FALSE) printf("Outside.  ***TEST FAILED***\n");

/*  Test Case 5:  Points outside polygon (between teeth. */

/*  Test Case 5A. */

    dXPoint = 10.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5A -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 5B. */

    dXPoint = 12.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5B -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 5C. */

    dXPoint = 14.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5C -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 5D. */

    dXPoint = 16.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5D -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 5E. */

    dXPoint = 18.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5E -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");

/*  Test Case 5F. */

    dXPoint = 20.0;
    dYPoint = 6.0;
    ReturnValue = bsa_IsPointInPolygon(dXPoint,dYPoint,
                                       kNumberOfVertices,
                                       dXVertices,
                                       dYVertices,
                                      &lInside);
    if (ReturnValue != bsa_OK)
    {
      printf("Test1: Error returned from bsa_IsPointInPolygon\n");
      exit(1);
    }
    printf("Test1:  Test Case 5F -- Expected Outside.  Got ");
    if (lInside == bsa_TRUE)  printf("Inside.   ***TEST FAILED***\n");
    if (lInside == bsa_FALSE) printf("Outside.\n");
}
