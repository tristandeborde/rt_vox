#ifndef RT_VOX_HPP
# define RT_VOX_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <vector>
# include <iostream>
# include "Cube.Class.hpp"

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow* initWindow(void);

void render(const std::vector<Cube> &cubes, std::string &fn);



#endif