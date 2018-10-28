



#include "Camera.hpp"


Camera::Camera(){
    eye = vec3(-3, -0.3,1.3);
    lookAt = vec3(-3, -0.3, 0);
//    eye = vec3(-3, -0.1,-3);
//    lookAt = vec3(-4, -0.1, 0);
    right = vec3(1, 0, 0);
    up = vec3(0, 1, 0);
}
vec3 Camera::getEye(){
    return eye;
}

    
vec3 Camera::rayDirFromNdc(float x, float y) {
    return (lookAt - eye + right * x + up* y).normalize();
}
