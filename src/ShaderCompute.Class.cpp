#include "ShaderCompute.Class.hpp"


ShaderCompute::ShaderCompute()
{
    // create shader Program
    this->_ID = glCreateProgram();
    
    int cshader = this->createShader("rt.glslcs", GL_COMPUTE_SHADER);

    glAttachShader(this->_ID, cshader);
    glLinkProgram(this->_ID);
    checkLinkingErrors(this->_ID, "COMPUTE");

    glDeleteShader(cshader);
}

ShaderCompute::~ShaderCompute()
{
    return;
}

void ShaderCompute::init() {
    glUseProgram(this->_ID);
    glGetProgramiv(this->_ID, GL_COMPUTE_WORK_GROUP_SIZE, this->_workGroupSize);
    this->_camPosUniform = glGetUniformLocation(this->_ID, "camera.pos");
    this->_camDirUniform = glGetUniformLocation(this->_ID, "camera.dir");
    this->_camYAxisUniform = glGetUniformLocation(this->_ID, "camera.yAxis");
    this->_camXAxisUniform = glGetUniformLocation(this->_ID, "camera.xAxis");
    this->_camTanFovXUniform = glGetUniformLocation(this->_ID, "camera.tanFovX");
    this->_camTanFovYUniform = glGetUniformLocation(this->_ID, "camera.tanFovY");
    this->_widthUniform = glGetUniformLocation(this->_ID, "width");
    this->_heightUniform = glGetUniformLocation(this->_ID, "height");
    glUseProgram(0);
}

int *ShaderCompute::getWorkGroupSize() {
    return this->_workGroupSize;
}

int ShaderCompute::getCamPos() {
    return this->_camPosUniform;
}

int ShaderCompute::getCamDir() {
    return this->_camDirUniform;
}

int ShaderCompute::getCamYAxis() {
    return this->_camYAxisUniform;
}

int ShaderCompute::getCamXAxis() {
    return this->_camXAxisUniform;
}

int ShaderCompute::getCamTanFovX() {
    return this->_camTanFovXUniform;
}

int ShaderCompute::getCamTanFovY() {
    return this->_camTanFovYUniform;
}

int ShaderCompute::getWidth() {
    return this->_widthUniform;
}

int ShaderCompute::getHeight() {
    return this->_heightUniform;
}