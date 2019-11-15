#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "rt_vox.hpp"

void    mainLoop(GLFWwindow* window, std::vector<Cube> cubes) {
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        render();
        // TODO: rasterize_objects();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clear all allocated GLFW resources.
    glfwTerminate();
}

std::vector<Cube>& createObjects() {
    // LEGACY CODE FROM SCRATCH A PIXEL. THIS NEEDS TO BE REPLACED BY:
    srand48(13);
    std::vector<Cube> cubes;
    // position, radius, surface color, reflectivity, transparency, emission color
    cubes.push_back(cube(Vec3f( 0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
    cubes.push_back(cube(Vec3f( 0.0,      0, -20),     4, Vec3f(1.00, 0.32, 0.36), 1, 0.5));
    cubes.push_back(cube(Vec3f( 5.0,     -1, -15),     2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
    cubes.push_back(cube(Vec3f( 5.0,      0, -25),     3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
    cubes.push_back(cube(Vec3f(-5.5,      0, -15),     3, Vec3f(0.90, 0.90, 0.90), 1, 0.0));
    // light (yes its initialized as a cube)
    cubes.push_back(cube(Vec3f( 0.0,     20, -30),     3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));
    return cubes;
}

int main(void)
{
    GLFWwindow* window = initWindow();
    if (!window) {
        glfwTerminate();    
        return 1;
    }
    // TODO: initOctree();
    std::vector<Cube> cubes = createObjects();
    mainLoop(window, cubes);
    return 0;
}

