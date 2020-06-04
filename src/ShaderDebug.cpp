#include "ShaderDebug.hpp"

ShaderDebug::ShaderDebug()
{
    this->_ID = glCreateProgram();
    int vshader = this->createShader("debug.vs", GL_VERTEX_SHADER);
    int fshader = this->createShader("debug.fs", GL_FRAGMENT_SHADER);

    glAttachShader(this->_ID, vshader);
    glAttachShader(this->_ID, fshader);
    glBindAttribLocation(this->_ID, 0, "vertex");
    glBindFragDataLocation(this->_ID, 0, "color");
    glLinkProgram(this->_ID);
    // glGetProgram(this->_ID, GL_LINK_STATUS);
    checkLinkingErrors(this->_ID, "DEBUG");
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

ShaderDebug::~ShaderDebug()
{
}