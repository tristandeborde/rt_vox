#ifndef SHADER_COMPUTE_CLASS_HPP
# define SHADER_COMPUTE_CLASS_HPP

# include "Shader.Class.hpp"

class ShaderCompute: public Shader
{
private:
    int _workGroupSize[3];
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