#ifndef SCENEMANAGER_HPP
# define SCENEMANAGER_HPP

#include <time.h>
#include "Camera.Class.hpp"
#include "RenderingManager.hpp"
#include "PhysicsManager.hpp"

class SceneManager
{
public:
    SceneManager(Camera &cam, PhysicsManager &pm, RenderingManager &rm, SceneManager &sm);
    SceneManager() = delete;
    ~SceneManager();
    SceneManager &operator=(SceneManager &src) = delete;

    SceneManager(SceneManager &src) = delete;
    void addBox(clock_t last_update);
    Scene &getScene();
    void readScene();

private:
    Camera &m_cam;
    PhysicsManager &m_pm;
    RenderingManager &m_rm;
    SceneManager &m_sm;

    Scene m_sc;
    glm::mat4 setCenter(float x, float y, float z);

};

#endif
