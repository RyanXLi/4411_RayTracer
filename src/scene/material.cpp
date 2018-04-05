#include "ray.h"
#include "material.h"
#include "light.h"

#define MAX(a,b) (a > b ? a : b)

typedef list<Light*>::iterator 			liter;
typedef list<Light*>::const_iterator 	cliter;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	// return kd;

	vec3f Id = ke + ka.elementwiseMult(scene->ambient_light);
	for (cliter it = scene->beginLights(); it != scene->endLights(); ++it) {
		vec3f atten = (*it)->distanceAttenuation(r.at(i.t))*(*it)->shadowAttenuation(r.at(i.t));
		vec3f diffuse = kd * ((i.N).dot((*it)->getDirection(r.at(i.t))));
		vec3f V = -r.getDirection();
		vec3f R = (2 * ((*it)->getDirection(r.at(i.t)).dot(i.N) * i.N) - (*it)->getDirection(r.at(i.t)));
		vec3f specular = ks * pow(MAX(0,V.dot(R)),shininess*128);
		Id += atten.elementwiseMult((*it)->getColor({0,0,0})).elementwiseMult(diffuse + specular);
	}
	Id.clamp();
	return Id;
}
