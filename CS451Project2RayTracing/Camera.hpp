//
//  Camera.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/20/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.h"



class Camera{
    vec3 eye;		// World space camera position.
    vec3 lookAt;	// Center of window in world space.
    vec3 right;		// Vector from window center to window right-mid (in world space).
    vec3 up;		// Vector from window center to window top-mid (in world space).
    
public:
    Camera();
    vec3 getEye();
    void Control();// Compute ray through pixel at normalized device coordinates.
    vec3 rayDirFromNdc(float x, float y);
};

#endif /* Camera_hpp */
