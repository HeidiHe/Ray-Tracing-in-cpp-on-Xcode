
//  Scene.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/20/18.
//  Copyright © 2018 Heidi He. All rights reserved.

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

#include "util.hpp"

#include "Material.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Light.hpp"
#include "Intersectable.hpp"
#include "ray.hpp"
#include "Item.hpp"


const unsigned int windowWidth = 512, windowHeight = 512;
bool keyboardState[256];
int majorVersion = 3, minorVersion = 0;



Shader *shader = 0;

Camera camera;


class Scene
{
//  Camera camera;
    std::vector<Intersectable*> objects;
    std::vector<Material*> materials;
    std::vector<LightSource*> lights;
    std::vector<Item*> pieces;
//    Plane* plane;
    Metal* metal;
    
public:
    
    Scene(){
     
//        //create materials
        materials.push_back(new DiffuseMaterial(vec3(0,1,0),vec3(0,0,1),0.2));//0
        materials.push_back(new DiffuseMaterial(vec3(0,1,1),vec3(1,1,1),0.9));//1
//
//        //2 wood------------------------------------------------------------------------
        materials.push_back(new Wood());
//        //cylinder and its two closing sides ->3
//        objects.push_back((new ClippedQuadric(materials[2]))->cylinder(1)->transform(mat4x4::scaling(vec3(0.5, 1, 0.5))*mat4x4::translation(vec3(-2, -0.6, -1.5))));
//        objects.push_back((new ClippedQuadric(materials[2]))->cylinder(1)->getFaces()->transform(mat4x4::scaling(vec3(0.5, 1, 0.5))*mat4x4::translation(vec3(-2, -0.6, -1.5))));
//        
//      //3 gold------------------------------------------------------------------------
        materials.push_back(new Metal(vec3(3.13,2.23,1.76),vec3(0.21,0.485,1.29)));

        //4 silver------------------------------------------------------------------------
        materials.push_back(new Metal(vec3(3.7,3.11,2.47),vec3(0.15,0.14,0.13)));
        



//        //5 glass-----------------------------------------------------------------------
        materials.push_back(new Glass(0.0,1.46));//5 glass

//        
//        //Bottle
//        pieces.push_back((new Bottle(3,3,materials[1]))->
//                         transform(mat4x4::scaling(vec3(1.4, 1.1, 1.4))*mat4x4::translation(vec3(-0.5, -0.3, 0.5))));
        


        materials.push_back(new DiffuseMaterial(vec3(0.835,0.878,0.949),vec3(1,1,1),0.2));//6 light grey
        materials.push_back(new DiffuseMaterial(vec3(0.78,0.76,0.67),vec3(1,1,1),0.9));//7 grass grey
        materials.push_back(new DiffuseMaterial(vec3(1,1,1),vec3(1,1,1),0.1));//8 white
        materials.push_back(new DiffuseMaterial(vec3(0.34,0.32,0.22),vec3(1,1,1),0.2));//9 dark grey
        materials.push_back(new DiffuseMaterial(vec3(0.7, 0.64, 0.59),vec3(1,1,1),0.1));//10 rice background
        materials.push_back(new DiffuseMaterial(vec3(0.18,0.91,0.87),vec3(1,1,1),0.9));//11 green blue
        materials.push_back(new DiffuseMaterial(vec3(1,0,0),vec3(1,1,1),0.8));//12 red
        
//        objects.push_back((new ClippedQuadric(materials[4]))->
//                          hyperboloid(1.5, mat4x4::translation(vec3(0,0.2,0)))->transform(mat4x4::scaling(vec3(0.1, 0.5, 0.1))*mat4x4::translation(vec3(-4, -0.75, -0.8))));//half hyperboliod

        //cone ->7
//        objects.push_back((new ClippedQuadric(materials[8]))->
//                          cone(1)->transform(mat4x4::scaling(vec3(0.5, 0.7, 0.5))*mat4x4::translation(vec3(-4, 0.4, -1.5))));
//        objects.push_back((new ClippedQuadric(materials[8]))->
//                          cylinder(1)->transform(mat4x4::scaling(vec3(0.06, 0.25, 0.06))*mat4x4::translation(vec3(-4, 0.4, -1.5))));//small cylinder attaching to the cone
//        objects.push_back((new ClippedQuadric(materials[2]))->
//                          cylinder(1)->transform(mat4x4::scaling(vec3(0.47, 0.8, 0.47))*mat4x4::translation(vec3(-4, -0.7, -1.5)))); //large cylinder attaching to the cone

        objects.push_back((new Quadric(materials[4]))->sphere()->transform(mat4x4::scaling(vec3(0.5, 0.5, 0.5))*mat4x4::translation(vec3(-2, 0, -1.5))));
        
        objects.push_back((new Quadric(materials[5]))->sphere()->transform(mat4x4::scaling(vec3(0.3, 0.3, 0.3))*mat4x4::translation(vec3(-3, 0, -1.5))));
        
//        objects.push_back((new Quadric(materials[5]))->sphere()->transform(mat4x4::scaling(vec3(0.2, 0.2, 0.2))*mat4x4::translation(vec3(-3, 0.5, -0.5))));
        
        
//        sunlight
//        lights.push_back( new DirectionalLight(vec3(0.5,0.4,0.3),vec3(0,1,1)));
//      point light from camera
        lights.push_back(new PointLight(vec3(5,5,5),vec3(-8,2,-1))); //light intensity vs light direction
        lights.push_back(new PointLight(vec3(3,10,10),vec3(3,7,3)));
        
        
        //planes
        objects.push_back(new Plane(vec3(0,1,0),vec3(0,-1,0),materials[12]));//bottom
        objects.push_back(new Plane(vec3(0,-1,0),vec3(0,10,0),materials[9]));//top
        objects.push_back(new Plane(vec3(1,0,0),vec3(-10,0,0),materials[10]));//left
        objects.push_back(new Plane(vec3(-1,0,0),vec3(10,0,0),materials[11]));//right
        objects.push_back(new Plane(vec3(0,0,-1),vec3(0,0,10),materials[12]));//front
//        plane = new Plane(vec3(0,0,-1),vec3(0,0,10),materials[9]);//front
        objects.push_back(new Plane(vec3(0,0,1),vec3(0,0,-5),materials[12]));//back //marble //->9
        
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
        for (std::vector<Item*>::iterator iPiece = pieces.begin(); iPiece != pieces.end(); ++iPiece)
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
                

                //wood
                if (Wood* wood = dynamic_cast<Wood*>(hit.material)){
                    wood->makeWood(hit.position+hit.normal*ee, hit.normal, -ray.dir);
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
//              Camera& camera = scene.getCamera();
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
        static float vertexCoords[] = { -1, -1,     1, -1,      -1, 1,      1, 1 };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW); 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);   

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); 
        static float vertexTextureCoords[] = { 0, 0,    1, 0,       0, 1,       1, 1 };

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
    glutCreateWindow("Liwei and Heidi's masterpieces");

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
