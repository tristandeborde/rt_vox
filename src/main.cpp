#define GLM_FORCE_CTOR_INIT
// #include "Octree.Class.hpp"
#include "OpenGL.Class.hpp"
#include "Raytracer.Class.hpp"
#include "PhysicsManager.hpp"
#include "SceneManager.Class.hpp"
#include <time.h>
#include <bullet/btBulletDynamicsCommon.h>

#define WIDTH 1280
#define HEIGHT 1024

void    mainLoop(Raytracer &rt, OpenGL &gl, Camera &cam, PhysicsManager &phys, SceneManager &sm){
    clock_t last_update = clock();

    GLFWwindow *win = gl.getWindow();

    sm.uploadScene();
    while (win && !glfwWindowShouldClose(win))
    {
        gl.updateInput();
        cam.update(gl, 0.1f);
        if (gl.isKeyPressed(GLFW_KEY_SPACE)) {
            std::cout << "Adding a cube..." << std::endl;

            // Add box to renderer
            glm::vec3 position = cam.getPos();
            auto o = sm.addBox(position[0], position[1], position[2]);
            
            // Add box to Bullet Physics
            glm::vec4 origin = o.c.transMat[3];
            // std::cout << "position[0]: " << o.c.transMat[3][0] << "; position[1]: " << o.c.transMat[3][1] << "; position[2]: " << o.c.transMat[3][2] << std::endl;
            
            phys.addBox(origin.x, origin.y, origin.z, o.mass, o.c.halfSize);
        }

        sm.uploadObjects();
        rt.render_GPU();
        // TODO: rasterize_objects();

        phys.step(sm.getScene(), last_update);
        glfwSwapBuffers(win);
        last_update = clock();
    }

    // Clear all allocated GLFW resources.
    glfwTerminate();
}

int main(int ac, char **av)
{
    // Octree oc(1000, 1000, 250, cubes);
    
    // Create Camera manager
    glm::vec3 pos(3.0f, 2.0f, 7.0f);
    glm::vec3 lookAt(0.f, 0.f, 1.f);
    glm::vec3 up(0.f, 1.f, 0.f);
    Camera cam(WIDTH, HEIGHT, 45.f, 0.1f, pos, lookAt, up);
    
    // Create OpenGL manager
    OpenGL gl(WIDTH, HEIGHT);

    // Create Raytracer
    Raytracer rt(&cam, &gl); 

    // Scene Manager
    SceneManager sm(rt.getComputeShaderID());
    sm.readScene(); // TODO: parser to read .obj files directly
    
    // Real g: -9.80665
    PhysicsManager phys(-1.f);
    phys.addObjects(sm.getScene());

    mainLoop(rt, gl, cam, phys, sm);
    return 0;
}
