//
//  ray.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#include "ray.hpp"

// Ray structure.
Ray::Ray(vec3 o, vec3 d){
    origin = o;
    dir = d;
}


// Hit record structure. Contains all data that describes a ray-object intersection point.
Hit::Hit(){
    t = -1;
}

