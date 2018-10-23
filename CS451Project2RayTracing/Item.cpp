//
//  Item.cpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#include "Item.hpp"


//Item------------------------------------------------
Item::Item(int row, int col, Material* material): row(row), col(col), material(material){}    

//King------------------------------------------------
King::King(int row, int col, Material* material):
paraboloid(material), hyperboloid(material),cylinder1(material),cylinder2(material),
Item(row,col,material){
    
    
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

std::vector<Intersectable*> King::getObjects(){
    return allObjects;
}





//Bottle------------------------------------------------
Bottle::Bottle(int row, int col, Material* material):
c1(material), cone(material),c2(material),Item(row,col,material){
    
    
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

std::vector<Intersectable*> Bottle::getObjects(){
    return allObjects;
}

Bottle* Bottle::transform(mat4x4 t){
    for (unsigned int i=0; i<allObjects.size();i++){
        allObjects[i]->transform(t);
    }
    return this;
}


//Bottle------------------------------------------------
Bottle2::Bottle2(int row, int col, Material* material):
c1(material), paraboloid(material),c2(material),sphere(material),
c3(material),Item(row,col,material){
    
    
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

std::vector<Intersectable*> Bottle2::getObjects(){
    return allObjects;
}

Bottle2* Bottle2::transform(mat4x4 t){
    for (unsigned int i=0; i<allObjects.size();i++){
        allObjects[i]->transform(t);
    }
    return this;
}
