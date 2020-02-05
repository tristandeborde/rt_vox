#ifndef SHADER_COMPUTE_CLASS_HPP
# define SHADER_COMPUTE_CLASS_HPP

// TODO: inherit Shader class
// add: void initComputeProgram();

# include "Shader.Class.hpp"

class ShaderCompute: public Shader
{
private:
    int _workGroupSize[3];
    int _ray00Uniform;
    int _ray01Uniform;
    int _ray10Uniform;
    int _ray11Uniform;
    int _eyeUniform;
    int _camPosUniform;
    int _camDirUniform;
    int _camYAxisUniform;
    int _camXAxisUniform;
    int _camTanFovXUniform;
    int _camTanFovYUniform;
    int _widthUniform;
    int _heightUniform;

public:
    ShaderCompute();
    ~ShaderCompute();

    // activate the shader
    void init();

    int *getWorkGroupSize();
    int getCamPos();
    int getCamDir();
    int getCamYAxis();
    int getCamXAxis();
    int getCamTanFovX();
    int getCamTanFovY();
    int getWidth();
    int getHeight();
};



#endif