#include <cmath>

#include "light.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
#define M_PI 3.1415926

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    // return vec3f(1,1,1);
	
	vec3f d = getDirection(P);
	vec3f atten = {1, 1, 1};
	vec3f tempP = P;
	isect isec;
	ray tempr = ray(P,d);
	// recursively find intersection
	while (scene->intersect(tempr, isec)) {
		// printf("test\n");
		// a totally un-transparent object
		if (isec.getMaterial().kt.iszero()) return vec3f(0,0,0);
		// speed up the while loop when coming to small values
		if (atten[0] < 0.004 && atten[1] < 0.004 && atten[2] < 0.004) { printf("test\n"); return vec3f(0, 0, 0); }
		tempP = tempr.at(isec.t);
		tempr = ray(tempP, d);
		atten = atten.elementwiseMult(isec.getMaterial().kt);
	}
	return atten;
	
	//vec3f d = getDirection(P);

	//// loop to get the attenuation
	//vec3f curP = P;
	//isect isecP;
	//vec3f ret = getColor(P);
	//ray r = ray(curP, d);
	//while (scene->intersect(r, isecP))
	//{
	//	//if not transparent return black
	//	if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0);
	//	if (ret[0] < 0.004 && ret[1] < 0.004 && ret[2] < 0.004) { printf("test\n"); return vec3f(0, 0, 0); }
	//	//use current intersection point as new light source
	//	curP = r.at(isecP.t);
	//	r = ray(curP, d);
	//	ret = prod(ret, isecP.getMaterial().kt);
	//}
	//return ret;
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
	// return 1.0;
	double d = P.distanceTo(position);
	//printf("%lf %lf %lf\n", constant_attenuation_coeff, linear_attenuation_coeff, quadratic_attenuation_coeff);
	//printf("%lf\d", result);
	return MIN(1, 1.0 / (constant_attenuation_coeff + linear_attenuation_coeff * d + quadratic_attenuation_coeff * d*d));
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
	vec3f atten = {1, 1, 1};
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

vec3f SpotLight::shadowAttenuation(const vec3f & P) const
{
	vec3f L = (P - position).normalize(); //actually -L here
	double coslambda = MAX(0, L.dot(orientation)); double boundary = cos(coneangle*M_PI/180.0);
	// outside the focus of the spotlight, no shadow cast at all
	if (coslambda < boundary) return vec3f(1, 1, 1);
	//printf("inside\n");
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
	return vec3f();
}

double SpotLight::distanceAttenuation(const vec3f & P) const
{
	// distance attenuation here contains the Warn Model as well
	double d = P.distanceTo(position);
	double distance_atten = MIN(1, 1.0 / (constant_attenuation_coeff + linear_attenuation_coeff * d + quadratic_attenuation_coeff * d*d));
	vec3f L = (P - position).normalize(); //actually -L here
	double warn_atten = 1.0;
	double coslambda = MAX(0, L.dot(orientation)); double boundary = cos(coneangle*M_PI/180.0);
	if (coslambda < boundary) {
		// the particle is outside boundary
		//printf("outside\n");
		warn_atten = 0.0;
	}
	else {
		//printf("inside: %lf %lf\n", coslambda, boundary);
		warn_atten = pow(coslambda, focus_constant);
	}
	return distance_atten * warn_atten;
}

vec3f SpotLight::getColor(const vec3f & P) const
{
	return color;
}

vec3f SpotLight::getDirection(const vec3f & P) const
{
	return (position - P).normalize();
}

void SpotLight::changeAttenuationCoeff(double a, double b, double c)
{
	constant_attenuation_coeff = a;
	linear_attenuation_coeff = b;
	quadratic_attenuation_coeff = c;
}

void SpotLight::changeFocusConstraint(double p)
{
	focus_constant = p;
}
