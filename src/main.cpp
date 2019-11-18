#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "rt_vox.hpp"
#include "Cube.Class.hpp"

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

void    createObjects(std::vector<Cube> &cubes) {
    srand48(13);

    for (int i = 0; i < 6; i++) {
        // position, radius, surface color, reflectivity, transparency, emission color
        float width = (std::rand() % 4) + 1;
        // TODO: Replace radius with transMat to fit Cube constructor
        cubes.push_back(width, Cube(glm::vec3( 0.0, -10004, -20), glm::vec3(0.20, 0.20, 0.20), 0, 0.0));
        /* Keep it here for sanity checks
        cubes.push_back(Cube(glm::vec3( 0.0,      0, -20),     4, glm::vec3(1.00, 0.32, 0.36), 1, 0.5));
        cubes.push_back(Cube(glm::vec3( 5.0,     -1, -15),     2, glm::vec3(0.90, 0.76, 0.46), 1, 0.0));
        cubes.push_back(Cube(glm::vec3( 5.0,      0, -25),     3, glm::vec3(0.65, 0.77, 0.97), 1, 0.0));
        cubes.push_back(Cube(glm::vec3(-5.5,      0, -15),     3, glm::vec3(0.90, 0.90, 0.90), 1, 0.0));
        */
    }
    // light (yes its initialized as a cube)
    cubes.push_back(cube(glm::vec3( 0.0,     20, -30),     3, glm::vec3(0.00, 0.00, 0.00), 0, 0.0, glm::vec3(3)));
}

int main(void)
{
    std::vector<Cube> cubes;

    GLFWwindow* window = initWindow();
    if (!window) {
        glfwTerminate();    
        return 1;
    }
    // TODO: initOctree();
    createObjects(cubes);
    mainLoop(window, cubes);
    return 0;
}

