/*
 * File: Intersectable.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Intersectable.hpp"

/* Abstract Intersectable class */
Intersectable::Intersectable(Material* material):material(material) {}

/*
 * Solves the quadratic a*t*t + b*t + c = 0
 * using the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a,
 * and sets members t1 and t2 to store the roots.
 */
QuadraticRoots::QuadraticRoots(float a, float b, float c) {
    float discr = b * b - 4.0 * a * c;
    // no root
    if ( discr < 0 ) {
        t1 = -1;
        t2 = -1;
        return;
    }
    float sqrt_discr = sqrt( discr );
    t1 = (-b + sqrt_discr)/2.0/a;
    t2 = (-b - sqrt_discr)/2.0/a;
}
    
/*
 * Returns the lesser of the positive solutions,
 * or a negative value if there was no positive solution.
 */
float QuadraticRoots::getLesserPositive(){
    return (0 < t1 && (t2 < 0 || t1 < t2)) ? t1 : t2;
}


/* Quadratic Object */
Quadric::Quadric(Material* material):Intersectable(material){}

/* infinite 3D plane with a height */
Quadric* Quadric::parallelPlanes(float height) {
    coeffs._00 = 0;
    coeffs._11 = 1;
    coeffs._22 = 0;
    coeffs._33 = -(height/2)*(height/2);
    return this;
}

/* unit sphere */
Quadric* Quadric::sphere(){
    coeffs._33 = -1;
    return this;
}

/* cylinder, infinite along y axis */
Quadric* Quadric::cylinder(){
    coeffs._00 = 1;
    coeffs._11 = 0;
    coeffs._22 = 1;
    coeffs._33 = -1;
    return this;
}

/* cone, infinite along y axis, 45° */
Quadric* Quadric::cone(){
    coeffs._00 = 1;
    coeffs._11 = -1;
    coeffs._22 = 1;
    coeffs._33 = 0;
    return this;
}

/* paraboloid, infinite along y axis */
Quadric* Quadric::paraboloid(){
    coeffs._00 = 1;
    coeffs._11 = 0;
    coeffs._13 = -1;
    coeffs._22 = 1;
    coeffs._33 = 0;
    return this;
}

/* hyperboloid */
Quadric* Quadric::hyperboloid(){
    coeffs._00 = 1;
    coeffs._11 = -1;
    coeffs._22 = 1;
    coeffs._33 = -1;
    return this;
}

/* infinite flat plane */
Quadric* Quadric::plane(){
    coeffs._00=0;
    coeffs._11=0;
    coeffs._13=1;
    coeffs._22=0;
    coeffs._33=1;
    return this;
}

/* infinite 3D plane with a height */
Quadric* Quadric::planes(float height){
    coeffs._00=0;
    coeffs._11=1;
    coeffs._22=0;
    coeffs._33=-height*height;
    //y square = height
    return this;
}

/* solve the Quadratic function */
QuadraticRoots Quadric::solveQuadratic(const Ray& ray) {
    vec4 d = vec4(ray.dir.x, ray.dir.y, ray.dir.z, 0);
    vec4 e = vec4(ray.origin);
    float a = d.dot(coeffs*d);
    float b = d.dot(coeffs*e) + e.dot(coeffs*d);
    float c = e.dot(coeffs*e);
    return QuadraticRoots(a, b, c);
}

/* get the normal of the given vector */
vec3 Quadric::getNormalAt(vec4 x) {
    vec4 result = (coeffs*x + x*coeffs).normalize();
    return vec3(result.x, result.y,  result.z);
}

/* get the ray intersection */
Hit Quadric::intersect(const Ray& ray) {
    float t = solveQuadratic(ray).getLesserPositive();

    Hit hit;
    hit.t = t;
    hit.material = material;
    hit.position = ray.origin + ray.dir * t;
    hit.normal = getNormalAt(hit.position);
    
    return hit;
}

/* transform the Quadric object */
Quadric* Quadric::transform(mat4x4 t){
    t = t._invert();
    coeffs = t*coeffs*t.transpose();
    return this;
}

/* whether a ray is contained in scene */
bool Quadric::contains(vec3 r){
    vec4 ray = vec4(r);
    float eq = ray.dot(coeffs * ray);
    // negative
    if (eq < 0) {
        return true;
    }
    // positive
    else{
        return false;
    }
}
    
/* ClippedQuadric class */
ClippedQuadric::ClippedQuadric(Material* material): shape(material), clipper(material), Intersectable(material){
    
}

/* sphere */
ClippedQuadric* ClippedQuadric::sphere(float height, mat4x4 location){
    shape.sphere();
    clipper.parallelPlanes(height);
    return this;
}

/* cylinder */
ClippedQuadric* ClippedQuadric::cylinder(float height, mat4x4 location){
    shape.cylinder();
    clipper.parallelPlanes(height);
    return this;
}

/* cone */
ClippedQuadric* ClippedQuadric::cone(float height , mat4x4 location){
    shape.cone();
    clipper.parallelPlanes(height);
    clipper.transform(mat4x4::translation(vec3(0, -height/2, 0)));
    return this;
}

/* paraboloid */
ClippedQuadric* ClippedQuadric::paraboloid(float height,  mat4x4 location){
    shape.paraboloid();
    clipper.parallelPlanes(height)->transform(location);
    return this;
}

/* hyperboloid */
ClippedQuadric* ClippedQuadric::hyperboloid(float height, mat4x4 location){
    shape.hyperboloid();
    clipper.parallelPlanes(height)->transform(location);
    return this;
}

/* the top and bottom face of a cylinder */
ClippedQuadric* ClippedQuadric::clippedFaces(ClippedQuadric object){
    shape = object.getClipper();
    clipper = object.getShape();
    return this;
}

/* clip depending on input shape and input clipper */
ClippedQuadric* ClippedQuadric::freeClip(Quadric* curShape, Quadric* curClipper){
    shape = *curShape;
    clipper = *curClipper;
    return this;
}

/* get the faces of the Quadric object */
ClippedQuadric* ClippedQuadric::getFaces(){
    ClippedQuadric* q1 = new ClippedQuadric(getMaterial());
    return q1->clippedFaces(*this);
}

/* get the Shape of the Quadric object */
Quadric ClippedQuadric::getShape(){
    return shape;
}

/* get the Clipper */
Quadric ClippedQuadric::getClipper(){
    return clipper;
}

/* get the material */
Material* ClippedQuadric::getMaterial(){
    return material;
}

/* get the ray intersection */
Hit ClippedQuadric::intersect(const Ray& ray){
    QuadraticRoots roots = shape.solveQuadratic(ray);
    
    vec3 p1=ray.origin+ray.dir*roots.t1;
    vec3 p2=ray.origin+ray.dir*roots.t2;
    
    // reverse
    if(clipper.contains(p1) == false) { roots.t1 = -1; }
    if(clipper.contains(p2) == false) { roots.t2 = -1; }
    
    float t = roots.getLesserPositive();
    
    Hit hit;
    hit.t = t;
    hit.material = material;
    hit.position = ray.origin + ray.dir * t;
    hit.normal = shape.getNormalAt(hit.position);
    
    return hit;
}

/* transform the ClippedQuadric using a matrix */
ClippedQuadric* ClippedQuadric::transformClipper(mat4x4 t){
    clipper.transform(t);
    return this;
}

/* transform the ClippedQuadric */
ClippedQuadric* ClippedQuadric::transform(mat4x4 t){
    shape.transform(t);
    clipper.transform(t);
    return this;
}

/* Plane class */
Plane::Plane(const vec3& n, vec3 r0, Material* material): Intersectable(material),n(n),r0(r0){}

/* hit the ray on the Plane */
Hit Plane::intersect(const Ray& ray){
    float t = ((r0-ray.origin).dot(n))/ray.dir.dot(n);
    
    Hit hit;
    hit.t = t;
    hit.material = material;
    hit.position = ray.origin + ray.dir * t;
    hit.normal = n;
    
    return hit;
}

/* transform the plane */
Plane* Plane::transform(mat4x4 t){
    return this;
}
