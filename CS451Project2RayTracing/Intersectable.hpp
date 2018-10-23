//
//  Intersectable.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef Intersectable_hpp
#define Intersectable_hpp

#include <stdio.h>
#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.hpp"

#include "Material.hpp"
#include "ray.hpp"


// Abstract base class.

class Intersectable
{
protected:
    Material* material;
public:
    Intersectable(Material* material);
    virtual Hit intersect(const Ray& ray)=0;
    virtual Intersectable* transform(mat4x4 t)=0;
};



// Simple helper class to solve quadratic equations with the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a, and store the results.
class QuadraticRoots{
public:
    float t1;
    float t2;
    
    // Solves the quadratic a*t*t + b*t + c = 0 using the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a, and sets members t1 and t2 to store the roots.
    QuadraticRoots(float a, float b, float c);
    // Returns the lesser of the positive solutions, or a negative value if there was no positive solution.
    float getLesserPositive();
    
};

// Object realization.

class Quadric : public Intersectable
{
    mat4x4 coeffs;
public:
    Quadric(Material* material);
    
    // infinite slab, ideal for clipping
    Quadric* parallelPlanes(float height);
    
    // unit sphere
    Quadric* sphere();
    
    // infinite along y
    Quadric* cylinder();
    
    // infinite along y, 45°
    Quadric* cone();
    
    // infinite along y
    Quadric* paraboloid();
    
    //hyperboloid
    Quadric* hyperboloid();
    
    Quadric* plane();
    
    Quadric* planes(float height);
    
    QuadraticRoots solveQuadratic(const Ray& ray);
    vec3 getNormalAt(vec4 x);
    
    Hit intersect(const Ray& ray);
    
    Quadric* transform(mat4x4 t);
    
    bool contains(vec3 r);
    
};



class ClippedQuadric: public Intersectable{
    Quadric shape;
    Quadric clipper;
public:
    ClippedQuadric(Material* material);
    ClippedQuadric* sphere(float height, mat4x4 location = mat4x4());
    
    //1-x2-y2-z2 = 0
    
    ClippedQuadric* cylinder(float height, mat4x4 location = mat4x4());
    
    ClippedQuadric* cone(float height , mat4x4 location = mat4x4());
    
    ClippedQuadric* paraboloid(float height,  mat4x4 location = mat4x4());
    
    ClippedQuadric* hyperboloid(float height, mat4x4 location = mat4x4());
    
    //the top and bottom face of a cylinder
    ClippedQuadric* clippedFaces(ClippedQuadric object);
    
    //clip depending on input shape and input clipper
    ClippedQuadric* freeClip(Quadric* curShape, Quadric* curClipper);
    
    ClippedQuadric* getFaces();
    
    Quadric getShape();
    
    Quadric getClipper();
    
    Material* getMaterial();
    
    Hit intersect(const Ray& ray);
    
    ClippedQuadric* transformClipper(mat4x4 t);
    
    ClippedQuadric* transform(mat4x4 t);
    
};


// CLASS PLANE COULD COME HERE
class Plane : public Intersectable{
    vec3 n;//plane normal
    vec3 r0;//plane point
public:
    Plane(const vec3& n, vec3 r0, Material* material);
    
    Hit intersect(const Ray& ray);
    
    Plane* transform(mat4x4 t);
    
};



#endif /* Intersectable_hpp */
