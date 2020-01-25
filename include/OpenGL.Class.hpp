#ifndef OPENGL_CLASS_HPP
# define OPENGL_CLASS_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <vector>
# include <iostream>
# include "Cube.Class.hpp"
# include "Camera.Class.hpp"
# include "Raytracer.Class.hpp"

# ifdef __APPLE__
    # define GL_VERSION_MAJOR 3
# elif __linux__
    # define GL_VERSION_MAJOR 4
# endif    

// settings

class OpenGL
{
private:
    // void framebuffer_size_callback(int width, int height);
    void processInput();
    void initWindow(void);

    Camera *_cam;
    GLFWwindow* _window;
    const unsigned int _screen_height;
    const unsigned int _screen_width;

public:
    OpenGL(Camera *cam);
    ~OpenGL();
    OpenGL(const OpenGL &) = delete;
    OpenGL& operator=(const OpenGL &) = delete;
    
    void mainLoop(Raytracer &rt, std::vector<Cube> cubes);
    GLFWwindow * getWindow();
};


#endif