/*
 * File: Shader.hpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */
#include "util.hpp"

/* Shader class */
class Shader {
protected:
    unsigned int shaderProgram;
    
public:
    /* constructor of Shader */
    Shader();
    
    /* deconstruct the Shader classs */
    ~Shader();
    
    /* run the Shader on GPU */
    void Run();
    
    /* upload sampler ID */
    void UploadSamplerID();
};
