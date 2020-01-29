#define GLM_FORCE_CTOR_INIT
// #include "Octree.Class.hpp"
#include "OpenGL.Class.hpp"
#include "Raytracer.Class.hpp"

# define WIDTH 1280
# define HEIGHT 1024


int main(int ac, char **av)
{
    std::string         fn;
    
    if (ac > 1)
        fn = ac > 1 ? std::string(av[1]) : "untitled";

    std::vector<Cube>   cubes;
    // Octree oc(1000, 1000, 250, cubes);
    
    glm::vec3 pos(3.0f, 2.0f, 7.0f);
    glm::vec3 lookAt(0.f, 0.f, 1.f);
    glm::vec3 up(0.f, 1.f, 0.f);
    Camera cam(WIDTH, HEIGHT, 45.f, 0.1f, pos, lookAt, up);
    
    OpenGL gl(WIDTH, HEIGHT);

    Raytracer rt(cubes, &cam, &gl);
    
    rt.mainLoop();

    return 0;
}

