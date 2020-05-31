#ifndef PHYSICS_ENGINE_HPP
# define PHYSICS_ENGINE_HPP

# include <bullet/btBulletDynamicsCommon.h>
# include <glm/glm.hpp>
# include "Scene.Class.hpp"

// class CustomMotionState : public btDefaultMotionState {
//     ~CustomMotionState() = default;
//     void setWorldTransform(const btTransform& worldTrans) const;
// };

class PhysicsManager {
private:
    btCollisionConfiguration *m_collision_config;
    btDispatcher *m_dispatcher;
    btBroadphaseInterface *m_broadphase;
    btConstraintSolver *m_solver;
    btDynamicsWorld *m_world;
    std::vector<btRigidBody *> m_btBoxes;

    void updateBox(btRigidBody *bt_box, Cube &cube);
public:
    void addBox(float x, float y, float z, float mass, float size);

public:
    PhysicsManager(float gravity_acceleration);
    PhysicsManager() = delete;
    ~PhysicsManager();
    PhysicsManager(PhysicsManager &src) = delete;
    PhysicsManager &operator=(PhysicsManager &src) = delete;

    void step(Scene &sc, double last_update);
    void addObjects(Scene &sc);
};

#endif