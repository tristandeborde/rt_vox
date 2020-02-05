#ifndef OPENGL_CLASS_HPP
# define OPENGL_CLASS_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <vector>
# include <iostream>

# ifdef __APPLE__
    # define GL_VERSION_MAJOR 3
# elif __linux__
    # define GL_VERSION_MAJOR 3
# endif    

// settings

class OpenGL
{
private:
    // void framebuffer_size_callback(int width, int height);
    void processInput();
    void initWindow(void);

    GLFWwindow* _window;
    const unsigned int _screen_width;
    const unsigned int _screen_height;

    double _xPos = 0;
    double _yPos = 0;
    double _xDiff = 0;
    double _yDiff = 0;

public:
    OpenGL(int width, int height);
    ~OpenGL();
    OpenGL(const OpenGL &) = delete;
    OpenGL& operator=(const OpenGL &) = delete;
    
    GLFWwindow * getWindow();

    double getXPos() const;
    double getYPos() const;
    double getXPosDiff() const;
    double getYPosDiff() const;

    void updateInput();
    bool isKeyPressed(unsigned int GLFW_Key) const;
    bool isMouseButtonPressed(unsigned int GLFW_MouseButton) const;

};


#endif