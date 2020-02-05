#include "OpenGL.Class.hpp"

OpenGL::OpenGL(int w, int h): _screen_width(w), _screen_height(h)
{
    this->initWindow();
    return;
}

OpenGL::~OpenGL()
{
    return;
}

void OpenGL::updateInput()
{
    double x, y;
    glfwPollEvents();
    glfwGetCursorPos(this->_window, &x, &y);
    this->_xDiff = x - _xPos;
    this->_yDiff = y - _yPos;
    this->_xPos = x;
    this->_yPos = y;
}

bool OpenGL::isKeyPressed(unsigned int GLFW_Key) const
{
    return (glfwGetKey(this->_window, GLFW_Key) == GLFW_PRESS);
}

bool OpenGL::isMouseButtonPressed(unsigned int GLFW_MouseButton) const
{
    return (glfwGetMouseButton(this->_window, GLFW_MouseButton) == GLFW_PRESS);
}

void framebuffer_size_callback(int width, int height) {
    glViewport(0, 0, width, height);
}

void OpenGL::initWindow(void) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(3, GL_VERSION_MAJOR);
    glfwWindowHint(0, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    this->_window = glfwCreateWindow(this->_screen_width, this->_screen_height, "LearnOpenGL", NULL, NULL);
    if (this->_window) {
        glfwMakeContextCurrent(this->_window);
        // glfwSetFramebufferSizeCallback(this->_window, framebuffer_size_callback);

        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            this->_window = NULL;
        }
    }
}

GLFWwindow * OpenGL::getWindow() {
    return this->_window;
}

double OpenGL::getXPos() const
{
    return this->_xPos;
}

double OpenGL::getYPos() const {
    return this->_yPos;
}

double OpenGL::getXPosDiff() const {
    return this->_xDiff;
}

double OpenGL::getYPosDiff() const {
    return this->_yDiff;
}


