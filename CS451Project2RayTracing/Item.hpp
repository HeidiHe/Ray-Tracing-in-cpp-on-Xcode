//
//  Item.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef Item_hpp
#define Item_hpp

#include <stdio.h>
#include <stdio.h>
#include "vec3.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4x4.h"
#include "util.hpp"

#include "Material.hpp"
#include "ray.hpp"
#include "Intersectable.hpp"

class Item{
    int row;
    int col;
    Material* material;
public:
    Item(int row, int col, Material* material);
    virtual std::vector<Intersectable*> getObjects()=0;
};



class Pawn : public Item{
    Quadric head;
    ClippedQuadric body;
    std::vector<Intersectable*> allObjects;
public:
    Pawn(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
};



class King: public Item{
    ClippedQuadric paraboloid;
    ClippedQuadric hyperboloid;
    ClippedQuadric cylinder1;
    ClippedQuadric cylinder2;
    std::vector<Intersectable*> allObjects;
public:
    King(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
};



class Bishop : public Item{
    ClippedQuadric hyperboloid;
    ClippedQuadric head;
    Quadric hat;
    std::vector<Intersectable*> allObjects;
public:
    Bishop(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
};

//queen
class Queen: public Item{
    ClippedQuadric body;
    ClippedQuadric gown;
    Quadric sphere;
    std::vector<Intersectable*> allObjects;
public:
    Queen(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
};

//Bottle
class Bottle: public Item{
    ClippedQuadric c1;
    ClippedQuadric cone;
    ClippedQuadric c2;
    std::vector<Intersectable*> allObjects;
public:
    Bottle(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
    Bottle* transform(mat4x4 t);
};



//Bottle
class Bottle2: public Item{
    ClippedQuadric c1;
    ClippedQuadric paraboloid;
    ClippedQuadric c2;
    ClippedQuadric sphere;
    std::vector<Intersectable*> allObjects;
    ClippedQuadric c3;
public:
    Bottle2(int row, int col, Material* material);
    std::vector<Intersectable*> getObjects();
    Bottle2* transform(mat4x4 t);
};


#endif /* Item_hpp */
