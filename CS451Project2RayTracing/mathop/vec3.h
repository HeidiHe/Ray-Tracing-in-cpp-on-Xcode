/*
 * File: vec3.h
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#pragma once
#include <math.h>
#include <stdlib.h>

/* 3D vector class */
class vec3 {
public:
	float x;
	float y;
	float z;

	vec3() {
		x = 0;
		y = 0;
		z = 0;
	}
    
    vec3(float x, float y, float z):x(x),y(y),z(z){}

    /* get a 3D vector with random entries */
	static vec3 random() {
		return vec3(
			((float)rand() / RAND_MAX),
			((float)rand() / RAND_MAX),
			((float)rand() / RAND_MAX));
	}

    /* -------------------- 3D vector operators -------------------- */
	vec3 operator-() const {
		return vec3(-x, -y, -z);
	}

	vec3 operator+(const vec3& operand) const {
		return vec3(x + operand.x, y + operand.y, z + operand.z);
	}

	vec3 operator-(const vec3& operand) const {
		return vec3(x - operand.x, y - operand.y, z - operand.z);
	}

	vec3 operator*(const vec3& operand) const {
		return vec3(x * operand.x, y * operand.y, z * operand.z);
	}

	vec3 operator/(const vec3& operand) const {
		return vec3(x / operand.x, y / operand.y, z / operand.z);
	}

	vec3 operator*(float operand) const {
		return vec3(x * operand, y * operand, z * operand);
	}

	void operator-=(const vec3& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	void operator+=(const vec3& a) {
		x += a.x;
		y += a.y;
		z += a.z;
	}

	void operator*=(const vec3& a) {
		x *= a.x;
		y *= a.y;
		z *= a.z;
	}

	void operator*=(float a) {
		x *= a;
		y *= a;
		z *= a;
	}

    /* get the norm of the 3D vector */
	float norm() const {
		return sqrtf(x*x+y*y+z*z);
	}

    /* get the 2 normal of the 3D vector */
	float norm2() const {
		return x*x+y*y+z*z;
	}

    /* normalize the 3D vector */
	vec3 normalize() {
		float oneOverLength = 1.0f / norm();
		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
		return *this;
	}
	
    /* get the cross product of this 3D vector and another 3D vector */
	vec3 cross(const vec3& operand) const {
		return vec3(
			y * operand.z - z * operand.y,
			z * operand.x - x * operand.z,
			x * operand.y - y * operand.x);
	}

    /* get the dot product of this 3D vector and another 3D vector */
	float dot(const vec3& operand) const {
		return x * operand.x + y * operand.y + z * operand.z;
	}
};
