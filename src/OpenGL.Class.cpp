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

void GLAPIENTRY debugMessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void OpenGL::initWindow(void) {
    // glfw: initialize and configure

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    this->_window = glfwCreateWindow(this->_screen_width, this->_screen_height, "rt_vox", NULL, NULL);
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

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( debugMessageCallback, 0 );
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


