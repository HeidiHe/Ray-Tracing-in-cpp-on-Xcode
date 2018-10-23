

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>		 
#include <GL/freeglut.h>	
#endif

#include <vector>

#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"

const unsigned int windowWidth = 512, windowHeight = 512;

bool keyboardState[256];

int majorVersion = 3, minorVersion = 0;

void getErrorInfo(unsigned int handle) 
{
	int logLen;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) 
	{
		char * log = new char[logLen];
		int written;
		glGetShaderInfoLog(handle, logLen, &written, log);
		printf("Shader log:\n%s", log);
		delete log;
	}
}

void checkShader(unsigned int shader, char * message) 
{
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) 
	{
		printf("%s!\n", message);
		getErrorInfo(shader);
	}
}

void checkLinking(unsigned int program) 
{
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) 
	{
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
	}
}

class Shader
{
protected:
	unsigned int shaderProgram;

public:
	Shader()
	{
		const char *vertexSource = "\n\
			#version 150 \n\
			precision highp float; \n\
			\n\
			in vec2 vertexPosition;	\n\
			in vec2 vertexTexCoord; \n\
			out vec2 texCoord; \n\
			\n\
			void main() \n\
			{ \n\
				texCoord = vertexTexCoord; \n\
				gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1); \n\
			} \n\
		"; 

		const char *fragmentSource = "\n\
			#version 150 \n\
			precision highp float; \n\
			\n\
			uniform sampler2D samplerUnit; \n\
			in vec2 texCoord;  \n\
			out vec4 fragmentColor; \n\
			\n\
			void main() { \n\
			fragmentColor = texture(samplerUnit, texCoord);  \n\
			} \n\
		";

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!vertexShader) { printf("Error in vertex shader creation\n"); exit(1); }

		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		checkShader(vertexShader, "Vertex shader error");

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fragmentShader) { printf("Error in fragment shader creation\n"); exit(1); }

		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, "Fragment shader error");

		shaderProgram = glCreateProgram();
		if (!shaderProgram) { printf("Error in shader program creation\n"); exit(1); }

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glBindAttribLocation(shaderProgram, 0, "vertexPosition");
		glBindAttribLocation(shaderProgram, 1, "vertexTexCoord");

		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

		glLinkProgram(shaderProgram);
		checkLinking(shaderProgram);
	}

	~Shader()
	{
		if(shaderProgram) glDeleteProgram(shaderProgram);
	}

	void Run()
	{
		if(shaderProgram) glUseProgram(shaderProgram);
	}

	void UploadSamplerID()
	{
		int samplerUnit = 0; 
		int location = glGetUniformLocation(shaderProgram, "samplerUnit");
		glUniform1i(location, samplerUnit);
		glActiveTexture(GL_TEXTURE0 + samplerUnit); 
	}
};

Shader *shader = 0;

// Simple material class, with object color, and headlight shading.
class Material{
    
public:
    virtual vec3 shade(vec3 normal,
                       vec3 viewDir,
                       vec3 lightDir,
                       vec3 powerDensity)=0;
    
    
};

class DiffuseMaterial : public Material
{
    vec3 kd;
    vec3 ks;
    float shiness;
public:
    DiffuseMaterial(vec3 kd, vec3 ks=vec3(0,0,0), float shiness=1.0):kd(kd),ks(ks),shiness(shiness){

    }
    
    vec3 getColor(vec3 position,
                  vec3 normal,
                  vec3 viewDir){
        return kd;
    }
    
    vec3 shade(
               vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        
        float cosTheta = normal.dot(lightDir);

        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta)+ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
//        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);

        return L;
    }

};

class Metal : public Material{
    vec3 exCoeff;
    vec3 reIndex;
    vec3 R0;
public:
    Metal(vec3 ex, vec3 re){
        exCoeff = ex;
        reIndex = re;
        R0 = ((re-vec3(1,1,1))*(re-vec3(1,1,1))+ex*ex)/
                ((re+vec3(1,1,1))*(re+vec3(1,1,1))+ex*ex);
    }
    
    
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        return vec3(0,0,0);
    }
    
    vec3 reflect(  	vec3 inDir,
                   vec3 normal)
    {
        vec3 perp = normal * normal.dot(inDir);
        vec3 parallel = inDir - perp;
        return parallel - perp;
    }
    
    vec3 R(vec3 dir, vec3 n) {
        float alpha = dir.dot(n);
        return R0 + (vec3(1,1,1)-R0)*pow(1-cos(alpha), 5);
    }
};


class Glass : public Material {
    float  refractiveIndex;
    float extinctCoeff;
    float R0;
public:
    Glass(float ex, float re){
        extinctCoeff = ex;
        refractiveIndex = re;
        R0 = powf(refractiveIndex-1.0, 2.0)+extinctCoeff*extinctCoeff
        /powf(refractiveIndex+1.0, 2.0)+extinctCoeff*extinctCoeff;
    }
    
    float T(vec3 dir, vec3 n) {
        float alpha = dir.dot(n);
        float R = R0 + (1-R0)*pow(1-cos(alpha), 5);
        return 1-R;
    }
    
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        return vec3(0,0,0);
    }
    
    
    vec3 refract(vec3 inDir, vec3 normal) {
        //vec3 perp = -normal * normal.dot(inDir);
        vec3 perp = -normal * normal.dot(inDir);
        vec3 parallel = inDir - perp;
        
        float ri = refractiveIndex;
        float cosa = -normal.dot(inDir);
        if (cosa < 0) { cosa = -cosa; normal = -normal; ri = 1/ri; }
        float disc = 1 - (1 - cosa * cosa) / ri / ri;
        float cosb = (disc < 0)?0:sqrt(disc);
        return parallel*(1.0/ri) - normal * cosb;
    }
    
    vec3 reflect(  	vec3 inDir,
                 vec3 normal)
    {
        vec3 perp = normal * normal.dot(inDir);
        vec3 parallel = inDir - perp;
        return parallel - perp;
    }
    
    float R(vec3 dir, vec3 n) {
        float alpha = dir.dot(n);
        return (R0 + (1-R0)*powf(1-cos(alpha), 5));
//        float R = (R0 + (1-R0)*powf(1-cos(alpha), 5));
//        return 2*R*0.2 / (1+R);
    }
};



class ChessBoard : public Material{
    vec3 kd = vec3(0,0,0);
public:
    ChessBoard(){}
        
    void makeBoard(vec3 position){
        if(position.x<0 && position.x>-8 && position.z<0 && position.z>-8){
            if(fmod((int)position.x,2)==fmod((int)position.z,2)){
                kd = vec3(0,0,0);
            }
            else{
                kd = vec3(1,1,1);
            }
        }
        else{
            kd = vec3(0.72, 0.72, 0.72);
        }
        

    }
        
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        float cosTheta = normal.dot(lightDir);
        
        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta);
        
        //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
        
        return L;
        
    }
    
};

class Stripe : public Material{
    vec3 kd = vec3(1,1,1);
public:
    Stripe(){}
    
    void makeStripe(vec3 position){
        
        if(fmod((int)(position.x*10.0),2)==0){
            kd = vec3(0,0,0);
        }
        else{
            kd = vec3(1,1,1);
        }

        
    }
    
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        float cosTheta = normal.dot(lightDir);
        
        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta)
                + vec3(1,1,1)*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),0.5);
        //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
        
        return L;
        
    }
    
};

//Wood
class Wood : public Material
{
    float scale;
    float turbulence;
    float period;
    float sharpness;
    vec3 kd;
public:
    Wood()
    {
        scale = 16;
        turbulence = 500;
        period = 8;
        sharpness = 10;
        kd = vec3(0.10,0.98,0.26);
    }
    
    void makeWood(  vec3 position, vec3 normal, vec3 viewDir){
        //return normal;
        float w = position.x * period + pow(snoise(position * scale), sharpness)*turbulence + 10000.0;
        w -= int(w);
        //light wood vec3(0.7,0.6,0.3)
        kd= (vec3(0.5,0.3,0) * w + vec3(0.3, 0.18, 0.05) * (1-w)) * normal.dot(viewDir);
    }
    
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        
        float cosTheta = normal.dot(lightDir);
        
        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta)
                +vec3(0.3,0.3,0.3)*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),0.2);
        //        vec3 L = ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
        
        return L;
    }
    
    float snoise(vec3 r) {
        unsigned int x = 0x0625DF73;
        unsigned int y = 0xD1B84B45;
        unsigned int z = 0x152AD8D0;
        float f = 0;
        for(int i=0; i<32; i++) {
            vec3 s(	x/(float)0xffffffff,
                   y/(float)0xffffffff,
                   z/(float)0xffffffff);
            f += sin(s.dot(r));
            x = x << 1 | x >> 31;
            y = y << 1 | y >> 31;
            z = z << 1 | z >> 31;
        }
        return f / 64.0 + 0.5;
    }
};

//Marble
class Marble : public Material
{
    float scale;
    float turbulence;
    float period;
    float sharpness;
    vec3 kd;
    vec3 ks;
    float shiness;
public:
    Marble()
    {
        scale = 32;
        turbulence = 50;
        period = 32;
        sharpness = 1;
        kd = vec3(0.96, 0.96, 0.96);
        ks = vec3(1,1,1);
        shiness = 1;
    }
    
    void makeMarble(  vec3 position, vec3 normal, vec3 viewDir){
        float w = position.x * period + pow(snoise(position * scale), sharpness)*turbulence;
        w = pow(sin(w)*0.5+0.5, 4);
        kd = (vec3(0.96, 0.96, 0.96) * w + vec3(0.72, 0.72, 0.72) * (1-w)) * normal.dot(viewDir);
    }
    
    vec3 shade(vec3 normal,
               vec3 viewDir,
               vec3 lightDir,
               vec3 powerDensity){
        
        float cosTheta = normal.dot(lightDir);
        
        vec3 halfway = (viewDir + lightDir).normalize();
        
        vec3 L = kd*powerDensity*fmax(0.0, cosTheta)+ ks*powerDensity*pow((double)fmax(0.0, normal.dot(halfway)),shiness);
        
        return L;
    }
    
    float snoise(vec3 r) {
        unsigned int x = 0x0625DF73;
        unsigned int y = 0xD1B84B45;
        unsigned int z = 0x152AD8D0;
        float f = 0;
        for(int i=0; i<32; i++) {
            vec3 s(	x/(float)0xffffffff,
                   y/(float)0xffffffff,
                   z/(float)0xffffffff);
            f += sin(s.dot(r));
            x = x << 1 | x >> 31;
            y = y << 1 | y >> 31;
            z = z << 1 | z >> 31;
        }
        return f / 64.0 + 0.5;
    }
};



class Camera
{
	vec3 eye;		// World space camera position.
	vec3 lookAt;	// Center of window in world space.
	vec3 right;		// Vector from window center to window right-mid (in world space).
	vec3 up;		// Vector from window center to window top-mid (in world space).

public:
	Camera()
	{
		eye = vec3(-3.5, 0, 1);
		lookAt = vec3(-3.5, 0, 0);
		right = vec3(1, 0, 0);
		up = vec3(0, 1, 0);
	}
	vec3 getEye()
	{
		return eye;
	}

	// Compute ray through pixel at normalized device coordinates.
    void Control(){
        if(keyboardState['d']) eye.x+=0.1;//right
        if(keyboardState['a']) eye.x-=0.1;//left
        if(keyboardState['w']) {eye.z-=0.1; printf("moving forward");}//foward
        if(keyboardState['s']) {eye.z+=0.1;}//back
        if(keyboardState['x']) {eye.y+=0.1;}//up
        if(keyboardState['z']) {eye.y-=0.1;}//down
    }
    
	vec3 rayDirFromNdc(float x, float y) {
		return (lookAt - eye
			+ right * x
			+ up    * y
			).normalize();
	}
};


class LightSource
{
public:
    virtual vec3 getPowerDensityAt ( vec3 x )=0;
    virtual vec3 getLightDirAt     ( vec3 x )=0;
    virtual float  getDistanceFrom ( vec3 x )=0;
};

class DirectionalLight : public LightSource{
    vec3 p;//power density
    vec3 l;//light direction
public:
    DirectionalLight(vec3 p, vec3 l):p(p),l(l){

    }
    
    vec3 getPowerDensityAt( vec3 x ){
        return p;
    }
    
    vec3 getLightDirAt( vec3 x ){
        return l;
    }
    
    float getDistanceFrom( vec3 x ){
        return MAXFLOAT;
    }
    
};

class PointLight : public LightSource{
    vec3 p;//power density
    vec3 l;//light point position
public:
    PointLight(vec3 p, vec3 l):p(p),l(l){
        
    }
    
    vec3 getPowerDensityAt( vec3 x ){
        float distance = getDistanceFrom(x);
        float disInv = 1.0/(distance*distance);
        return p*disInv;
    }
    
    vec3 getLightDirAt( vec3 x ){
        vec3 lightDir = l-x;
        return lightDir.normalize();
    }
    
    float getDistanceFrom( vec3 x ){
        float distance = (l-x).norm();
//        float distance = powf((x.x-l.x), 2.0) + powf((x.y-l.y), 2.0) + powf((x.z-l.z), 2.0);
        return distance;
    }
    
};



// Ray structure.
class Ray
{
public:
    vec3 origin;
    vec3 dir;
    Ray(vec3 o, vec3 d)
    {
        origin = o;
        dir = d;
    }
};

// Hit record structure. Contains all data that describes a ray-object intersection point.

class Hit
{
public:
	Hit()
	{
		t = -1;
	}
	float t;				// Ray paramter at intersection. Negative means no valid intersection.
	vec3 position;			// Intersection coordinates.
	vec3 normal;			// Surface normal at intersection.
	Material* material;		// Material of intersected surface.
};

// Abstract base class.

class Intersectable
{
protected:
	Material* material;
public:
	Intersectable(Material* material):material(material) {}
    virtual Hit intersect(const Ray& ray)=0;
    virtual Intersectable* transform(mat4x4 t)=0;
};

// Simple helper class to solve quadratic equations with the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a, and store the results.

class QuadraticRoots
{
public:
	float t1;
	float t2;

	// Solves the quadratic a*t*t + b*t + c = 0 using the Quadratic Formula [-b +- sqrt(b^2-4ac)] / 2a, and sets members t1 and t2 to store the roots.

	QuadraticRoots(float a, float b, float c)
	{
        float discr = b * b - 4.0 * a * c;
        if ( discr < 0 ) // no roots
		{
			t1 = -1;
			t2 = -1;
			return;
		}
        float sqrt_discr = sqrt( discr );
		t1 = (-b + sqrt_discr)/2.0/a;
		t2 = (-b - sqrt_discr)/2.0/a;
	}

	// Returns the lesser of the positive solutions, or a negative value if there was no positive solution.
	float getLesserPositive()
	{
		return (0 < t1 && (t2 < 0 || t1 < t2)) ? t1 : t2;
	}

};

// Object realization.

class Quadric : public Intersectable
{
    mat4x4 coeffs;
public:
    Quadric(Material* material):
		Intersectable(material){}
    
    // infinite slab, ideal for clipping
    Quadric* parallelPlanes(float height) {
        coeffs._00 = 0;
        coeffs._11 = 1;
        coeffs._22 = 0;
        coeffs._33 = -(height/2)*(height/2);
        return this;
    }

    // unit sphere
    Quadric* sphere(){
        coeffs._33 = -1;
        return this;
    }
    
    // infinite along y
    Quadric* cylinder(){
        coeffs._00 = 1;
        coeffs._11 = 0;
        coeffs._22 = 1;
        coeffs._33 = -1;
        return this;
    }
    
    // infinite along y, 45°
    Quadric* cone(){
        coeffs._00 = 1;
        coeffs._11 = -1;
        coeffs._22 = 1;
        coeffs._33 = 0;
        return this;
    }
    
    // infinite along y
    Quadric* paraboloid(){
        coeffs._00 = 1;
        coeffs._11 = 0;
        coeffs._13 = -1;
        coeffs._22 = 1;
        coeffs._33 = 0;
        return this;
    }
    
    //hyperboloid
    Quadric* hyperboloid(){
        coeffs._00 = 1;
        coeffs._11 = -1;
        coeffs._22 = 1;
        coeffs._33 = -1;
        return this;
    }
    
    Quadric* plane(){
        coeffs._00=0;
        coeffs._11=0;
        coeffs._13=1;
        coeffs._22=0;
        coeffs._33=1;
        return this;
    }
    
    Quadric* planes(float height){
        coeffs._00=0;
        coeffs._11=1;
        coeffs._22=0;
        coeffs._33=-height*height;
        //y square = height
        return this;
    }

	QuadraticRoots solveQuadratic(const Ray& ray)
	{
        vec4 d = vec4(ray.dir.x, ray.dir.y, ray.dir.z, 0);
        vec4 e = vec4(ray.origin);
        float a = d.dot(coeffs*d);
        float b = d.dot(coeffs*e) + e.dot(coeffs*d);
        float c = e.dot(coeffs*e);
		return QuadraticRoots(a, b, c);
	}
	vec3 getNormalAt(vec4 x)
	{
		vec4 result = (coeffs*x + x*coeffs).normalize();
        return vec3(result.x, result.y,  result.z);
	}
    
    Hit intersect(const Ray& ray)
    {
		// This is a generic intersect that works for any shape with a quadratic equation. solveQuadratic should solve the proper equation (+ ray equation) for the shape, and getNormalAt should return the proper normal.

		float t = solveQuadratic(ray).getLesserPositive();
			
		Hit hit;
		hit.t = t;
		hit.material = material;
		hit.position = ray.origin + ray.dir * t;
		hit.normal = getNormalAt(hit.position);

		return hit;
    }
    
    Quadric* transform(mat4x4 t){
        t = t._invert();
        coeffs = t*coeffs*t.transpose();
        return this;
    }
    
    bool contains(vec3 r){
        vec4 ray = vec4(r);// 0? 1?
        float eq = ray.dot(coeffs*ray);
        // return true if negative
        if (eq<0){
            return true;
        }
        else{
            return false;
        }
    }

}; 

class ClippedQuadric: public Intersectable{
    Quadric shape;
    Quadric clipper;
public:
    ClippedQuadric(Material* material): shape(material), clipper(material), Intersectable(material){
        
    }
    
    ClippedQuadric* sphere(float height, mat4x4 location = mat4x4()){
        shape.sphere();
        clipper.parallelPlanes(height);
        return this;
    }
    
    //1-x2-y2-z2 = 0
    
    ClippedQuadric* cylinder(float height, mat4x4 location = mat4x4()){
        shape.cylinder();
        clipper.parallelPlanes(height);
        return this;
    }
    
    ClippedQuadric* cone(float height , mat4x4 location = mat4x4()){
        shape.cone();
        clipper.parallelPlanes(height);
        clipper.transform(mat4x4::translation(vec3(0, -height/2, 0)));
        return this;
    }
    
    ClippedQuadric* paraboloid(float height,  mat4x4 location = mat4x4()){
        shape.paraboloid();
        clipper.parallelPlanes(height)->transform(location);
        return this;
    }
    
    ClippedQuadric* hyperboloid(float height, mat4x4 location = mat4x4()){
        shape.hyperboloid();
        clipper.parallelPlanes(height)->transform(location);
        return this;
    }
    
    //the top and bottom face of a cylinder
    ClippedQuadric* clippedFaces(ClippedQuadric object){
        shape = object.getClipper();
        clipper = object.getShape();
        return this;
    }
    
    //clip depending on input shape and input clipper
    ClippedQuadric* freeClip(Quadric* curShape, Quadric* curClipper){
        shape = *curShape;
        clipper = *curClipper;
        return this;
    }
    
    ClippedQuadric* getFaces(){
        ClippedQuadric* q1 = new ClippedQuadric(getMaterial());
        printf("get faces");
        return q1->clippedFaces(*this);
    }

    Quadric getShape(){
        return shape;
    }
    
    Quadric getClipper(){
        return clipper;
    }
    
    Material* getMaterial(){
        return material;
    }
    
    Hit intersect(const Ray& ray){
    
        QuadraticRoots roots = shape.solveQuadratic(ray);
        
        vec3 p1=ray.origin+ray.dir*roots.t1;
        vec3 p2=ray.origin+ray.dir*roots.t2;
        
        //reverse
        if(clipper.contains(p1)==false){roots.t1 = -1;}
        if(clipper.contains(p2)==false){roots.t2 = -1;}
        
        float t = roots.getLesserPositive();
        
        Hit hit;
        hit.t = t;
        hit.material = material;
        hit.position = ray.origin + ray.dir * t;
        hit.normal = shape.getNormalAt(hit.position);
        
        return hit;
    }
    
    ClippedQuadric* transformClipper(mat4x4 t){
        clipper.transform(t);
        return this;
    }
    
    ClippedQuadric* transform(mat4x4 t){
        shape.transform(t);
        clipper.transform(t);
        return this;
    }

};

class ChessPiece{
    int row;
    int col;
    Material* material;
public:
    ChessPiece(int row, int col, Material* material): row(row), col(col), material(material){}
    
    virtual std::vector<Intersectable*> getObjects()=0;
    
    
};

class Pawn : public ChessPiece{
    Quadric head;
    ClippedQuadric body;
    std::vector<Intersectable*> allObjects;
public:
    Pawn(int row, int col, Material* material):
            head(material), body (material), ChessPiece(row,col,material){
        head.sphere()->transform(
                                 mat4x4::scaling(vec3(0.25, 0.25, 0.25))
                                 *mat4x4::translation(vec3(-row+0.5, 0, -col+0.5)));
        body.cone(1.5)->transform(
                                  mat4x4::scaling(vec3(0.5, 1.5, 0.5))
                                  *mat4x4::translation(vec3(-row+0.5, 0, -col+0.5))
                                  );
        allObjects.push_back(&head);
        allObjects.push_back(&body);
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
    
};

class King: public ChessPiece{
    ClippedQuadric paraboloid;
    ClippedQuadric hyperboloid;
    ClippedQuadric cylinder1;
    ClippedQuadric cylinder2;
    std::vector<Intersectable*> allObjects;
public:
    King(int row, int col, Material* material):
        paraboloid(material), hyperboloid(material),cylinder1(material),cylinder2(material),
        ChessPiece(row,col,material){

            
            paraboloid.paraboloid(2)->transform(mat4x4::scaling(vec3(0.5, 0.5, 0.5))
                                                *mat4x4::translation(vec3(-row+0.5,0.2, -col+0.5)));
            
            //body
            hyperboloid.hyperboloid(3, mat4x4::translation(vec3(0, -1, 0)))
                                    ->transform(mat4x4::scaling(vec3(0.25, 0.7, 0.25))*
                                       mat4x4::translation(vec3(-row+0.5, 0, -col+0.5)));
        
            //crossing
            cylinder1.cylinder(1)->transform(mat4x4::scaling(vec3(0.1, 0.7, 0.1))
                                            *mat4x4::rotation(vec3(0, 0, 1), 1.57)
                                            *mat4x4::translation(vec3(-row+0.5, 1, -col+0.5))
                                            );
            cylinder2.cylinder(1)->transform(
                                             
                                             mat4x4::scaling(vec3(0.1, 0.7, 0.1))
                                             *mat4x4::translation(vec3(-row+0.5, 1, -col+0.5))
                                             );
            
            allObjects.push_back(&paraboloid);
            allObjects.push_back(paraboloid.getFaces());
            allObjects.push_back(&hyperboloid);
            allObjects.push_back(hyperboloid.getFaces());
            allObjects.push_back(&cylinder1);
            allObjects.push_back(cylinder1.getFaces());
            allObjects.push_back(&cylinder2);
            allObjects.push_back(cylinder2.getFaces());
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
};

class Bishop : public ChessPiece{
    ClippedQuadric hyperboloid;
    ClippedQuadric head;
    Quadric hat;
    std::vector<Intersectable*> allObjects;
public:
    Bishop(int row, int col, Material* material): hyperboloid(material),head(material),hat(material), ChessPiece(row,col,material){
        
//        Quadric* clipperSphere = (new Quadric(material))->sphere()->transform(
//                                                        mat4x4::scaling(vec3(0.2, 0.5, 0.2)));
//        
//        Quadric* headSphere = (new Quadric(material))->sphere();
//        
//        head.freeClip(clipperSphere, headSphere)->transform(
//                                            mat4x4::scaling(vec3(0.3, 0.3, 0.3))*
//                                            mat4x4::translation(vec3(-row+0.5, 1, -col+0.5)));
        head.sphere(3)->transform( mat4x4::scaling(vec3(0.3, 0.4, 0.3))
                                  * mat4x4::translation(vec3(-row+0.5, 0.5, -col+0.5)));
        hat.sphere()->transform( mat4x4::scaling(vec3(0.1, 0.1, 0.1))
                                * mat4x4::translation(vec3(-row+0.5, 1.0, -col+0.5)));
        //body
        hyperboloid.hyperboloid(3, mat4x4::translation(vec3(0, -1, 0)))
        ->transform(mat4x4::scaling(vec3(0.15, 0.5, 0.15))*
                    mat4x4::translation(vec3(-row+0.5, 0, -col+0.5)));
        
//        allObjects.push_back(clipperSphere);
//        allObjects.push_back(headSphere);
        allObjects.push_back(&hyperboloid);
        allObjects.push_back(hyperboloid.getFaces());
        allObjects.push_back(&hat);
        allObjects.push_back(&head);
//        allObjects.push_back(head.getFaces());
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
};

//queen
class Queen: public ChessPiece{
    ClippedQuadric body;
    ClippedQuadric gown;
    Quadric sphere;
    std::vector<Intersectable*> allObjects;
public:
    Queen(int row, int col, Material* material):
    body(material), gown(material),sphere(material),ChessPiece(row,col,material){
        
        
        gown.hyperboloid(2)->transform(mat4x4::scaling(vec3(0.3, 0.3, 0.3))
                                            *mat4x4::translation(vec3(-row+0.5,0.5, -col+0.5)));
        
        //body
        body.hyperboloid(3, mat4x4::translation(vec3(0, -1, 0)))
        ->transform(mat4x4::scaling(vec3(0.25, 0.7, 0.25))*
                    mat4x4::translation(vec3(-row+0.5, 0, -col+0.5)));
        
        //hat
        sphere.sphere()->transform(
                                         
                                         mat4x4::scaling(vec3(0.15, 0.15, 0.15))
                                         *mat4x4::translation(vec3(-row+0.5, 1.0, -col+0.5))
                                         );
        
        allObjects.push_back(&gown);
        allObjects.push_back(gown.getFaces());
        allObjects.push_back(&body);
        allObjects.push_back(body.getFaces());
        allObjects.push_back(&sphere);
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
};

//Bottle
class Bottle: public ChessPiece{
    ClippedQuadric c1;
    ClippedQuadric cone;
    ClippedQuadric c2;
    std::vector<Intersectable*> allObjects;
public:
    Bottle(int row, int col, Material* material):
    c1(material), cone(material),c2(material),ChessPiece(row,col,material){
        
        
        c1.cylinder(2)->transform(mat4x4::scaling(vec3(0.5, 0.8, 0.5))
                                       *mat4x4::translation(vec3(-row+0.5,0, -col+0.5)));
        
        //body
        cone.cone(1.5, mat4x4::translation(vec3(0, -1, 0)))
        ->transform(mat4x4::scaling(vec3(0.33, 0.7, 0.33))*
                    mat4x4::translation(vec3(-row+0.5, 1.8, -col+0.5)));
        
        //hat
        c2.cylinder(2)->transform(
                                   
                                   mat4x4::scaling(vec3(0.15, 0.4, 0.15))
                                   *mat4x4::translation(vec3(-row+0.5, 1.6, -col+0.5))
                                   );
        
        allObjects.push_back(&c1);
        allObjects.push_back(&cone);
        allObjects.push_back(&c2);
        allObjects.push_back(c2.getFaces());
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
    
    Bottle* transform(mat4x4 t){
        for (unsigned int i=0; i<allObjects.size();i++){
            allObjects[i]->transform(t);
        }
        return this;
    }
};

//Bottle
class Bottle2: public ChessPiece{
    ClippedQuadric c1;
    ClippedQuadric paraboloid;
    ClippedQuadric c2;
    ClippedQuadric sphere;
    std::vector<Intersectable*> allObjects;
    ClippedQuadric c3;
public:
    Bottle2(int row, int col, Material* material):
    c1(material), paraboloid(material),c2(material),sphere(material),
    c3(material),ChessPiece(row,col,material){
        
        
        c1.cylinder(2)->transform(mat4x4::scaling(vec3(0.485, 0.5, 0.485))
                                  *mat4x4::translation(vec3(-row+0.5,-0.3, -col+0.5)));
        
        //body
        paraboloid.paraboloid(2, mat4x4::translation(vec3(0, 0.5, 0)))
        ->transform(mat4x4::rotation( vec3(0,0,1), 3.14159)*
                    mat4x4::scaling(vec3(0.4, 0.3, 0.4))*
                    mat4x4::translation(vec3(-row+0.5,0.65, -col+0.5)));
        
        //long cylinder
        c2.cylinder(2)->transform(
                                  mat4x4::scaling(vec3(0.15, 0.6, 0.15))
                                  *mat4x4::translation(vec3(-row+0.5, 1.1, -col+0.5))
                                  );
        
        sphere.sphere(5)->transform(
                                   
                                   mat4x4::scaling(vec3(0.2, 0.2, 0.2))
                                   *mat4x4::translation(vec3(-row+0.5, 0.8, -col+0.5))
                                   );
        
        //topper
        c3.cylinder(0.2)->transform(
                                  mat4x4::scaling(vec3(0.2, 1, 0.2))
                                  *mat4x4::translation(vec3(-row+0.5, 1.6, -col+0.5))
                                  );
        allObjects.push_back(&c1);
        allObjects.push_back(c1.getFaces());
        allObjects.push_back(&paraboloid);
        allObjects.push_back(&c2);
        allObjects.push_back(&sphere);
        allObjects.push_back(&c3);
        allObjects.push_back(c3.getFaces());
    }
    
    std::vector<Intersectable*> getObjects(){
        return allObjects;
    }
    
    Bottle2* transform(mat4x4 t){
        for (unsigned int i=0; i<allObjects.size();i++){
            allObjects[i]->transform(t);
        }
        return this;
    }
};

// CLASS PLANE COULD COME HERE
class Plane : public Intersectable{
    vec3 n;//plane normal
    vec3 r0;//plane point
public:
    Plane(const vec3& n, vec3 r0, Material* material):
            Intersectable(material),n(n),r0(r0){ }
    
    
    Hit intersect(const Ray& ray){
        
        float t = ((r0-ray.origin).dot(n))/ray.dir.dot(n);
        
        Hit hit;
        hit.t = t;
        hit.material = material;
        hit.position = ray.origin + ray.dir * t;
        hit.normal = n;
        
        return hit;
    }
    
    Plane* transform(mat4x4 t){
        return this;
    }
    
};

Camera camera;


class Scene
{
//	Camera camera;
    std::vector<Intersectable*> objects;
    std::vector<Material*> materials;
    std::vector<LightSource*> lights;
    std::vector<ChessPiece*> pieces;
    Plane* plane;
    Metal* metal;
    
public:
    
    Scene()
	{
        //create materials
        materials.push_back(new DiffuseMaterial(vec3(0,1,0),vec3(0,0,1),0.2));//0
        materials.push_back(new DiffuseMaterial(vec3(0,1,1),vec3(1,1,1),0.9));//1
        materials.push_back(new ChessBoard());//2 chess board
        materials.push_back(new Wood());//3 wood
        materials.push_back(new Metal(vec3(3.13,2.23,1.76),vec3(0.21,0.485,1.29)));//4 gold
        materials.push_back(new Metal(vec3(3.7,3.11,2.47),vec3(0.15,0.14,0.13)));//5 silver
        materials.push_back(new Glass(0.0,1.46));//6 glass
        materials.push_back(new Marble());//7 marble
        materials.push_back(new Stripe());//8 stripe
        materials.push_back(new DiffuseMaterial(vec3(0.835,0.878,0.949),vec3(1,1,1),0.2));//9 light grey
        materials.push_back(new DiffuseMaterial(vec3(0.78,0.76,0.67),vec3(1,1,1),0.5));//10 grass grey
        materials.push_back(new DiffuseMaterial(vec3(0.56,0.54,0.46),vec3(1,1,1),0.1));//11
        materials.push_back(new DiffuseMaterial(vec3(0.34,0.32,0.22),vec3(1,1,1),0.8));//12 dark grey
        materials.push_back(new DiffuseMaterial(vec3(0.71,0.49,0.43),vec3(1,1,1),0.8));//13 dull red
        materials.push_back(new DiffuseMaterial(vec3(0.47,0.62,0.80),vec3(1,1,1),0.8));//13 dull dark blue

//        sunlight
//        lights.push_back( new DirectionalLight(vec3(0.5,0.4,0.3),vec3(0,1,1)));
        //point light from camera
//        lights.push_back(new PointLight(vec3(0,5,5),vec3(0,2,0)));
        lights.push_back(new PointLight(vec3(10,10,10),vec3(-6,4,0)));

        
        //cylinder and its two closing sides ->3
        objects.push_back((new ClippedQuadric(materials[3]))->cylinder(1)->transform(mat4x4::scaling(vec3(0.5, 1, 0.5))*mat4x4::translation(vec3(-2, -0.6, -1.5))));
        
        objects.push_back((new ClippedQuadric(materials[3]))->cylinder(1)->getFaces()->transform(mat4x4::scaling(vec3(0.5, 1, 0.5))*mat4x4::translation(vec3(-2, -0.6, -1.5))));
        
        //cone ->7
        objects.push_back((new ClippedQuadric(materials[5]))->cone(1)->transform(mat4x4::scaling(vec3(0.5, 0.8, 0.5))*mat4x4::translation(vec3(-2, 0.5, -1.5))));
        //small cylinder attaching to the cone
        objects.push_back((new ClippedQuadric(materials[5]))->cylinder(1)->transform(mat4x4::scaling(vec3(0.06, 1, 0.06))*mat4x4::translation(vec3(-2, 0.5, -1.5))));
        
        //half hyperboliod
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(1.5, mat4x4::translation(vec3(0,0.2,0)))->transform(mat4x4::scaling(vec3(0.4, 1.0, 0.4))*mat4x4::translation(vec3(-2.5, -0.5, -2))));
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(1.5, mat4x4::translation(vec3(0,0.2,0)))->getFaces()->transform(mat4x4::scaling(vec3(0.4, 1.0, 0.4))*mat4x4::translation(vec3(-2.5, -0.5, -2))));
        
        //Bottle
        pieces.push_back((new Bottle(3,3,materials[8]))->transform(mat4x4::translation(vec3(-1.5, -0.4, 0))));
        
//        hyperboliod 1 ->8
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(3)->transform(
                                                                mat4x4::scaling(vec3(0.25, 0.3, 0.25))*
                                                                mat4x4::translation(vec3(-3.5, -0.6, -1.5))));
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(3)->getFaces()->transform(
                                                                                        mat4x4::scaling(vec3(0.25, 0.3, 0.25))*
                                                                                        mat4x4::translation(vec3(-3.5, -0.6, -1.5))));
        //another glass
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(3)->transform(mat4x4::scaling(vec3(0.25, 0.3, 0.25))*mat4x4::translation(vec3(-5, -0.6, -1.7))));
        objects.push_back((new ClippedQuadric(materials[6]))->hyperboloid(3)->getFaces()->transform(mat4x4::scaling(vec3(0.25, 0.3, 0.25))*mat4x4::translation(vec3(-5, -0.6, -1.7))));

        //Bottle2
//        pieces.push_back((new Bottle2(4,2,materials[10]))->transform(
//                                                    mat4x4::translation(vec3(-1, -0.2, 0))));
        pieces.push_back((new Bottle2(6,2,materials[7]))->transform(mat4x4::scaling(vec3(0.6, 0.8, 0.6))*
                                                            mat4x4::translation(vec3(-1, -0.4, -0.5))));
        

        
        //planes
        objects.push_back(new Plane(vec3(0,1,0),vec3(0,-1,0),materials[7]));//bottom
       objects.push_back(new Plane(vec3(0,-1,0),vec3(0,10,0),materials[9]));//top
        objects.push_back(new Plane(vec3(1,0,0),vec3(-10,0,0),materials[9]));//left
        objects.push_back(new Plane(vec3(-1,0,0),vec3(10,0,0),materials[9]));//right
        plane = new Plane(vec3(0,0,-1),vec3(0,0,10),materials[9]);//front
        objects.push_back(new Plane(vec3(0,0,1),vec3(0,0,-5),materials[9]));//back //marble //->9
        
        for(unsigned int i = 0; i < pieces.size(); i++){
//            pieces[i]->pushObjects(objects);
            for(unsigned int j = 0; j < pieces[i]->getObjects().size(); j++){
                objects.push_back(pieces[i]->getObjects()[j]);
            }
        }
        printf("object size: %lu\n", objects.size());
        
        

	}
	~Scene()
	{
		// UNCOMMENT THESE WHEN APPROPRIATE
		for (std::vector<Material*>::iterator iMaterial = materials.begin(); iMaterial != materials.end(); ++iMaterial)
			delete *iMaterial;
        for (std::vector<ChessPiece*>::iterator iPiece = pieces.begin(); iPiece != pieces.end(); ++iPiece)
            delete *iPiece;
		for (std::vector<Intersectable*>::iterator iObject = objects.begin(); iObject != objects.end(); ++iObject)
			delete *iObject;
        for (std::vector<LightSource*>::iterator iLight = lights.begin(); iLight != lights.end(); ++iLight)
            delete *iLight;
	}



public:
	Camera& getCamera()
	{
		return camera;
	}

	vec3 trace(const Ray& ray, int depth)
	{
        int maxDepth = 15;
        float ee = 0.001;// to eliminate noise
        vec3 sum(0,0,0);
//        GLboolean reflective = false;
//        GLboolean refractive = false;
        
        Hit hit = firstIntersect(ray);
        
        
        if(hit.t < 0){
            //return background
            return vec3(0.835,0.878,0.949);//dark
        }
        
        for(unsigned int i = 0; i < lights.size(); i++){
            
            LightSource* curLight = lights[i];
            vec3 lightDir = curLight->getLightDirAt(hit.position);
            
            
            Ray ShadowRay(hit.position+hit.normal*ee, lightDir);
            
            Hit shadowHit = firstIntersect(ShadowRay);
            
            if ((shadowHit.t<0.0)||(curLight->getDistanceFrom(hit.position)<shadowHit.t)){
                
                //chess board
                if(ChessBoard* cb = dynamic_cast<ChessBoard*>(hit.material)){
                    cb->makeBoard(hit.position+hit.normal*ee);
                }
                //wood
                else if (Wood* wood = dynamic_cast<Wood*>(hit.material)){
                    wood->makeWood(hit.position+hit.normal*ee, hit.normal, -ray.dir);
                }
                //marble
                else if (Marble* marble = dynamic_cast<Marble*>(hit.material)){
                    marble->makeMarble(hit.position+hit.normal*ee, hit.normal, -ray.dir);
                }
                //chess board
                else if(Stripe* stripe = dynamic_cast<Stripe*>(hit.material)){
                    stripe->makeStripe(hit.position+hit.normal*ee);
                }
                
                sum += hit.material->shade(hit.normal, -ray.dir, lightDir, curLight->getPowerDensityAt(hit.position));

            }
        }
        
        if(depth > maxDepth){
            return vec3(0.835,0.878,0.949);//dark
        }
        
        //Metal
        if(Metal* metal = dynamic_cast< Metal*>(hit.material)){
            vec3 reflectionDir = metal->reflect(ray.dir, hit.normal);
            Ray reflectedRay(hit.position+hit.normal*ee, reflectionDir );
            sum += trace(reflectedRay, depth+1)*metal->R(ray.dir, hit.normal);
            //     sum += trace(reflectedRay, depth+1)* R(ray.dir, hit.normal);
        }
        
        if(Glass* glass = dynamic_cast<Glass*>(hit.material)) {
            vec3 reflectionDir = glass->reflect(ray.dir, hit.normal);
            Ray reflectedRay(hit.position+hit.normal*ee, reflectionDir );
            sum += trace(reflectedRay, depth+1)*glass->R(ray.dir, hit.normal);
            
            vec3 refractionDir = glass->refract(ray.dir, hit.normal);
            if (hit.normal.dot(-ray.dir)>0){
                Ray refractedRay(hit.position-hit.normal*ee, refractionDir );
                sum += trace(refractedRay, depth+1) * glass->T(ray.dir, hit.normal);
            }
            else{
                Ray refractedRay(hit.position+hit.normal*ee, refractionDir );
                sum += trace(refractedRay, depth+1) * glass->T(ray.dir, hit.normal);
            }
            
        }
        
        
		return sum;


    }
    
    Hit firstIntersect(Ray ray){
        float tmean = MAXFLOAT;
        
        Hit curHit;
        
        for(unsigned int i=0; i<objects.size(); i++){
            Hit newHit = objects[i]->intersect(ray);
            if (newHit.t>0.0){
                if( newHit.t < tmean ){
                    tmean = newHit.t;
                    curHit = newHit;
                }
            }
        }
        
        return curHit;
    }
    

};

Scene scene;




class FrameBuffer {
	unsigned int textureId;
	vec3 image[windowWidth * windowHeight];

public:
	FrameBuffer() {
		for(int i = 0; i < windowWidth * windowHeight; i++) image[i] = vec3(0.0, 0.0, 0.0);

		glGenTextures(1, &textureId); 
		glBindTexture(GL_TEXTURE_2D, textureId); 

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	}

	void Bind(Shader* s)
	{
		s->UploadSamplerID();
		glBindTexture(GL_TEXTURE_2D, textureId);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, image);
	}

	bool ComputeImage()
	{
		static unsigned int iPart = 0;

		if(iPart >= 64)
			return false;
		for(int j = iPart; j < windowHeight; j+=64)
		{
			for(int i = 0; i < windowWidth; i++)
			{
				float ndcX = (2.0 * i - windowWidth) / windowWidth;
				float ndcY = (2.0 * j - windowHeight) / windowHeight;
//				Camera& camera = scene.getCamera();
				Ray ray = Ray(camera.getEye(), camera.rayDirFromNdc(ndcX, ndcY));
			
				image[j*windowWidth + i] = scene.trace(ray,0);
			}
		}
		iPart++;
		return true;
	}
};

class Screen {
	FrameBuffer frameBuffer;
	unsigned int vao;

public:
	Screen() 
	{ 
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		unsigned int vbo[2];
		glGenBuffers(2, &vbo[0]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); 
		static float vertexCoords[] = { -1, -1,		1, -1,		-1, 1,		1, 1 };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW); 
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);   

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); 
		static float vertexTextureCoords[] = { 0, 0,	1, 0,		0, 1,		1, 1 };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTextureCoords), vertexTextureCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);  
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
	}

	void Draw(Shader* s)
	{
		if(frameBuffer.ComputeImage())
		glutPostRedisplay();

		s->Run();
		frameBuffer.Bind(s);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(vao); 
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisable(GL_BLEND);
	}
};

Screen *screen = 0;


void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	screen->Draw(shader);

    glutSwapBuffers(); 
}

void onInitialization() 
{
	glViewport(0, 0, windowWidth, windowHeight);

	shader = new Shader();
	
	screen = new Screen();
}

void onExit() 
{
	delete screen; screen = 0;
	delete shader; shader = 0;	
	printf("exit");
}

void onIdle(){
    camera.Control();
}

void onKeyboard(unsigned char key, int x, int y)
{
    keyboardState[key] = true;
}

void onKeyboardUp(unsigned char key, int x, int y)
{
    keyboardState[key] = false;
}

void onReshape(int winWidth, int winHeight)
{
    glViewport(0, 0, winWidth, winHeight);
}

int main(int argc, char * argv[]) {
	glutInit(&argc, argv);
#if !defined(__APPLE__)
	glutInitContextVersion(majorVersion, minorVersion);
#endif
	glutInitWindowSize(windowWidth, windowHeight);				
	glutInitWindowPosition(100, 100);							
#if defined(__APPLE__)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);  
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutCreateWindow("Ray Casting");

#if !defined(__APPLE__)
	glewExperimental = true;	
	glewInit();
#endif

	printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glViewport(0, 0, windowWidth, windowHeight);

	onInitialization();
    
    
	glutDisplayFunc(onDisplay);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutReshapeFunc(onReshape);

	glutMainLoop();
		
	onExit();
	
	return 1;
}

