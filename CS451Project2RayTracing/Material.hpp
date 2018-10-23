//
//  Material.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/20/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp

#include <stdio.h>
#include <vector>

#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.hpp"



// Simple material class, with object color, and headlight shading.
class Material{
public:
    virtual vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity)=0;
    
};


//Diffuse Material
class DiffuseMaterial : public Material{
    vec3 kd;
    vec3 ks;
    float shiness;
public:
    DiffuseMaterial(vec3 kd, vec3 ks, float shiness);
    vec3 getColor(vec3 position, vec3 normal, vec3 viewDir);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
};


//Metal
class Metal : public Material{
    vec3 exCoeff;
    vec3 reIndex;
    vec3 R0;
public:
    Metal(vec3 ex, vec3 re);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    vec3 reflect(vec3 inDir, vec3 normal);
    vec3 R(vec3 dir, vec3 n);
};


//Glass
class Glass : public Material {
    float  refractiveIndex;
    float extinctCoeff;
    float R0;
public:
    Glass(float ex, float re);
    float T(vec3 dir, vec3 n);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    vec3 refract(vec3 inDir, vec3 normal);
    vec3 reflect(vec3 inDir, vec3 normal);
    float R(vec3 dir, vec3 n);
};



//Chess Board Material
class ChessBoard : public Material{
    vec3 kd = vec3(0,0,0);
public:
    ChessBoard(){};
    void makeBoard(vec3 position);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    
};



//Stripe Material
class Stripe : public Material{
    vec3 kd = vec3(1,1,1);
public:
    Stripe(){};
    void makeStripe(vec3 position);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    
};



//Wood
class Wood : public Material{
    float scale;
    float turbulence;
    float period;
    float sharpness;
    vec3 kd;
public:
    Wood();
    void makeWood(vec3 position, vec3 normal, vec3 viewDir);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    float snoise(vec3 r);
};



//Marble
class Marble : public Material{
    float scale;
    float turbulence;
    float period;
    float sharpness;
    vec3 kd;
    vec3 ks;
    float shiness;
public:
    Marble();
    void makeMarble(  vec3 position, vec3 normal, vec3 viewDir);
    vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity);
    float snoise(vec3 r);
};


#endif /* Material_hpp */
