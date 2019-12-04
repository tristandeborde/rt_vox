#include "ShaderQuad.Class.hpp"

ShaderQuad::ShaderQuad()
{
    this->_ID = glCreateProgram();
    int vshader = this->createShader("quad.vs", GL_VERTEX_SHADER);
    int fshader = this->createShader("quad.fs", GL_FRAGMENT_SHADER);

    glAttachShader(quadProgram, vshader);
    glAttachShader(quadProgram, fshader);
    glBindAttribLocation(quadProgram, 0, "vertex");
    glBindFragDataLocation(quadProgram, 0, "color");
    glLinkProgram(quadProgram);
    glGetProgrami(quadProgram, GL_LINK_STATUS);
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