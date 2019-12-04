#include "ShaderCompute.Class.hpp"


ShaderCompute::ShaderCompute(const char* filePath = "rt.glslcs")
{
    std::string path = this->_folder + filePath;

    int cshader = this->createShader(filePath, GL_COMPUTE_SHADER);

    // create shader Program
    this->_ID = glCreateProgram();
    glAttachShader(this->_ID, cshader);
    glLinkProgram(this->_ID);
    checkCompileErrors(this->_ID, "COMPUTE");
    glDeleteShader(cshader);
}

ShaderCompute::~ShaderCompute()
{
    return;
}

void ShaderCompute::init() {
    glUseProgram(this->_ID);
    glGetProgramiv(this->_ID, GL_COMPUTE_WORK_GROUP_SIZE, this->_workGroupSize);
    this->_eyeUniform = glGetUniformLocation(this->_ID, "eye");
    this->_ray00Uniform = glGetUniformLocation(this->_ID, "ray00");
    this->_ray10Uniform = glGetUniformLocation(this->_ID, "ray10");
    this->_ray01Uniform = glGetUniformLocation(this->_ID, "ray01");
    this->_ray11Uniform = glGetUniformLocation(this->_ID, "ray11");
    glUseProgram(0);
}

int *ShaderCompute::getWorkGroupSize() {
    return this->_workGroupSize;
}

int ShaderCompute::getEye() {
    return this->_eyeUniform;
}

int ShaderCompute::getRay00() {
    return this->_ray00Uniform;
}

int ShaderCompute::getRay01() {
    return this->_ray01Uniform;
}

int ShaderCompute::getRay10() {
    return this->_ray10Uniform;
}

int ShaderCompute::getRay11() {
    return this->_ray11Uniform;
}