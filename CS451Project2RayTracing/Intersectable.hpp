/*
 * File: Intersectable.hpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#ifndef Intersectable_hpp
#define Intersectable_hpp

#include "util.hpp"
#include "Material.hpp"
#include "Ray.hpp"

/* abstract Intersectable class */
class Intersectable {
protected:
    Material* material;
public:
    Intersectable(Material* material);
    virtual Hit intersect(const Ray& ray)=0;
    virtual Intersectable* transform(mat4x4 t)=0;
};


/*
 * Helper class to solve quadratic equations with the
 * Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a,
 * and store the results.
 */
class QuadraticRoots{
public:
    float t1;
    float t2;
    
    /*
     * Solves the quadratic a*t*t + b*t + c = 0
     * using the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a,
     * and sets members t1 and t2 to store the roots.
     */
    QuadraticRoots(float a, float b, float c);
    
    /* Returns the lesser of the positive solutions, or a negative value if there was no positive solution. */
    float getLesserPositive();
};

/* Quadratic Object */
class Quadric : public Intersectable {
    mat4x4 coeffs;
public:
    /* constructor of Quadric class */
    Quadric(Material* material);
    
    /* infinite 3D plane with a height */
    Quadric* parallelPlanes(float height);
    
    /* unit sphere */
    Quadric* sphere();
    
    /* cylinder, infinite along y axis */
    Quadric* cylinder();
    
    /* cone, infinite along y axis, 45° */
    Quadric* cone();
    
    /* paraboloid, infinite along y axis */
    Quadric* paraboloid();
    
    /* hyperboloid */
    Quadric* hyperboloid();
    
    /* infinite flat plane */
    Quadric* plane();
    
    /* infinite 3D plane with a height */
    Quadric* planes(float height);
    
    /* solve the Quadratic function */
    QuadraticRoots solveQuadratic(const Ray& ray);
    
    /* get the normal of the given vector */
    vec3 getNormalAt(vec4 x);
    
    /* get the ray intersection */
    Hit intersect(const Ray& ray);
    
    /* transform the Quadric object */
    Quadric* transform(mat4x4 t);
    
    /* whether a ray is contained in scene */
    bool contains(vec3 r);
};


/* ClippedQuadric class */
class ClippedQuadric: public Intersectable{
    Quadric shape;
    Quadric clipper;
public:
    /* constructor of ClippedQuadric class */
    ClippedQuadric(Material* material);
    
    /* sphere */
    ClippedQuadric* sphere(float height, mat4x4 location = mat4x4());
    
    /* cylinder */
    ClippedQuadric* cylinder(float height, mat4x4 location = mat4x4());
    
    /* cone */
    ClippedQuadric* cone(float height , mat4x4 location = mat4x4());
    
    /* paraboloid */
    ClippedQuadric* paraboloid(float height,  mat4x4 location = mat4x4());
    
    /* hyperboloid */
    ClippedQuadric* hyperboloid(float height, mat4x4 location = mat4x4());
    
    /* the top and bottom face of a cylinder */
    ClippedQuadric* clippedFaces(ClippedQuadric object);
    
    /* clip depending on input shape and input clipper */
    ClippedQuadric* freeClip(Quadric* curShape, Quadric* curClipper);
    
    /* get the faces of the Quadric object */
    ClippedQuadric* getFaces();
    
    /* get the Shape of the Quadric object */
    Quadric getShape();
    
    /* get the Clipper */
    Quadric getClipper();
    
    /* get the material */
    Material* getMaterial();
    
    /* get the ray intersection */
    Hit intersect(const Ray& ray);
    
    /* transform the ClippedQuadric using a matrix */
    ClippedQuadric* transformClipper(mat4x4 t);
    
    /* transform the ClippedQuadric */
    ClippedQuadric* transform(mat4x4 t);
};


/* Plane class */
class Plane : public Intersectable{
    vec3 n; // plane normal
    vec3 r0; //plane point
public:
    /* constructor of Plane class */
    Plane(const vec3& n, vec3 r0, Material* material);

    /* hit the ray on the Plane */
    Hit intersect(const Ray& ray);
    
    /* transform the plane */
    Plane* transform(mat4x4 t);
};

#endif /* Intersectable_hpp */
