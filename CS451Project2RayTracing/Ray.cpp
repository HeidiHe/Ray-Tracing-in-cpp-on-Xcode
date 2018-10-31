/*
 * File: Ray.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Ray.hpp"

/* Ray structure. */
Ray::Ray(vec3 o, vec3 d) {
    origin = o;
    dir = d;
}

/* Hit record structure. Contains all data that describes a ray-object intersection point. */ 
Hit::Hit() {
    t = -1;
}
