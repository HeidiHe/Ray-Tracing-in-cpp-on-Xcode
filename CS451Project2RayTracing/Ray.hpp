/*
 * File: Scene.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#ifndef Ray_h
#define Ray_h

#include "util.hpp"
#include "Material.hpp"

/* Ray structure. */
class Ray{
public:
    vec3 origin;
    vec3 dir;
    Ray(vec3 o, vec3 d);
};

/* Hit record structure. Contains all data that describes a ray-object intersection point. */ 
class Hit{
public:
    Hit();
    float t;				// Ray paramter at intersection. Negative means no valid intersection.
    vec3 position;			// Intersection coordinates.
    vec3 normal;			// Surface normal at intersection.
    Material* material;		// Material of intersected surface.
};

#endif
