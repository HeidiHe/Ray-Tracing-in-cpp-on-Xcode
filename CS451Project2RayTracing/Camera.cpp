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

    // Compute ray through pixel at normalized device coordinates.
    void Camera::Control(){
//        if(keyboardState['d']) eye.x+=0.1;//right
//        if(keyboardState['a']) eye.x-=0.1;//left
//        if(keyboardState['w']) {eye.z-=0.1; printf("moving forward");}//foward
//        if(keyboardState['s']) {eye.z+=0.1;}//back
//        if(keyboardState['x']) {eye.y+=0.1;}//up
//        if(keyboardState['z']) {eye.y-=0.1;}//down
    }
    
vec3 Camera::rayDirFromNdc(float x, float y) {
    return (lookAt - eye + right * x + up* y).normalize();
}
