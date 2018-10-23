//
//  LightSource.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#include "LightSource.hpp"


//Point Light---------------------------------------
DirectionalLight::DirectionalLight(vec3 p, vec3 l):p(p),l(l){
}

vec3 DirectionalLight::getPowerDensityAt( vec3 x ){
    return p;
}

vec3 DirectionalLight::getLightDirAt( vec3 x ){
    return l;
}

float DirectionalLight::getDistanceFrom( vec3 x ){
    return MAXFLOAT;
}



//Point Light---------------------------------------
PointLight::PointLight(vec3 p, vec3 l):p(p),l(l){
    
}

vec3 PointLight::getPowerDensityAt( vec3 x ){
    float distance = getDistanceFrom(x);
    float disInv = 1.0/(distance*distance);
    return p*disInv;
}

vec3 PointLight::getLightDirAt( vec3 x ){
    vec3 lightDir = l-x;
    return lightDir.normalize();
}

float PointLight::getDistanceFrom( vec3 x ){
    float distance = (l-x).norm();
    //        float distance = powf((x.x-l.x), 2.0) + powf((x.y-l.y), 2.0) + powf((x.z-l.z), 2.0);
    return distance;
}
