/*
 * File: vec2.h
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#pragma once
#include <math.h>
#include <stdlib.h>

/* 2D vector class */
class vec2 {
public:
	float x;
	float y;

	vec2() {
		x = 0.0f;
		y = 0.0f;
	}

	vec2(float x, float y):x(x),y(y){}

    /* -------------------- 2D vector operators -------------------- */
	vec2 operator-() const {
		return vec2(-x, -y);
	}

	vec2 operator+(const vec2& operand) const {
		return vec2(x + operand.x, y + operand.y);
	}

	vec2 operator-(const vec2& operand) const {
		return vec2(x - operand.x, y - operand.y);
	}

	vec2 operator*(const vec2& operand) const {
		return vec2(x * operand.x, y * operand.y);
	}
	
	vec2 operator*(float operand) const {
		return vec2(x * operand, y * operand);
	}

	void operator-=(const vec2& a) {
		x -= a.x;
		y -= a.y;
	}

	void operator+=(const vec2& a) {
		x += a.x;
		y += a.y;
	}

	void operator*=(const vec2& a) {
		x *= a.x;
		y *= a.y;
	}

	void operator*=(float a) {
		x *= a;
		y *= a;
	}

    /* get the norm of the 2D vector */
	float norm() {
		return sqrtf(x*x+y*y);
	}

    /* get the 2 norm of the 2D vector  */
	float norm2() {
		return x*x+y*y;
	}

    /* normalizes the 2D vector  */
	vec2 normalize() {
		float oneOverLength = 1.0f / norm();
		x *= oneOverLength;
		y *= oneOverLength;
		return *this;
	}

    /* get a 2D vector with random entries */
	static vec2 random() {
		return vec2(
			((float)rand() / RAND_MAX) * 2 - 1,
			((float)rand() / RAND_MAX) * 2 - 1);
	}
};
