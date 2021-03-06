#ifndef SCENEMANAGER_HPP
# define SCENEMANAGER_HPP

#include <time.h>
#include <utility>
#include "Camera.hpp"
#include "RenderingManager.hpp"
#include "PhysicsManager.hpp"


struct Ray{
    glm::vec3 origin;
    glm::vec3 dir;
};

enum class Planes {
    left = 0,
    right,
    top,
    bottom,
    front,
    back
};

struct PlaneInfo {
    uint    ax_index;
    float   center_proj;
    float   ray_proj;
    float   min_intersect;
    float   max_intersect;
    Planes  plane;
};

typedef std::pair<std::vector<Object>::iterator, Planes> PlaneHitInfo;
class SceneManager
{
public:
    SceneManager(Camera &cam, PhysicsManager &pm, RenderingManager &rm, SceneManager &sm);
    SceneManager() = delete;
    ~SceneManager();
    SceneManager &operator=(SceneManager &src) = delete;
    SceneManager(SceneManager &src) = delete;

    void selectPlane(void);
    void stickBox(clock_t last_update, const glm::vec3 &look);
    void addBox(clock_t last_update, const glm::vec3 &look);
    void addCompositeBox(clock_t last_update, const glm::vec3 &look);
    Scene &getScene();
    void readScene();

private:
    Camera              &m_cam;
    PhysicsManager      &m_pm;
    RenderingManager    &m_rm;
    SceneManager        &m_sm;
    int                 m_selection_mat_idx;
    int                 m_prev_selection_mat_idx;
    PlaneHitInfo        m_selected_plane;
    Scene               m_sc;

    glm::mat4 setCenter(float x, float y, float z);
    Ray initRay(uint x, uint y);
    bool intersectPlanes(const glm::vec3 &axis, std::vector<Object>::iterator it, PlaneInfo &p_info);
    PlaneInfo intersectBox(Ray &r, std::vector<Object>::iterator it);
    PlaneHitInfo RaycastBoxes(Ray r);

};

#endif
