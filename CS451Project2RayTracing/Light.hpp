/*
 * File: Light.hpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#ifndef LightSource_hpp
#define LightSource_hpp

#include <stdio.h>
#include "util.hpp"

/* abstract LightSource class */
class LightSource {
public:
    virtual vec3 getPowerDensityAt (vec3 x)=0;
    virtual vec3 getLightDirAt     (vec3 x)=0;
    virtual float  getDistanceFrom (vec3 x)=0;
};

/* DirectionalLight class */
class DirectionalLight : public LightSource{
    vec3 p; // power density
    vec3 l; // light direction
public:
    /* constructor of DirectionalLight */
    DirectionalLight(vec3 p, vec3 l);
    
    /* get the power density at a point */
    vec3 getPowerDensityAt( vec3 x );
    
    /* get the light direction at a point */
    vec3 getLightDirAt( vec3 x );
    
    /* get the distance from the light to a point */
    float getDistanceFrom( vec3 x );
};

/* PointLight class */
class PointLight : public LightSource{
    vec3 p; // power density
    vec3 l; // light point position
public:
    /* constructor of PointLight */
    PointLight(vec3 p, vec3 l);
    
    /* get the power density at a point */
    vec3 getPowerDensityAt( vec3 x );
    
    /* get the light direction at a point */
    vec3 getLightDirAt( vec3 x );
    
    /* get the distance from the light to a point */
    float getDistanceFrom( vec3 x );
};

#endif /* LightSource_hpp */
