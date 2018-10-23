//
//  Camera.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#include "Camera.hpp"


Camera::Camera(){
    eye = vec3(-3.5, 0, 1);
    lookAt = vec3(-3.5, 0, 0);
    right = vec3(1, 0, 0);
    up = vec3(0, 1, 0);
}
vec3 Camera::getEye(){
    return eye;
}

    
vec3 Camera::rayDirFromNdc(float x, float y) {
    return (lookAt - eye + right * x + up* y).normalize();
}
