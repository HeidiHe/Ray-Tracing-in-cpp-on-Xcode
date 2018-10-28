


#ifndef Shader_hpp
#define Shader_hpp

#include <stdio.h>
#include "util.hpp"

class Shader {
protected:
    unsigned int shaderProgram;
    
public:
    Shader();
    ~Shader();
    void Run();
    void UploadSamplerID();
};

#endif /* Shader_hpp */
