#ifndef SHADER_COMPUTE_CLASS_HPP
# define SHADER_COMPUTE_CLASS_HPP

// TODO: inherit Shader class
// add: void initComputeProgram();

# include "Shader.Class.hpp"

class ShaderCompute: public Shader
{
private:
    int _workGroupSize[3];
    int _eyeUniform;
    int _ray00Uniform;
    int _ray01Uniform;
    int _ray10Uniform;
    int _ray11Uniform;

public:
    ShaderCompute();
    ~ShaderCompute();

    // activate the shader
    void init();

    int *getWorkGroupSize();
    int getEye();
    int getRay00();
    int getRay01();
    int getRay10();
    int getRay11();
};



#endif