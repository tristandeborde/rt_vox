#ifndef SHADER_QUAD_CLASS_HPP
# define SHADER_QUAD_CLASS_HPP

// TODO: inherit Shader class
// Add: void initQuadProgram();
# include "Shader.Class.hpp"

class ShaderQuad: public Shader
{
public:
    ShaderQuad();
    ~ShaderQuad();

    void init();

};

#endif