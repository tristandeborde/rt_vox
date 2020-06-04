#define GLM_FORCE_CTOR_INIT
// #include "Octree.Class.hpp"
#include "OpenGL.Class.hpp"
#include "Raytracer.Class.hpp"
#include "PhysicsManager.hpp"
#include "PhysicsDebugDrawer.hpp"
#include "ShaderDebug.hpp"
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
    PhysicsDebugDrawer ph_debug;
    ShaderDebug sh_debug;

    rm.uploadScene(sm.getScene());
    
    while (win && !glfwWindowShouldClose(win))
    {
        last_update = clock();
        gl.updateInput();
        cam.update(gl, 0.1f);
        if (gl.isKeyPressed(GLFW_KEY_SPACE) && last_update - last_key_press > MIN_INPUT_DELTA) {
            std::cout << "Adding a cube at delta = " << last_update - last_key_press << std::endl;
            last_key_press = last_update;
            sm.addBox(last_update, cam.getLookDir());
        }

        rm.uploadObjects(sm.getScene());
        pm.step(sm.getScene(), last_update);
        if (DEBUG){
            pm.getDynamicsWorld()->debugDrawWorld();
            
            glm::mat4 Projection = cam.getProjMat(); 
            glm::mat4 View = cam.getViewMat();
            sh_debug.use();
            GLint MatrixID =	glGetUniformLocation(sh_debug.getID(), "MVP"); // use the MVP in the simple shader
            // // make the View and  Projection matrix
            glm::mat4 VP = Projection * View;  // Remember order seems backwards
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &VP[0][0]);
            
            ph_debug.DoDebugDraw();
        }
        else
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
    Camera cam(WIDTH, HEIGHT, 45.f, 0.1f, pos, lookAt, up);
    
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
