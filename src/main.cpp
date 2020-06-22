#define GLM_FORCE_CTOR_INIT
#include "OpenGL.Class.hpp"
#include "Raytracer.Class.hpp"
#include "PhysicsManager.hpp"
#include "RenderingManager.hpp"
#include "SceneManager.hpp"
#include <time.h>
#include <bullet/btBulletDynamicsCommon.h>

#define WIDTH 1280
#define HEIGHT 1024
#define MIN_INPUT_DELTA 120000
#define DEBUG false

void    mainLoop(Raytracer &rt, OpenGL &gl, Camera &cam, PhysicsManager &pm, RenderingManager &rm, SceneManager &sm){
    clock_t last_update = clock();
    clock_t last_key_press = last_update;
    
    GLFWwindow *win = gl.getWindow();

    rm.uploadScene(sm.getScene());
    while (win && !glfwWindowShouldClose(win))
    {
        last_update = clock();
        gl.updateInput();
        cam.update(gl, 0.1f);

        if (last_update - last_key_press > MIN_INPUT_DELTA) {
            if (gl.isKeyPressed(GLFW_KEY_SPACE)) {
                std::cout << "Adding CUBE at delta = " << last_update - last_key_press << std::endl;
                sm.addBox(last_update, cam.getLookDir());
            }
            else if (gl.isKeyPressed(GLFW_KEY_C)) {
                std::cout << "Adding COMPOSITE at delta = " << last_update - last_key_press << std::endl;
                sm.addCompositeBox(last_update, cam.getLookDir());
            }
            else if (gl.isKeyPressed(GLFW_KEY_V)) {
                std::cout << "Sticking CUBE at delta = " << last_update - last_key_press << std::endl;
                sm.stickBox(last_update, cam.getLookDir());
            }
            else if (gl.isKeyPressed(GLFW_KEY_ESCAPE)) {
                std::cout << "Exiting..." << std::endl;
                break;
            }
            last_key_press = last_update;
        }
        sm.selectPlane();

        rm.uploadObjects(sm.getScene());
        pm.step(sm.getScene(), last_update);
        rt.render_GPU();
        glfwSwapBuffers(win);
    }

    glfwTerminate();
}

int main(int ac, char **av)
{
    // Octree oc(1000, 1000, 250, cubes);
    
    // Create Camera manager
    glm::vec3 pos(3.0f, 2.0f, 7.0f);
    glm::vec3 lookAt(0.f, 0.f, 1.f);
    glm::vec3 up(0.f, 1.f, 0.f);
    Camera cam(WIDTH, HEIGHT, 45.f, 0.05f, pos, lookAt, up);
    
    // Create OpenGL manager
    OpenGL gl(WIDTH, HEIGHT);

    // Create Raytracer
    Raytracer rt(&cam, &gl); 

    // Create Rendering Manager
    RenderingManager rm(rt.getComputeShaderID());
    
    // Real g: -9.80665
    PhysicsManager pm(-5.f);

    // Create SceneManager
    SceneManager sm(cam, pm, rm, sm);
    sm.readScene(); // TODO: parser to read .obj files directly

    pm.addObjects(sm.getScene());

    mainLoop(rt, gl, cam, pm, rm, sm);
    return 0;
}
