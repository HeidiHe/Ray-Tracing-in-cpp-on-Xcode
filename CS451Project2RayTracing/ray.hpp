//
//  ray.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef ray_hpp
#define ray_hpp

#include <stdio.h>
#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.hpp"

#include "Material.hpp"





// Ray structure.
class Ray{
public:
    vec3 origin;
    vec3 dir;
    Ray(vec3 o, vec3 d);
};

// Hit record structure. Contains all data that describes a ray-object intersection point.

class Hit{
public:
    Hit();
    float t;				// Ray paramter at intersection. Negative means no valid intersection.
    vec3 position;			// Intersection coordinates.
    vec3 normal;			// Surface normal at intersection.
    Material* material;		// Material of intersected surface.
};

#endif /* ray_hpp */
