#include "ray.h"
#include "material.h"
#include "light.h"

#define MAX(a,b) (a > b ? a : b)
#define M_PI 3.14159265357

typedef list<Light*>::iterator 			liter;
typedef list<Light*>::const_iterator 	cliter;

vec3f getTextureColor(const unsigned char* texture, int width, int height, float u, float v);
vec3f TextureMapping(const unsigned char* texture, int width, int height, const ray& r, const isect& i); // sphere only

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i, const unsigned char * texture, const int w, const int h) const
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
		//printf("light\n");
		vec3f satten = (*it)->shadowAttenuation(r.at(i.t));
		//std::cout << satten << std::endl;
		vec3f atten = (*it)->distanceAttenuation(r.at(i.t))*satten;
		vec3f kdofuse;
		if (texture) {
			// update kd according to texture
			kdofuse = TextureMapping(texture, w, h, r, i);
		}
		else kdofuse = kd;
		vec3f diffuse = kdofuse * MAX(0,(i.N).dot((*it)->getDirection(r.at(i.t))));
		vec3f V = -r.getDirection();
		vec3f R = (2 * ((*it)->getDirection(r.at(i.t)).dot(i.N) * i.N) - (*it)->getDirection(r.at(i.t)));
		vec3f specular = ks * pow(MAX(0,V.dot(R)),shininess*128);
		Id += atten.elementwiseMult((*it)->getColor({0,0,0})).elementwiseMult(diffuse + specular);
	}
	Id.clamp();
	return Id;
}

vec3f getTextureColor(const unsigned char * texture, int width, int height, float u, float v)
{
	if (u < 0 || u > 1 || v < 0 || v > 1) {
		printf("wrong u,v axis\n");
		return vec3f(1, 1, 1);
	}
	if (u == 1) u = 0;
	if (v == 1) v = 0;
	int x = u * width;
	int y = v * height;
	double r = texture[3 * (x + y * width)] / 255.0;
	double g = texture[3 * (x + y * width) + 1] / 255.0;
	double b = texture[3 * (x + y * width) + 2] / 255.0;
	return vec3f(r,g,b);
}

vec3f TextureMapping(const unsigned char* texture, int width, int height, const ray & r, const isect & i)
{
	vec3f Sp(0, 0, 1);
	vec3f Se(1, 0, 0);
	vec3f Sn = (i.N).normalize();
	double cosphi = Sp.dot(Sn);
	double sinphi = sqrt(1 - cosphi * cosphi);
	double phi = acosf(Sp.dot(Sn));
	double v = phi / M_PI;
	double theta = acosf(((Se.dot(Sn))) / sinphi);
	double u = 0;
	if ((Sp.cross(Se)).dot(Sn) > 0) u = theta / 2 / M_PI;
	else u = 1 - theta / 2 / M_PI;
	//printf("phi:%lf v:%lf Se.dot(Sn):%lf sinphi:%lf u:%lf\n", phi, v, Se.dot(Sn), sinphi, u);
	return getTextureColor(texture,width,height,u,v);
}
