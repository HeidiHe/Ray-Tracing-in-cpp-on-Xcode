//
//  Shader.hpp
//  CS451Project2RayTracing
//
//  Created by Heidi He on 10/22/18.
//  Copyright © 2018 Heidi He. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <stdio.h>
#include "util.hpp"

class Shader
{
protected:
    unsigned int shaderProgram;
    
public:
    Shader();
    ~Shader();
    void Run();
    void UploadSamplerID();
};


#endif /* Shader_hpp */
