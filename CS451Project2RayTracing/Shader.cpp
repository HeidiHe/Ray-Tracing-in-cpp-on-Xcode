/*
 * File: Shader.cpp
 * F18 CS451 Project 2
 * Names: Liwei Jiang, Heidi He
 * Date: 10/10/2018
 */

#include "Shader.hpp"

/* constructor of Shader */
Shader::Shader() {
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

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!fragmentShader) { printf("Error in fragment shader creation\n"); exit(1); }
        
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        if (!shaderProgram) { printf("Error in shader program creation\n"); exit(1); }
        
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        
        glBindAttribLocation(shaderProgram, 0, "vertexPosition");
        glBindAttribLocation(shaderProgram, 1, "vertexTexCoord");
        
        glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
        
        glLinkProgram(shaderProgram);
    }

/* deconstruct the Shader classs */
Shader::~Shader(){
    if(shaderProgram) glDeleteProgram(shaderProgram);
}

/* run the Shader on GPU */
void Shader::Run(){
    if(shaderProgram) glUseProgram(shaderProgram);
}

/* upload sampler ID */
void Shader::UploadSamplerID(){
    int samplerUnit = 0; 
    int location = glGetUniformLocation(shaderProgram, "samplerUnit");
    glUniform1i(location, samplerUnit);
    glActiveTexture(GL_TEXTURE0 + samplerUnit); 
}
