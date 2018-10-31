/*
 * File: Material.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Material.hpp"

/* --------- Diffuse Material --------- */
DiffuseMaterial::DiffuseMaterial(vec3 _kd, vec3 _ks=vec3(0,0,0), float _shiness=1.0){
    kd = _kd;
    ks = _ks;
    shiness = _shiness;
}

/* get the color at a point, with a specified surface normal and view direction */
vec3 DiffuseMaterial::getColor(vec3 position, vec3 normal, vec3 viewDir){
    return kd;
}

/* the shade with specified surface normal, view direction, light direction, and power density */
vec3 DiffuseMaterial::shade( vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
    float cosTheta = normal.dot(lightDir);
    
    vec3 halfway = (viewDir + lightDir).normalize();
    vec3 L = kd*powerDensity*fmax(0.0, cosTheta)+ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    
    return L;
}


/* --------- Metal --------- */
Metal::Metal(vec3 ex, vec3 re){
    exCoeff = ex;
    reIndex = re;
    R0 = ((re-vec3(1,1,1)) * (re-vec3(1,1,1)) + ex * ex)/((re + vec3(1,1,1)) * (re+vec3(1,1,1)) + ex * ex);
}

/* the shade with specified surface normal, view direction, light direction, and power density */
vec3 Metal::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity) {
    return vec3(0,0,0);
}

/* reflection on Metal material */
vec3 Metal::reflect( vec3 inDir, vec3 normal) {
    vec3 perp = normal * normal.dot(inDir);
    vec3 parallel = inDir - perp;
    return parallel - perp;
}

/* rotate */
vec3 Metal::R(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    return R0 + (vec3(1,1,1)-R0) * pow(1-cos(alpha), 5);
}


/* --------- Glass --------- */
Glass::Glass(float ex, float re){
        extinctCoeff = ex;
        refractiveIndex = re;
        R0 = powf(refractiveIndex-1.0, 2.0) + extinctCoeff * extinctCoeff
        /powf(refractiveIndex+1.0, 2.0) + extinctCoeff * extinctCoeff;
}

/* translate */
float Glass::T(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    float R = R0 + (1-R0) * pow(1-cos(alpha), 5);
    return 1-R;
}

/* the shade with specified surface normal, view direction, light direction, and power density */
vec3 Glass::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
    return vec3(0,0,0);
}

/* refraction */
vec3 Glass::refract(vec3 inDir, vec3 normal) {
    //vec3 perp = -normal * normal.dot(inDir);
    vec3 perp = -normal * normal.dot(inDir);
    vec3 parallel = inDir - perp;
    
    float ri = refractiveIndex;
    float cosa = -normal.dot(inDir);
    if (cosa < 0) { cosa = -cosa; normal = -normal; ri = 1/ri; }
    float disc = 1 - (1 - cosa * cosa) / ri / ri;
    float cosb = (disc < 0)?0:sqrt(disc);
    return parallel*(1.0/ri) - normal * cosb;
}

/* reflection */
vec3 Glass::reflect( vec3 inDir, vec3 normal) {
    vec3 perp = normal * normal.dot(inDir);
    vec3 parallel = inDir - perp;
    return parallel - perp;
}

/* rotate */
float Glass::R(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    return (R0 + (1-R0)*powf(1-cos(alpha), 5));
}


/* --------- Stripe --------- */
void Stripe::makeStripe(vec3 position) {
        if(fmod((int)(position.x*10.0),2) == 0) {
            kd = vec3(0,0,0);
        }
        else{
            kd = vec3(1,1,1);
        }
}

/* the shade with specified surface normal, view direction, light direction, and power density */
vec3 Stripe::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity) {
        float cosTheta = normal.dot(lightDir);
        vec3 halfway = (viewDir + lightDir).normalize();
        vec3 L = kd * powerDensity*fmax(0.0, cosTheta)
                    + vec3(1,1,1)
                    * powerDensity
                    * pow((double)fmax(0.0, normal.dot(halfway)),0.5);
        return L;
}


/* --------- Wood --------- */
Wood::Wood(){
    scale = 16;
    turbulence = 500;
    period = 8;
    sharpness = 10;
    kd = vec3(0.10,0.98,0.26);
}

/* make Wood material */
void Wood::makeWood(  vec3 position, vec3 normal, vec3 viewDir) {
    float w = position.x * period + pow(snoise(position * scale), sharpness)*turbulence + 10000.0;
    w -= int(w);
    kd= (vec3(0.5,0.3,0) * w + vec3(0.3, 0.18, 0.05) * (1-w)) * normal.dot(viewDir);
}

/* the shade with specified surface normal, view direction, light direction, and power density */
vec3 Wood::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity) {
    float cosTheta = normal.dot(lightDir);
    vec3 halfway = (viewDir + lightDir).normalize();
    vec3 L = kd * powerDensity
                * fmax(0.0, cosTheta)
                + vec3(0.3,0.3,0.3)
                * powerDensity
                * pow((double)fmax(0.0, normal.dot(halfway)),0.2);
    return L;
}

/* add noise */
float Wood::snoise(vec3 r) {
    unsigned int x = 0x0625DF73;
    unsigned int y = 0xD1B84B45;
    unsigned int z = 0x152AD8D0;
    float f = 0;
    for(int i=0; i<32; i++) {
        vec3 s(	x/(float)0xffffffff,
               y/(float)0xffffffff,
               z/(float)0xffffffff);
        f += sin(s.dot(r));
        x = x << 1 | x >> 31;
        y = y << 1 | y >> 31;
        z = z << 1 | z >> 31;
    }
    return f / 64.0 + 0.5;
}
