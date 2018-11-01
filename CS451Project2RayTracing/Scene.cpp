/*
 * File: Scene.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "util.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Light.hpp"
#include "Intersectable.hpp"
#include "Item.hpp"

const unsigned int windowWidth = 800, windowHeight = 800;
bool keyboardState[256];
int majorVersion = 3, minorVersion = 0;
Shader *shader = 0;
Camera camera;

/* Scene class */
class Scene {
    std::vector<Intersectable*> objects;
    std::vector<Material*> materials;
    std::vector<LightSource*> lights;
    std::vector<Item*> pieces;
    Metal* metal;
    
public:
    /* constructor of scene */
    Scene(){
        // [0] create materials
        materials.push_back(new DiffuseMaterial(vec3(0,1,0), vec3(0,0,1), 0.2));
        // [1]
        materials.push_back(new DiffuseMaterial(vec3(0,1,1), vec3(1,1,1), 0.9));
        // [2] wood material
        materials.push_back(new Wood());
        // [3] gold
        materials.push_back(new Metal(vec3(3.13,2.23,1.76), vec3(0.21,0.485,1.29)));
        // [4] silver
        materials.push_back(new Metal(vec3(3.7,3.11,2.47), vec3(0.15,0.14,0.13)));
        // [5] glass
        materials.push_back(new Glass(0.0,1.46));
        // [6] light grey
        materials.push_back(new DiffuseMaterial(vec3(0.835,0.878,0.949), vec3(1,1,1),0.2));
        // [7] grass grey
        materials.push_back(new DiffuseMaterial(vec3(0.78,0.76,0.67), vec3(1,1,1),0.9));
        // [8] white
        materials.push_back(new DiffuseMaterial(vec3(1,1,1), vec3(1,1,1),0.1));
        // [9] dark grey
        materials.push_back(new DiffuseMaterial(vec3(0.34,0.32,0.22), vec3(1,1,1),0.2));
        // [10] rice background
        materials.push_back(new DiffuseMaterial(vec3(0.7, 0.64, 0.59), vec3(1,1,1),0.1));
        // [11] green blue
        materials.push_back(new DiffuseMaterial(vec3(0.18,0.91,0.87), vec3(1,1,1),0.9));
        // [12] red
        materials.push_back(new DiffuseMaterial(vec3(1,0,0),vec3(1,1,1),0.8));
        
        /* ----------------------------------------------- create objects and push them into the stack ----------------------------------------------- */
        objects.push_back((new Quadric(materials[1]))->sphere()->transform(mat4x4::scaling(vec3(0.5, 0.5, 0.5)) * mat4x4::translation(vec3(-2, 0, -1.5))));
        objects.push_back((new Quadric(materials[1]))->sphere()->transform(mat4x4::scaling(vec3(0.3, 0.3, 0.3)) * mat4x4::translation(vec3(-3.5, 0, -1.5))));

        // objects.push_back((new Quadric(materials[5]))->sphere()->transform(mat4x4::scaling(vec3(0.3, 0.3, 0.3)) * mat4x4::translation(vec3(-3, 0, -1.5))));
        
        // point light from camera
        // light intensity vs light direction
        lights.push_back(new PointLight(vec3(5, 5, 5), vec3(-2, 2, 2)));
//        lights.push_back(new DirectionalLight(vec3(5, 0, 5), vec3(1, 2, 0)));
//        lights.push_back(new PointLight(vec3(3,10,10), vec3(3,7,3)));
        
        // planes
        objects.push_back(new Plane(vec3(0,1,0), vec3(0,-1,0), materials[12])); // bottom
        objects.push_back(new Plane(vec3(0,-1,0), vec3(0,10,0), materials[9])); // top
        objects.push_back(new Plane(vec3(1,0,0), vec3(-10,0,0), materials[10])); // left
        objects.push_back(new Plane(vec3(-1,0,0), vec3(10,0,0), materials[11])); // right
        objects.push_back(new Plane(vec3(0,0,-1), vec3(0,0,10), materials[12])); // front
        objects.push_back(new Plane(vec3(0,0,1), vec3(0,0,-5), materials[12])); // back marble
        
    
        
        
    }
    
    /* delete the scene object */
    ~Scene() {
        for (std::vector<Material*>::iterator iMaterial = materials.begin(); iMaterial != materials.end(); ++iMaterial) {
            delete *iMaterial;
        }
        for (std::vector<Item*>::iterator iPiece = pieces.begin(); iPiece != pieces.end(); ++iPiece) {
            delete *iPiece;
        }
        for (std::vector<Intersectable*>::iterator iObject = objects.begin(); iObject != objects.end(); ++iObject) {
            delete *iObject;
        }
        for (std::vector<LightSource*>::iterator iLight = lights.begin(); iLight != lights.end(); ++iLight) {
            delete *iLight;
        }
    }

public:
    /* get the Camera object */
    Camera& getCamera() {
        return camera;
    }
    
    /* trace */
    vec3 trace(const Ray& ray, int depth) {
        int maxDepth = 15;
        float ee = 0.001; // to eliminate noise
        vec3 sum(0,0,0);
        
        Hit hit = firstIntersect(ray);
        
        // return dark background
        if(hit.t < 0) {
            return vec3(0.835,0.878,0.949);
        }
        
        for(unsigned int i = 0; i < lights.size(); i++) {
            LightSource* curLight = lights[i];
            vec3 lightDir = curLight->getLightDirAt(hit.position);
            Ray ShadowRay(hit.position + hit.normal*ee, lightDir);
            Hit shadowHit = firstIntersect(ShadowRay);
            
            if ((shadowHit.t<0.0)||(curLight->getDistanceFrom(hit.position)<shadowHit.t)) {
                // wood
                if (Wood* wood = dynamic_cast<Wood*>(hit.material)) {
                    wood->makeWood(hit.position + hit.normal*ee, hit.normal, -ray.dir);
                }
                // chess board
                else if(Stripe* stripe = dynamic_cast<Stripe*>(hit.material)) {
                    stripe->makeStripe(hit.position + hit.normal*ee);
                }
                sum += hit.material->shade(hit.normal, -ray.dir, lightDir, curLight->getPowerDensityAt(hit.position));
            }
        }
        
        // if the depth is greater than maxDepth, then set the background to dark
        if(depth > maxDepth) {
            return vec3(0.835,0.878,0.949);
        }
        
        // Metal
        if(Metal* metal = dynamic_cast<Metal*>(hit.material)) {
            vec3 reflectionDir = metal->reflect(ray.dir, hit.normal);
            Ray reflectedRay(hit.position + hit.normal*ee, reflectionDir);
            sum += trace(reflectedRay, depth+1)*metal->R(ray.dir, hit.normal);
        }
        
        // Glass
        if(Glass* glass = dynamic_cast<Glass*>(hit.material)) {
            vec3 reflectionDir = glass->reflect(ray.dir, hit.normal);
            Ray reflectedRay(hit.position+hit.normal * ee, reflectionDir);
            sum += trace(reflectedRay, depth+1) * glass->R(ray.dir, hit.normal);
            
            vec3 refractionDir = glass->refract(ray.dir, hit.normal);
            if (hit.normal.dot(-ray.dir) > 0) {
                Ray refractedRay(hit.position-hit.normal * ee, refractionDir );
                sum += trace(refractedRay, depth+1) * glass->T(ray.dir, hit.normal);
            }
            else {
                Ray refractedRay(hit.position + hit.normal * ee, refractionDir );
                sum += trace(refractedRay, depth+1) * glass->T(ray.dir, hit.normal);
            }
        }
        return sum;
    }
    
    /* find the intersection and draw that */
    Hit firstIntersect(Ray ray){
        float tmean = MAXFLOAT;
        Hit curHit;
        
        for(unsigned int i = 0; i < objects.size(); i++) {
            Hit newHit = objects[i]->intersect(ray);
            if (newHit.t > 0.0) {
                if( newHit.t < tmean ) {
                    tmean = newHit.t;
                    curHit = newHit;
                }
            }
        }
        return curHit;
    }
};


Scene scene;

/* FrameBuffer class */
class FrameBuffer {
    unsigned int textureId;
    vec3 image[windowWidth * windowHeight];

public:
    /* constructor of FrameBuffer */
    FrameBuffer() {
        for(int i = 0; i < windowWidth * windowHeight; i++) image[i] = vec3(0.0, 0.0, 0.0);
        glGenTextures(1, &textureId); 
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    }

    /* bind the Shader with the buffer */
    void Bind(Shader* s) {
        s->UploadSamplerID();
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, image);
    }

    /* compute the image to draw on the screen */
    bool ComputeImage() {
        static unsigned int iPart = 0;
        if(iPart >= 64) {
            return false;
        }
        for(int j = iPart; j < windowHeight; j+=64) {
            for(int i = 0; i < windowWidth; i++) {
                float ndcX = (2.0 * i - windowWidth) / windowWidth;
                float ndcY = (2.0 * j - windowHeight) / windowHeight;
                Ray ray1 = Ray(camera.getEye(), camera.rayDirFromNdc(ndcX, ndcY));
                Ray ray2 = Ray(camera.getEye(), camera.rayDirFromNdc(ndcX, ndcY));
                Ray ray3 = Ray(camera.getEye(), camera.rayDirFromNdc(ndcX, ndcY));
                Ray ray4 = Ray(camera.getEye(), camera.rayDirFromNdc(ndcX, ndcY));
                
                image[j*windowWidth + i] = scene.trace((ray1 + ray2 + ray3 + ray4) / 4, 0);
            }
        }
        iPart++;
        return true;
    }
};


/* Screen class */
class Screen {
    FrameBuffer frameBuffer;
    unsigned int vao;

public:
    /* constructor of Screen */
    Screen() {
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

    /* draw the scene on the screen */
    void Draw(Shader* s) {
        if(frameBuffer.ComputeImage()) {
            glutPostRedisplay();
        }
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

/* display the scene */
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screen->Draw(shader);
    glutSwapBuffers(); 
}

/* initilizes the Screen and Shader object */
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
    screen = new Screen();
    shader = new Shader();
}

/*
 * called when the program exits
 * deletes the Screen and Shader objects created
 */
void onExit() {
    delete screen;
    screen = 0;
    delete shader;
    shader = 0;
    printf("exit");
}

/* main function */
int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);              
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutCreateWindow("Liwei and Heidi's masterpieces");

    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    glViewport(0, 0, windowWidth, windowHeight);
    onInitialization();
    glutDisplayFunc(onDisplay);
    glutMainLoop();
    
    onExit();
    return 1;
}
