// The main ray tracer.

#include <Fl/fl_ask.h>
#include <random>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "fileio/bitmap.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(traceUI->getThresh(), traceUI->getThresh(), traceUI->getThresh()), traceUI->getDepth() ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
    if (depth < 0 || thresh[0] > 1 || thresh[1] > 1 || thresh[2] > 1) { return{ 0, 0, 0 }; }

	isect i;
    vec3f intensity;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		//printf("%d %d\n", texture_width, texture_height);
		if (texture_switch && texture && !bump_switch) intensity = m.shade(scene, r, i, texture, texture_width, texture_height);
        else if (texture_switch && texture && bump && bump_switch) 
			intensity = m.shade(scene, r, i, texture, texture_width, texture_height, bump);
		else intensity = m.shade(scene, r, i);


        double n_i, n_t;
        bool flipNormal;
        if (r.getDirection().dot(i.N) < 0) { // if entering object
            n_i = 1; // air
            n_t = i.getMaterial().index;
            flipNormal = TRUE;
        }
        else {
            n_i = i.getMaterial().index;
            n_t = 1; // air
            flipNormal = FALSE;
        }

        vec3f reflectDir = reflectDirection(r, i, flipNormal);
        if (traceUI->getGloss() > 0) {
            vec3f perpVec[16];
            perpVec[0] = reflectDir.cross(i.N).normalize();
            perpVec[1] = -perpVec[0];
            perpVec[2] = reflectDir.cross(perpVec[0]).normalize();
            perpVec[3] = -perpVec[2];

            perpVec[4] = (perpVec[0] + perpVec[2]).normalize();
            perpVec[5] = (perpVec[1] + perpVec[3]).normalize();
            perpVec[6] = (perpVec[2] + perpVec[1]).normalize();
            perpVec[7] = (perpVec[3] + perpVec[0]).normalize();

            perpVec[8] = (perpVec[3] + perpVec[7]).normalize();
            perpVec[9] = (perpVec[7] + perpVec[0]).normalize();
            perpVec[10] = (perpVec[0] + perpVec[4]).normalize();
            perpVec[11] = (perpVec[4] + perpVec[2]).normalize();

            perpVec[12] = (perpVec[2] + perpVec[6]).normalize();
            perpVec[13] = (perpVec[6] + perpVec[1]).normalize();
            perpVec[14] = (perpVec[1] + perpVec[5]).normalize();
            perpVec[15] = (perpVec[5] + perpVec[3]).normalize();
            

            //ray mainRay(r.at(i.t) + i.N.normalize() * NORMAL_EPSILON, reflectDir.normalize());
            //intensity += weights[0] * i.getMaterial().kr.elementwiseMult(
            //    traceRay(scene, mainRay, thresh / i.getMaterial().kr, depth - 1));

            //int numTrials = 10;
            double glossFactor = traceUI->getGloss() / 70;
            //printf("%f\n", glossFactor);

            //std::mt19937 rng;
            //rng.seed(std::random_device()());
            //std::uniform_real_distribution<> dist(0.0, 1.0);
            
            //for (int j = 0; j < numTrials; j++) {
            //    double rands[4];
            //    for (int p = 0; p < 4; p++) {
            //        rands[p] = dist(rng);
            //        //printf("rand: %f\n", rands[p]);
            //    }
            //    //printf("\n");
            //    vec3f dir((reflectDir.normalize() 
            //        + glossFactor 
            //            * ((perpVec[0] * rands[0])
            //                + (perpVec[1] * rands[1])
            //                + (perpVec[2] * rands[2])
            //                + (perpVec[3] * rands[3])
            //            ))
            //        .normalize());
            //   // printf("%f\n", dir[0]);
            //   // printf("%f\n", dir[1]);
            //   // printf("%f\n", dir[2]);
            //   // printf("\n");
            //    ray sideRay(r.at(i.t) + i.N.normalize() * NORMAL_EPSILON, 
            //        (reflectDir.normalize() + glossFactor * perpVec[j]).normalize());
            //    intensity += 1/(double)numTrials * i.getMaterial().kr.elementwiseMult(
            //        traceRay(scene, sideRay, thresh / i.getMaterial().kr, depth - 1));
            //}

            for (int j = 0; j < 16; j++) {
                ray sideRay(r.at(i.t) + i.N.normalize() * NORMAL_EPSILON,
                    (reflectDir.normalize() + glossFactor * perpVec[j]).normalize());
                intensity += 1 / 16.0 * i.getMaterial().kr.elementwiseMult(
                    traceRay(scene, sideRay, thresh / i.getMaterial().kr, depth - 1));
            }

        }
        else {
            ray reflectRay(r.at(i.t) + i.N.normalize() * NORMAL_EPSILON, reflectDir.normalize());
            intensity += i.getMaterial().kr.elementwiseMult(
                traceRay(scene, reflectRay, thresh / i.getMaterial().kr, depth - 1));
        }

        
        if (!isTIR(r, i, n_i, n_t)) {
            //printf("refract");
            vec3f retractDir = retractDirection(r, i, n_i, n_t, flipNormal);
            ray retractRay(r.at(i.t), retractDir.normalize());
            intensity += i.getMaterial().kt.elementwiseMult(
                traceRay(scene, retractRay, thresh / i.getMaterial().kt, depth - 1));
        }
        intensity = intensity.clamp();
		return intensity;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		// return vec3f( 0.0, 0.0, 0.0 );

		if (background_switch && background) return getBackgroundColor(scene->getCamera()->xx, scene->getCamera()->yy);
		else return vec3f(0, 0, 0);
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	texture = NULL;
	background = NULL;
	bump = NULL;
	background_width = background_height = 0;
	bump_width = bump_height = 0;
	texture_width = texture_height = 0;
	m_bSceneLoaded = false;
	texture_switch = false;
	background_switch = false;
	bump_switch = false;
}


RayTracer::~RayTracer()
{
	if (buffer) delete [] buffer;
	if (texture) delete [] texture;
	if (background) delete[] background;
	if (bump) delete[] bump;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

bool RayTracer::loadBackground(char * fn)
{
	unsigned char * data = NULL;
	data = readBMP(fn, background_width, background_height);
	if (data) {
		// check
		if (texture != NULL) delete[] background;
		background = data;
	}
	else return false;
	return true;
}

bool RayTracer::loadTexture(char * fn)
{
	unsigned char * data = NULL;
	data = readBMP(fn, texture_width, texture_height);
	if (data) {
		// check
		if (texture != NULL) delete[] texture;
		texture = data;
	}
	else return false;
	return true;
}

bool RayTracer::loadBump(char * fn)
{
	unsigned char * data = NULL;
	data = readBMP(fn, bump_width, bump_height);
	if (texture_width != bump_width || texture_height != bump_height) {
		printf("wrong format bump image\n");
		delete[] data;
		return false;
	}
	if (data) {
		// check
		printf("start\n");
		if (bump != NULL) delete[] bump;
		printf("start1\n");
		//printf("%d %d\n", bump_width, bump_height);
		bump = new unsigned char[bump_width*bump_height];
		for (int i = 0; i < 3 * bump_width * bump_height; i+=3) {
			//if (i > bump_width*bump_height - 1) printf("%d\n", i);
			bump[i/3] = (data[i] + data[i + 1] + data[i + 2]) / 3;
		}
	}
	else return false;
	return true;
}

vec3f RayTracer::getBackgroundColor(double u, double v)
{
	if (u < 0 || u > 1 || v < 0 || v > 1) {
		printf("wrong u,v axis for background\n");
		return vec3f(1, 1, 1);
	}
	if (u == 1) u = 0;
	if (v == 1) v = 0;
	int x = u * background_width;
	int y = v * background_height;
	//printf("%d %d\n", x, y);
	double r = background[3 * (x + y * background_width)] / 255.0;
	double g = background[3 * (x + y * background_width) + 1] / 255.0;
	double b = background[3 * (x + y * background_width) + 2] / 255.0;
	//printf("%lf %lf %lf\n", r, g, b);
	return vec3f(r, g, b);
}

void RayTracer::traceSetup( int w, int h )
{
    // ADDED
    if (traceUI->isRayDist()) {
        if (rayDistTable != nullptr) {
            delete[] rayDistTable;
        }
        rayDistTable = new double[w*h];
        for (int i = 0; i < w*h; i++) {
            rayDistTable[i] = 0.0;
        }
    }
    // ADDED END
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
    vec3f col = { 0, 0, 0 };

	if( !scene )
		return;

    int sampleNum = traceUI->getSampleNum();

    if (sampleNum == 1) {
        double x, y;
        if (!traceUI->isJitter()) {
            x = double(i) / double(buffer_width);
            y = double(j) / double(buffer_height);
        }
        else {
            double x0 = (double(i) - 0.5) / double(buffer_width);
            double y0 = (double(j) - 0.5) / double(buffer_height);
            double dx = 1 / (double(buffer_width));
            double dy = 1 / (double(buffer_height));
            x = x0 + (rand() / (RAND_MAX + 1.)) * dx;
            y = y0 + (rand() / (RAND_MAX + 1.)) * dy;
        }

        col = trace(scene, x, y);

        unsigned char *pixel = buffer + (i + j * buffer_width) * 3;

        pixel[0] = (int)(255.0 * col[0]);
        pixel[1] = (int)(255.0 * col[1]);
        pixel[2] = (int)(255.0 * col[2]);
    }
    else {

        if (!traceUI->isAdaptive()) {

            // normal antialiasing or jitter

            double x0 = (double(i) - 0.5) / double(buffer_width);
            double y0 = (double(j) - 0.5) / double(buffer_height);

            if (!traceUI->isJitter()) {
                // jitter

                double dx = 1 / (double(buffer_width) * (sampleNum - 1));
                double dy = 1 / (double(buffer_height) * (sampleNum - 1));

                for (int p = 0; p < sampleNum; p++) {
                    for (int q = 0; q < sampleNum; q++) {
                        double x = x0 + p * dx;
                        double y = y0 + q * dy;

                        col += trace(scene, x, y);
                    }
                }
            }
            else {
                // normal

                double dx = 1 / (double(buffer_width) * (sampleNum));
                double dy = 1 / (double(buffer_height) * (sampleNum));

                for (int p = 0; p < sampleNum; p++) {
                    for (int q = 0; q < sampleNum; q++) {
                        double x = x0 + (p + (rand() / (RAND_MAX + 1.))) * dx;
                        double y = y0 + (q + (rand() / (RAND_MAX + 1.))) * dy;

                        col += trace(scene, x, y);
                    }
                }
            }

            unsigned char *pixel = buffer + (i + j * buffer_width) * 3;

            pixel[0] = (int)(255.0 * col[0] / sampleNum / sampleNum);
            pixel[1] = (int)(255.0 * col[1] / sampleNum / sampleNum);
            pixel[2] = (int)(255.0 * col[2] / sampleNum / sampleNum);
        }
        else {
            // adaptive

            double adaptThres = 0.1;
            int depth = 1;

            if (sampleNum <= 3) { 
                sampleNum = 3; 
                depth = 1;
            }
            else { 
                sampleNum = 5;
                depth = 2;
            }

            double x, y;
            x = double(i) / double(buffer_width);
            y = double(j) / double(buffer_height);

            if (traceUI->isRayDist()) {
                traceCorners(x, y, 1 / double(buffer_width), 1 / double(buffer_height), depth, adaptThres);
                double val = rayDistTable[(int)(x*buffer_width) + (int)(y*buffer_height)*buffer_width];
                if (val > 0.001) {
                    //printf("idx: %f\n", (int)(x*buffer_width) + (int)(y*buffer_height)*buffer_width);
                    //printf("val: %f\n", val);
                }
                col = { val, val, val };
            }
            else {
                col = traceCorners(x, y, 1 / double(buffer_width), 1 / double(buffer_height), depth, adaptThres);
            }

            unsigned char *pixel = buffer + (i + j * buffer_width) * 3;

            pixel[0] = (int)(255.0 * col[0]);
            pixel[1] = (int)(255.0 * col[1]);
            pixel[2] = (int)(255.0 * col[2]);

        }
    }

	
}

class Point {
public:
    double x;
    double y;
};

vec3f RayTracer::traceCorners(double x, double y, double sideX, double sideY, int depth, double adaptThres) {
    // returns final colors between 0 and 1
    if (depth <= 0) {
        if (traceUI->isRayDist()) {
            rayDistTable[(int)(x*buffer_width) + (int)(y*buffer_height)*buffer_width] = 1;
        }

        return trace(scene, x, y);

    }

    Point corners[4];
    corners[0] = { x - 0.5*sideX, y - 0.5*sideY };
    corners[1] = { x + 0.5*sideX, y - 0.5*sideY };
    corners[2] = { x - 0.5*sideX, y + 0.5*sideY };
    corners[3] = { x + 0.5*sideX, y + 0.5*sideY };

    vec3f centerCol = trace(scene, x, y);
    vec3f col = { 0,0,0 };

    for (int i = 0; i <= 3; i++) {
        vec3f cornerCol = trace(scene, corners[i].x, corners[i].y);

        //if ((256*centerCol).distanceTo(256*cornerCol) > 0.000001) {
        //    printf("%f\n", (256 * centerCol).distanceTo(256 * cornerCol));
        //    printf("%f\n", centerCol[0]);
        //    printf("%f\n", centerCol[1]);
        //    printf("%f\n", centerCol[2]);
        //    printf("%f\n", cornerCol[0]);
        //    printf("%f\n", cornerCol[1]);
        //    printf("%f\n", cornerCol[2]);
        //    printf("\n");
        //}

        if ((centerCol[0] - cornerCol[0] > adaptThres)
            || (centerCol[1] - cornerCol[1] > adaptThres)
            || (centerCol[2] - cornerCol[2] > adaptThres)
            ) {
            //printf("%d", depth);
            if (traceUI->isRayDist() && depth > 1) { 
                rayDistTable[(int)(x*buffer_width) + (int)(y*buffer_height)*buffer_width] = 1 / (depth-1); 
            }
            col += 0.25 * traceCorners((x + corners[i].x) / 2, (y + corners[i].y) / 2, sideX / 2, sideY / 2, depth - 1, adaptThres);
        }
        else {
            
            col += 0.25 * cornerCol;

        }      
    }

    return col;
}


vec3f RayTracer::reflectDirection(ray r, isect i, bool flipNormal) {
    vec3f negD = r.getDirection().normalize(); 
    negD *= -1; // the negitive ray direction

    vec3f normal = i.N.normalize();
    if (!flipNormal) { normal *= -1; }

    return 2 * negD.dot(normal) * normal - negD;
}

vec3f RayTracer::retractDirection(ray r, isect i, double n_i, double n_t, bool flipNormal) {
    vec3f ret(0, 0, 0);
    vec3f n = i.N;
    if (flipNormal) { n *= -1; }
    vec3f v = r.getDirection().normalize();

    for (int i = 0; i < 3; i++) {
        ret[i] = n_i / n_t * (
            (sqrt(pow(n.dot(v), 2)+pow(n_t/n_i , 2)-1)-n.dot(v)) * n[i] + v[i]
            );
    }
    return ret;
}

bool RayTracer::isTIR(ray r, isect i, double n_i, double n_t) {
    return (
        pow(i.N.normalize().dot(r.getDirection().normalize()), 2) <=
        1 - pow(n_t/n_i , 2)
        );
}