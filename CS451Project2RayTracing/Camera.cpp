/*
 * File: Camera.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Camera.hpp"

/* Camera Class */
Camera::Camera() {
    eye = vec3(-3, -0.3,1.3);
    lookAt = vec3(-3, -0.3, 0);
    right = vec3(1, 0, 0);
    up = vec3(0, 1, 0);
}

/* get the eye position */
vec3 Camera::getEye() { return eye; }

/* get the ray direction */
vec3 Camera::rayDirFromNdc(float x, float y) {
    return (lookAt - eye + right * x + up* y).normalize();
}
