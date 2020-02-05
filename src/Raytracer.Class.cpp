#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cassert>
#include "Raytracer.Class.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

Raytracer::Raytracer(std::vector<Cube> &c, Camera *cam, OpenGL *gl): _cubes(c), _camera(cam), _gl(gl) {
    this->_width = 1280;
    this->_height = 1024;
    this->_maxRayDepth = 5;
    this->_bias = 1e-4;

    // Texture
    this->createFramebufferTexture();

    // VAO / VBO
    this->quadFullScreenVao();
    
    // Shaders
    this->_qShader = new ShaderQuad();
    this->_qShader->init();
    this->_cShader = new ShaderCompute();
    this->_cShader->init();
} 

Raytracer::~Raytracer() {
    delete this->_qShader;
    delete this->_cShader;
    return;
}

void    Raytracer::mainLoop(){

    GLFWwindow *win = this->_gl->getWindow();

    while (win && !glfwWindowShouldClose(win))
    {
        this->_gl->updateInput();
        this->_camera->update(*this->_gl, 0.1f);

        this->render_GPU();
        // TODO: rasterize_objects();

        glfwSwapBuffers(win);
    }

    // Clear all allocated GLFW resources.
    glfwTerminate();
}


void Raytracer::render_GPU(void) {
    this->_cShader->use();

    /* Set viewing frustum corner rays in shader */
    glUniform3f(_cShader->getCamPos(), _camera->getPos().x, _camera->getPos().y, _camera->getPos().z);
    glUniform3f(_cShader->getCamDir(), _camera->getLookDir().x, _camera->getLookDir().y, _camera->getLookDir().z);
    glUniform3f(_cShader->getCamYAxis(), _camera->getUp().x, _camera->getUp().y, _camera->getUp().z);
    glUniform3f(_cShader->getCamXAxis(), _camera->getRight().x, _camera->getRight().y, _camera->getRight().z);
    glUniform1f(_cShader->getCamTanFovX(), _camera->getFovX());
    glUniform1f(_cShader->getCamTanFovY(), _camera->getFovY());
    glUniform1ui(_cShader->getWidth(), this->_width);
    glUniform1ui(_cShader->getHeight(), this->_height);

    /* Bind level 0 of framebuffer texture as writable image in the shader. */
    glBindImageTexture(0, this->_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    /* Compute appropriate invocation dimension. */
    int worksizeX = Raytracer::nextPowerOfTwo(this->_width);
    int worksizeY = Raytracer::nextPowerOfTwo(this->_height);
    int workGroupSizeX = this->_cShader->getWorkGroupSize()[0];
    int workGroupSizeY = this->_cShader->getWorkGroupSize()[1];

    /* Invoke the compute shader. */
    glDispatchCompute(worksizeX / workGroupSizeX, worksizeY / workGroupSizeY, 1);

    /* Reset image binding. */
    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glUseProgram(0);

    /* Draw rendered image on the screen using textured full-screen quad. */
    this->_qShader->use();
    glBindVertexArray(this->_vao);
    glBindTexture(GL_TEXTURE_2D, this->_tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Raytracer::quadFullScreenVao() {
    glGenVertexArrays(1, &this->_vao);
    glGenBuffers(1, &this->_vbo);
    glBindVertexArray(this->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    
    // Create two triangles composing our quad
    char triangles[] = {
        -1, -1,
        1, -1,
        1, 1,
        1, 1,
        -1, 1,
        -1, -1,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_BYTE, false, 0, 0L);
    glBindVertexArray(0);
}

// Allocate empty texture
void Raytracer::createFramebufferTexture() {
    glGenTextures(1, &this->_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->_width, this->_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Utility functions

float Raytracer::mix(const float &a, const float &b, const float &mix)
{
    return b * mix + a * (1 - mix);
}

int  Raytracer::nextPowerOfTwo(unsigned int x){
    // Source: bits.stephan-brumme.com
    x--;
    x |= x >> 1; // handle 2 bit numbers
    x |= x >> 2; // handle 4 bit numbers
    x |= x >> 4; // handle 8 bit numbers
    x |= x >> 8; // handle 16 bit numbers
    x |= x >> 16; // handle 32 bit numbers
    x++;
    return x;
}