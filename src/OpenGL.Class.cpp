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

/*
//TODO: Attach Camera pointer to modify it.
void OpenGL::processInput() {
    if(glfwGetKey(this->_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->_window, true);
    float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(this->_window, GLFW_KEY_W) == GLFW_PRESS)
        _cam->setPosition(_cam->getPosition() + cameraSpeed * _cam->getDirection()); // normalize direction ?
    if (glfwGetKey(this->_window, GLFW_KEY_S) == GLFW_PRESS)
        _cam->setPosition(_cam->getPosition() - cameraSpeed * _cam->getDirection()); // normalize direction ?
    if (glfwGetKey(this->_window, GLFW_KEY_A) == GLFW_PRESS)
        _cam->setPosition(_cam->getPosition() - glm::normalize(glm::cross(_cam->getDirection(), _cam->getUp())) * cameraSpeed);
    if (glfwGetKey(this->_window, GLFW_KEY_D) == GLFW_PRESS)
        _cam->setPosition(_cam->getPosition() + glm::normalize(glm::cross(_cam->getDirection(), _cam->getUp())) * cameraSpeed);
}
*/

void framebuffer_size_callback(int width, int height) {
    // make sure the viewport matches the new this->_window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void OpenGL::initWindow(void) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    // glfw this->_window creation
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


