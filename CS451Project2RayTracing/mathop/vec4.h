/*
 * File: vec4.h
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
s */

#pragma once
#include <math.h>
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

/* 4D vector class */
class vec4 {
public:
	union{
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		
		float v[4];
	};

	vec4():x(0.0f),y(0.0f),z(0.0f),w(0.0f){}

	vec4(float f):x(f),y(f),z(f),w(f){}

	vec4(vec3 f3):x(f3.x),y(f3.y),z(f3.z),w(1.0f){}

	vec4(float x, float y, float z, float w):x(x),y(y),z(z),w(w){}

    /* -------------------- 4D vector operators -------------------- */
	vec4& operator+=(const vec4& o) {
		x += o.x;
		y += o.y;
		z += o.z;
		w += o.w;
		return *this;
	}

	vec4& operator-=(const vec4& o) {
		x -= o.x;
		y -= o.y;
		z -= o.z;
		w -= o.w;
		return *this;
	}

	vec4& operator*=(const vec4& o) {
		x *= o.x;
		y *= o.y;
		z *= o.z;
		w *= o.w;
		return *this;
	}

	vec4& operator/=(const vec4& o) {
		x /= o.x;
		y /= o.y;
		z /= o.z;
		w /= o.w;
		return *this;
	}

	vec4& operator%=(const vec4& o) {
		x = fmodf(x, o.x);
		y = fmodf(y, o.y);
		z = fmodf(z, o.z);
		w = fmodf(w, o.w);
		return *this;
	}

	vec4 operator+(const vec4& o) const {
		return vec4(x + o.x, y + o.y, z + o.z, w + o.w);
	}

	vec4 operator-(const vec4& o) const {
		return vec4(x - o.x, y - o.y, z - o.z, w - o.w);
	}

	vec4 operator*(const vec4& o) const {
		return vec4(x * o.x, y * o.y, z * o.z, w * o.w);
	}

	vec4 operator/(const vec4& o) const {
		return vec4(x / o.x, y / o.y, z / o.z, w / o.w);
	}

	vec4 operator%(const vec4& o) const {
		return vec4(fmodf(x, o.x), fmodf(y, o.y), fmodf(z, o.z), fmodf(w, o.w));
	}


	vec4 operator+() const {
		return vec4(+x, +y, +z, +w);
	}

	vec4 operator-() const {
		return vec4(-x, -y, -z, -w);
	}

	vec4 operator!() const {
		return vec4(-x, -y, -z, +w);
	}

    /* get the distance between this 4D vector and another 4D vector */
	float distance(const vec4& o) const {
		return (*this - o).norm();
	}

    /* get the dot product of this 4D vector and another 4D vector */
	float dot(const vec4& o) const {
		return x * o.x + y * o.y + z * o.z + w * o.w;
	}

    /* get the norm of the 4D vector */
	float norm() const {
		return sqrtf( this->dot(*this));
	}

    /* get the 2 norm of the 4D vector */
	float norm2() const {
		return this->dot(*this);
	}

    /* normalize the 4D vector */
	vec4 normalize() const {
		return *this / norm();
	}
};
