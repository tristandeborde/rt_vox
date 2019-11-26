#define GLM_FORCE_CTOR_INIT
#include "rt_vox.hpp"
#include "Octree.Class.hpp"
#include "Raytracer.Class.hpp"

void    mainLoop(GLFWwindow* window, std::vector<Cube> cubes) {
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // render(cubes);
        // TODO: rasterize_objects();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clear all allocated GLFW resources.
    glfwTerminate();
}

void    createObjects(std::vector<Cube> &cubes) {
    srand48(42);
    float width = (std::rand() % 4) + 1;
    glm::mat4 transMat = glm::mat4();
    transMat[3] = {0, 0.2, -20, 1};
    cubes.push_back(Cube(width, transMat, glm::vec3(1.00, 0.32, 0.36), 1, 0.5, glm::vec3(0)));

    transMat[3] = {5.0, -1, -15, 1};
    cubes.push_back(Cube(width, transMat, glm::vec3(0.90, 0.76, 0.46), 1, 0.0, glm::vec3(0)));

    transMat[3] = {5.0, 0, -25, 1};
    cubes.push_back(Cube(width, transMat, glm::vec3(0.65, 0.77, 0.97), 1, 0.0, glm::vec3(0)));

    transMat[3] = {-5.5, 0, -15, 1};
    cubes.push_back(Cube(width, transMat, glm::vec3(0.90, 0.90, 0.90), 1, 0.0, glm::vec3(0)));

    /* Automated creation for later
    for (int i = 0; i < 6; i++) {
        width = (std::rand() % 4) + 1;
        transMat = glm::mat4();
        transMat[3] = {std::rand() % 6, std::rand() % 2, std::rand() % 5 + 15, 1};
        //                  width, transMat, surface color, reflectivity, transparency, emission color
        cubes.push_back(Cube(width, transMat, glm::vec3(0.20, 0.20, 0.20), 1, 0.0, glm::vec3(0)));
    }*/

    // light (yes its initialized as a cube)
    transMat[3] = {0.0, 20, -30, 1};
    cubes.push_back(Cube(3, transMat, glm::vec3(0.00, 0.00, 0.00), 0, 0.0, glm::vec3(3)));
}

int main(int ac, char **av)
{
    std::vector<Cube>   cubes;
    Raytracer           rt(cubes);
    std::string         fn;
    
    fn = ac > 1 ? std::string(av[1]) : "untitled";

    GLFWwindow* window = initWindow();
    if (!window) {
        glfwTerminate();    
        return 1;
    }

    createObjects(cubes);
    
    Octree oc(1000, 1000, 250, cubes);
    
    rt.render(fn);
    // mainLoop(window, cubes);
    return 0;
}

