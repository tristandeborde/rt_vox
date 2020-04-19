#ifndef PHYSICS_ENGINE_HPP
# define PHYSICS_ENGINE_HPP

# include <bullet/btBulletDynamicsCommon.h>
# include <glm/glm.hpp>
# include "Scene.Class.hpp"

class PhysicsEngine {
private:
    btCollisionConfiguration *m_collision_config;
    btDispatcher *m_dispatcher;
    btBroadphaseInterface *m_broadphase;
    btConstraintSolver *m_solver;
    btDynamicsWorld *m_world;
    std::vector<btRigidBody *> m_btBoxes;

    void updateBox(btRigidBody *bt_box, Cube &cube);
    btRigidBody *addBox(float x, float y, float z, float mass);

public:
    PhysicsEngine(float gravity_acceleration);
    PhysicsEngine() = delete;
    ~PhysicsEngine();
    PhysicsEngine(PhysicsEngine &src) = delete;
    PhysicsEngine &operator=(PhysicsEngine &src) = delete;

    void step(Scene &sc, double last_update);
    void addObjects(Scene &sc);
};

#endif