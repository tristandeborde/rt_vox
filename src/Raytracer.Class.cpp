#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cassert>
#include "Raytracer.Class.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Todo: checker si val d'init de la camera OK avec tuto
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

    // this->render_CPU();
    // exit(0);
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
    glm::vec3 eyeRay;
    this->_cShader->use();

    /* Set viewing frustum corner rays in shader */
    glUniform3f(_cShader->getEye(), _camera->getPos().x, _camera->getPos().y, _camera->getPos().z);
    eyeRay = _camera->getEyeRay(-1, -1);
    glUniform3f(_cShader->getRay00(), eyeRay.x, eyeRay.y, eyeRay.z);
    eyeRay = _camera->getEyeRay(-1, 1);
    glUniform3f(_cShader->getRay01(), eyeRay.x, eyeRay.y, eyeRay.z);
    eyeRay = _camera->getEyeRay(1, -1);
    glUniform3f(_cShader->getRay10(), eyeRay.x, eyeRay.y, eyeRay.z);
    eyeRay = _camera->getEyeRay(1, 1);
    glUniform3f(_cShader->getRay11(), eyeRay.x, eyeRay.y, eyeRay.z);

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

// CPU ########################
// ############################

void saveImage(const std::string &fn, const glm::vec3 *image) {
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./" + fn + ".ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << 1280 << " " << 1024 << "\n255\n";
    for (unsigned i = 0; i < 1280 * 1024; ++i) {
        ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
               (unsigned char)(std::min(float(1), image[i].y) * 255) <<
               (unsigned char)(std::min(float(1), image[i].z) * 255);
    }
    ofs.close();
    delete [] image;
}

struct box {
glm::vec3 min;
glm::vec3 max;
};

const box boxes[] = {
/* The ground */
{glm::vec3(-5.0, -0.1, -5.0), glm::vec3(5.0, 0.0, 5.0)},
/* Box in the middle */
{glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.0, 0.5)}
};

struct hitinfo {
glm::vec2 lambda;
int bi;
};

#define MAX_SCENE_BOUNDS 100.0
#define NUM_BOXES 2

glm::vec2 intersectBox(glm::vec3 origin, glm::vec3 dir, const box b) {
    glm::vec3 tMin = (b.min - origin) / dir;
    glm::vec3 tMax = (b.max - origin) / dir;
    glm::vec3 t1 = min(tMin, tMax);
    glm::vec3 t2 = max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
    return glm::vec2(tNear, tFar);
}

bool intersectBoxes(glm::vec3 origin, glm::vec3 dir, hitinfo &info) {
    float smallest = MAX_SCENE_BOUNDS;
    bool found = false;
    for (int i = 0; i < NUM_BOXES; i++) {
        glm::vec2 lambda = intersectBox(origin, dir, boxes[i]);
        if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
            info.lambda = lambda;
            info.bi = i;
            smallest = lambda.x;
            found = true;
        }
    }
    return found;
}

glm::vec3 trace(glm::vec3 origin, glm::vec3 dir) {
    hitinfo i;
    if (intersectBoxes(origin, dir, i)) {
        return glm::vec3(i.bi / 10.0 + 0.8);
    }
    return glm::vec3(0.0, 0.0, 0.0);
}

void Raytracer::render_CPU(void) {
    /* Set viewing frustum corner rays in shader */
    glm::vec3 eye = _camera->getPos();
    glm::vec3 eyeRay00 = _camera->getEyeRay(-1, -1);
    glm::vec3 eyeRay01 = _camera->getEyeRay(-1, 1);
    glm::vec3 eyeRay10 = _camera->getEyeRay(1, -1);
    glm::vec3 eyeRay11 = _camera->getEyeRay(1, 1);

    glm::vec3 *image = new glm::vec3[1280 * 1024];
    glm::vec3 *pixel = image;

    for (int w = 0; w < 1280; w++) {
        for (int h = 0; h < 1024; h++) {
            glm::vec2 pix(w, h);
            glm::vec2 pos = pix / glm::vec2(1280 - 1, 1024 - 1);
            glm::vec3 dir = glm::mix(glm::mix(eyeRay00, eyeRay01, pos.y), glm::mix(eyeRay10, eyeRay11, pos.y), pos.x);
            *pixel = trace(eye, dir);
            pixel++;
        }
    }
    saveImage("test1", image);
    exit(0);
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