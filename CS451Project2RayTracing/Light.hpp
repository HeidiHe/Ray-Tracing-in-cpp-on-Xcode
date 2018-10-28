//
//  LightSource.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef LightSource_hpp
#define LightSource_hpp

#include <stdio.h>
#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.hpp"

class LightSource {
public:
    virtual vec3 getPowerDensityAt (vec3 x)=0;
    virtual vec3 getLightDirAt     (vec3 x)=0;
    virtual float  getDistanceFrom (vec3 x)=0;
};

class DirectionalLight : public LightSource{
    vec3 p;//power density
    vec3 l;//light direction
public:
    DirectionalLight(vec3 p, vec3 l);
    vec3 getPowerDensityAt( vec3 x );
    vec3 getLightDirAt( vec3 x );
    float getDistanceFrom( vec3 x );
};

class PointLight : public LightSource{
    vec3 p;//power density
    vec3 l;//light point position
public:
    PointLight(vec3 p, vec3 l);
    vec3 getPowerDensityAt( vec3 x );
    vec3 getLightDirAt( vec3 x );
    float getDistanceFrom( vec3 x );
};

#endif /* LightSource_hpp */
