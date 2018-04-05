#include <cmath>

#include "light.h"

#define MAX(a,b) (a > b ? a : b)

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
     return vec3f(1,1,1);
	
	vec3f d = getDirection(P);
	ray r(P, d);
	vec3f atten = { 1, 1, 1 };
	vec3f tempP = P;
	isect isec;
	ray tempr(r);
	// recursively find intersection
	while (scene->intersect(tempr, isec)) {
		// printf("test\n");
		// a totally un-transparent object
		if (isec.getMaterial().kt.iszero()) return { 0,0,0 };
		// speed up the while loop when coming to small values
		if (atten[0] < 0.01 && atten[1] < 0.01 && atten[2] < 0.01) return { 0,0,0 };
		tempP = tempr.at(isec.t);
		tempr = ray(tempP, d);
		atten = atten.elementwiseMult(isec.getMaterial().kt);
	}
	return atten;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	//return 1.0;
	double d = P.distanceTo(position);
	//printf("%lf %lf %lf\n", constant_attenuation_coeff, linear_attenuation_coeff, quadratic_attenuation_coeff);
	return MAX(1, 1 / (constant_attenuation_coeff + linear_attenuation_coeff * d + quadratic_attenuation_coeff * d*d));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    // return vec3f(1,1,1);
	double distance = P.distanceTo(position);
	vec3f d = (position - P).normalize();
	ray r(P, d);
	vec3f atten = { 1, 1, 1 };
	vec3f tempP = P;
	isect isec;
	ray tempr(r);
	// recursively find intersection
	while (scene->intersect(tempr, isec)) {
		//printf("intersection\n");
		// intersection is not before light
		if ((distance -= isec.t) < RAY_EPSILON) { return atten; }
		// a totally un-transparent object
		if (isec.getMaterial().kt.iszero()) return { 0,0,0 };
		tempP = tempr.at(isec.t);
		tempr = ray(tempP, d);
		atten = atten.elementwiseMult(isec.getMaterial().kt);
	}
	// std::cout << atten << std::endl;
	return atten;
}
