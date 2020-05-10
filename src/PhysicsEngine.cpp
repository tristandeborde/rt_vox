#include "PhysicsEngine.hpp"
#include "Scene.Class.hpp"
#include <glm/gtc/type_ptr.inl>
#include <iostream>

PhysicsEngine::PhysicsEngine(float gravity_acceleration) {
    m_collision_config = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collision_config);
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_config);
    m_world->setGravity(btVector3(0, gravity_acceleration, 0));
}

PhysicsEngine::~PhysicsEngine() {
    delete m_collision_config;
    delete m_dispatcher;
    delete m_broadphase;
    delete m_solver;
    delete m_world;
}

void PhysicsEngine::addObjects(Scene &sc) {
    for (auto obj: sc.objects) {
        glm::vec4 origin = obj.c.transMat[3];
        // TODO: choose mass from "Object" instance (for now its just "Cube")
        m_btBoxes.push_back(addBox(origin.x, origin.y, origin.z, 1));
    }
}

btRigidBody *PhysicsEngine::addBox(float x, float y, float z, float mass) {
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));

    btBoxShape *box = new btBoxShape(btVector3(1.0, 1.0, 1.0));
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0)
        box->calculateLocalInertia(mass, inertia);

    btMotionState *state = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, state, box, inertia);
    btRigidBody *body = new btRigidBody(info);
    return body;
}

void PhysicsEngine::step(Scene &sc, double last_update) {
    for (unsigned long i = 0; i < sc.objects.size(); i++)
        this->updateBox(m_btBoxes[i], sc.objects[i].c);
    double step_time = (clock() - last_update) / (double) CLOCKS_PER_SEC;
    m_world->stepSimulation(step_time);
}

void PhysicsEngine::updateBox(btRigidBody *bt_box, Cube &cube) {
    btTransform t;
    // Get rotation and translation in quaternion form
    bt_box->getMotionState()->getWorldTransform(t);
    // Convert quaternion to transMat
    btScalar bt_transMat[16]; 
    t.getOpenGLMatrix(bt_transMat);
    cube.transMat = glm::make_mat4(bt_transMat);
}

// TODO: 1/ Do renderBox, or should I say updateBox ? (https://youtu.be/1CEI2pOym1Y?t=2104)
// To do that, replace OpenGL Model view matmul, by simple quaternion (t) to Matrix.
// This matrix is used to modify the given cube.transMat in Cubes. 
// => No actual rendering ! just modifying the cube.transMat, rendering is done in rt.comp

// TODO: 3/ Add a small function to spawn box on spacebar.