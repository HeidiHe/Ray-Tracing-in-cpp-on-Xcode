/*
 * File: Camera.hpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "mathop/vec3.h"
#include "util.h"

/* Camera class */
class Camera{
    vec3 eye;		// World space camera position.
    vec3 lookAt;	// Center of window in world space.
    vec3 right;		// Vector from window center to window right-mid (in world space).
    vec3 up;		// Vector from window center to window top-mid (in world space).
    
public:
    /* constructor of Camera class */
    Camera();
    
    /* get the eye position */
    vec3 getEye();
    
    /* get the ray direction */
    vec3 rayDirFromNdc(float x, float y);
};
