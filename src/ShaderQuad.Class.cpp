#include "ShaderQuad.Class.hpp"

ShaderQuad::ShaderQuad()
{
    this->_ID = glCreateProgram();
    int vshader = this->createShader("quad.vs", GL_VERTEX_SHADER);
    int fshader = this->createShader("quad.fs", GL_FRAGMENT_SHADER);

    glAttachShader(this->_ID, vshader);
    glAttachShader(this->_ID, fshader);
    glBindAttribLocation(this->_ID, 0, "vertex");
    glBindFragDataLocation(this->_ID, 0, "color");
    glLinkProgram(this->_ID);
    glGetProgrami(this->_ID, GL_LINK_STATUS);
    checkCompileErrors(this->_ID, "QUAD");
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

ShaderQuad::~ShaderQuad()
{
}

void Shader::init() {
    glUseProgram(this->_ID);
    int texUniform = glGetUniformLocation(this->_ID, "tex");
    glUniform1i(texUniform, 0);
    glUseProgram(0);
}