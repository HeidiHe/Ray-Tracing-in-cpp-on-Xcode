/*
 * File: Material.hpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#ifndef Material_hpp
#define Material_hpp

#include "util.hpp"

/* abstract Material class with object color, and headlight shading. */
class Material{
public:
    virtual vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity)=0;
};


/* --------- Diffuse Material --------- */
class DiffuseMaterial : public Material{
    vec3 kd;
    vec3 ks;
    float shiness;
public:
    /* constructor of DiffuseMaterial class */
    DiffuseMaterial(vec3 kd, vec3 ks, float shiness);
    
    /* get the color at a point, with a specified surface normal and view direction */
    vec3 getColor(vec3 position, vec3 normal, vec3 viewDir);
    
    /* the shade with specified surface normal, view direction, light direction, and power density */
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
};


/* --------- Metal class --------- */
class Metal : public Material{
    vec3 exCoeff;
    vec3 reIndex;
    vec3 R0;
public:
    /* constructor of Metal */
    Metal(vec3 ex, vec3 re);
    
    /* the shade with specified surface normal, view direction, light direction, and power density */
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    
    /* reflection on Metal material */
    vec3 reflect(vec3 inDir, vec3 normal);
    
    /* rotate */
    vec3 R(vec3 dir, vec3 n);
};


/* --------- Glass class --------- */
class Glass : public Material {
    float  refractiveIndex;
    float extinctCoeff;
    float R0;
public:
    /* constructor of Glass */
    Glass(float ex, float re);
    
    /* translate */
    float T(vec3 dir, vec3 n);

    /* the shade with specified surface normal, view direction, light direction, and power density */
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    
    /* refraction */
    vec3 refract(vec3 inDir, vec3 normal);
    
    /* reflection */
    vec3 reflect(vec3 inDir, vec3 normal);
    
    /* rotate */
    float R(vec3 dir, vec3 n);
};


/* --------- Stripe Material --------- */
class Stripe : public Material{
    vec3 kd = vec3(1,1,1);
public:
    /* constructor of Stripe */
    Stripe(){};
    
    /* make Stripe material */
    void makeStripe(vec3 position);
    
    /* the shade with specified surface normal, view direction, light direction, and power density */
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
};


/* --------- Wood --------- */
class Wood : public Material{
    float scale;
    float turbulence;
    float period;
    float sharpness;
    vec3 kd;
public:
    /* constructor of Wood */
    Wood();
    
    /* make Wood material */
    void makeWood(vec3 position, vec3 normal, vec3 viewDir);
    
    /* the shade with specified surface normal, view direction, light direction, and power density */
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    
    /* add noise */
    float snoise(vec3 r);
};

#endif /* Material_hpp */
