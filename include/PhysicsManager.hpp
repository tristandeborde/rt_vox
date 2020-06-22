#pragma once

# include <bullet/btBulletDynamicsCommon.h>
# include <bullet/LinearMath/btVector3.h>
# include <bullet/LinearMath/btAlignedObjectArray.h>
# include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
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
    btRigidBody *addBox(float x, float y, float z, float mass, float size);
    void stickBox(btRigidBody *pbodyA, btRigidBody *pbodyB, btVector3 &offset);
    btRigidBody *getBox(int index);

    PhysicsManager(float gravity_acceleration);
    PhysicsManager() = delete;
    ~PhysicsManager();
    PhysicsManager(PhysicsManager &src) = delete;
    PhysicsManager &operator=(PhysicsManager &src) = delete;

    void step(Scene &sc, double last_update);
    void addObjects(Scene &sc);
    btDynamicsWorld *getDynamicsWorld();
};
