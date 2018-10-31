/*
 * File: Light.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Light.hpp"

/* ------------------ Directional Light ------------------ */
DirectionalLight::DirectionalLight(vec3 p, vec3 l):p(p),l(l){}

/* get the power density at a point */
vec3 DirectionalLight::getPowerDensityAt( vec3 x ){ return p; }

/* get the light direction at a point */
vec3 DirectionalLight::getLightDirAt( vec3 x ){ return l; }

/* get the distance from the light to a point */
float DirectionalLight::getDistanceFrom( vec3 x ){ return MAXFLOAT; }


/* ------------------ Point Light ------------------ */
PointLight::PointLight(vec3 p, vec3 l):p(p),l(l){}

/* get the power density at a point */
vec3 PointLight::getPowerDensityAt( vec3 x ){
    float distance = getDistanceFrom(x);
    float disInv = 1.0/(distance*distance);
    return p*disInv;
}

/* get the light direction at a point */
vec3 PointLight::getLightDirAt( vec3 x ){
    vec3 lightDir = l-x;
    return lightDir.normalize();
}

/* get the distance from the light to a point */
float PointLight::getDistanceFrom( vec3 x ){
    float distance = (l-x).norm();
    return distance;
}
