#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	
    vec3f o = r.at(0);
    vec3f d = r.getDirection();
    double txmin, txmax, tymin, tymax, tzmin, tzmax, tmin, tmax;
    
    txmin = (-0.5-o[0]) / d[0];
    txmax = (0.5 - o[0]) / d[0];
    if (txmin > txmax) { swap(txmin, txmax); }

    tmin = txmin;
    tmax = txmax;

    tymin = (-0.5 - o[1]) / d[1];
    tymax = (0.5 - o[1]) / d[1];
    if (tymin > tymax) { swap(tymin, tymax); }

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }

    tmin = max(tmin, tymin);
    tmax = min(tmax, tymax);

    tzmin = (-0.5 - o[2]) / d[2];
    tzmax = (0.5 - o[2]) / d[2];
    if (tzmin > tzmax) { swap(tzmin, tzmax); }

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    tmin = max(tmin, tzmin);
    tmax = min(tmax, tzmax);

    i.obj = this;
    i.t = tmin < 0 ? tmax : tmin;

    double err = 0.0001;

    vec3f isectPoint = r.at(i.t);
    if (fabs(isectPoint[0] - 0.5 <= err)) {
        i.N = { 1,0,0 };
    } 
    else if (fabs(isectPoint[0] + 0.5 <= err)) {
        i.N = { -1,0,0 };
    }
    else if (fabs(isectPoint[1] - 0.5 <= err)) {
        i.N = { 0,1,0 };
    }
    else if (fabs(isectPoint[1] + 0.5 <= err)) {
        i.N = { 0,-1,0 };
    }
    else if (fabs(isectPoint[2] - 0.5 <= err)) {
        i.N = { 0,0,1 };
    }
    else if (fabs(isectPoint[2] + 0.5 <= err)) {
        i.N = { 0,0,-1 };
    }
    return true;
}
