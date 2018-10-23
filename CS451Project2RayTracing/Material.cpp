//
//  Material.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/20/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#include "Material.hpp"
#include <vector>




//Diffuse Material Class------------------------------------------
DiffuseMaterial::DiffuseMaterial(vec3 _kd, vec3 _ks=vec3(0,0,0), float _shiness=1.0){
    kd = _kd;
    ks = _ks;
    shiness = _shiness;
}

vec3 DiffuseMaterial::getColor(vec3 position, vec3 normal, vec3 viewDir){
    return kd;
}

vec3 DiffuseMaterial::shade( vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
        
    float cosTheta = normal.dot(lightDir);
    
    vec3 halfway = (viewDir + lightDir).normalize();
    
    vec3 L = kd*powerDensity*fmax(0.0, cosTheta)+ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    
    return L;
}



//Metal------------------------------------------
Metal::Metal(vec3 ex, vec3 re){
        exCoeff = ex;
        reIndex = re;
        R0 = ((re-vec3(1,1,1))*(re-vec3(1,1,1))+ex*ex)/
        ((re+vec3(1,1,1))*(re+vec3(1,1,1))+ex*ex);
}


vec3 Metal::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
    return vec3(0,0,0);
}
    
vec3 Metal::reflect( vec3 inDir, vec3 normal){
    vec3 perp = normal * normal.dot(inDir);
    vec3 parallel = inDir - perp;
    return parallel - perp;
}

    
vec3 Metal::R(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    return R0 + (vec3(1,1,1)-R0)*pow(1-cos(alpha), 5);
}




//Glass------------------------------------------
Glass::Glass(float ex, float re){
        extinctCoeff = ex;
        refractiveIndex = re;
        R0 = powf(refractiveIndex-1.0, 2.0)+extinctCoeff*extinctCoeff
        /powf(refractiveIndex+1.0, 2.0)+extinctCoeff*extinctCoeff;
}

float Glass::T(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    float R = R0 + (1-R0)*pow(1-cos(alpha), 5);
    return 1-R;
}
    
vec3 Glass::shade(vec3 normal,
           vec3 viewDir,
           vec3 lightDir,
           vec3 powerDensity){
    return vec3(0,0,0);
}
    
    
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
    
vec3 Glass::reflect(  	vec3 inDir,
             vec3 normal){
    vec3 perp = normal * normal.dot(inDir);
    vec3 parallel = inDir - perp;
    return parallel - perp;
}

float Glass::R(vec3 dir, vec3 n) {
    float alpha = dir.dot(n);
    return (R0 + (1-R0)*powf(1-cos(alpha), 5));
    //        float R = (R0 + (1-R0)*powf(1-cos(alpha), 5));
    //        return 2*R*0.2 / (1+R);
}





//Chess Board------------------------------------------
void ChessBoard::makeBoard(vec3 position){
    if(position.x<0 && position.x>-8 && position.z<0 && position.z>-8){
        if(fmod((int)position.x,2)==fmod((int)position.z,2)){
            kd = vec3(0,0,0);
        }else{
            kd = vec3(1,1,1);
        }
    }else{
        kd = vec3(0.72, 0.72, 0.72);
    }
}

vec3 ChessBoard::shade(vec3 normal,
           vec3 viewDir,
           vec3 lightDir,
           vec3 powerDensity){
    float cosTheta = normal.dot(lightDir);
    
    vec3 halfway = (viewDir + lightDir).normalize();
    
    vec3 L = kd*powerDensity*fmax(0.0, cosTheta);
    
    //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    
    return L;
    
}



//Stripe------------------------------------------
void Stripe::makeStripe(vec3 position){
        
        if(fmod((int)(position.x*10.0),2)==0){
            kd = vec3(0,0,0);
        }else{
            kd = vec3(1,1,1);
        }
        
        
}

vec3 Stripe::shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        float cosTheta = normal.dot(lightDir);
        
        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta)
        + vec3(1,1,1)*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),0.5);
        //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
        
        return L;
        
}


//Wood------------------------------------------
Wood::Wood(){
    scale = 16;
    turbulence = 500;
    period = 8;
    sharpness = 10;
    kd = vec3(0.10,0.98,0.26);
}

void Wood::makeWood(  vec3 position, vec3 normal, vec3 viewDir){
    //return normal;
    float w = position.x * period + pow(snoise(position * scale), sharpness)*turbulence + 10000.0;
    w -= int(w);
    //light wood vec3(0.7,0.6,0.3)
    kd= (vec3(0.5,0.3,0) * w + vec3(0.3, 0.18, 0.05) * (1-w)) * normal.dot(viewDir);
}


vec3 Wood::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
    
    float cosTheta = normal.dot(lightDir);
    
    vec3 halfway = (viewDir + lightDir).normalize();
    
    vec3 L = kd*powerDensity*fmax(0.0, cosTheta)
    +vec3(0.3,0.3,0.3)*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),0.2);
    //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    
    return L;
}

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



//Marble------------------------------------------
Marble::Marble(){
        scale = 32;
        turbulence = 50;
        period = 32;
        sharpness = 1;
        kd = vec3(0.96, 0.96, 0.96);
        ks = vec3(1,1,1);
        shiness = 1;
}


void Marble::makeMarble(  vec3 position, vec3 normal, vec3 viewDir){
    float w = position.x * period + pow(snoise(position * scale), sharpness)*turbulence;
    w = pow(sin(w)*0.5+0.5, 4);
    kd = (vec3(0.96, 0.96, 0.96) * w + vec3(0.72, 0.72, 0.72) * (1-w)) * normal.dot(viewDir);
}


vec3 Marble::shade(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 powerDensity){
    
    float cosTheta = normal.dot(lightDir);
    
    vec3 halfway = (viewDir + lightDir).normalize();
    
    vec3 L = kd*powerDensity*fmax(0.0, cosTheta)+ ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
    
    return L;
}


float Marble::snoise(vec3 r) {
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
