#include "PhysicsManager.hpp"
#include "Scene.Class.hpp"
#include <glm/gtc/type_ptr.inl>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

PhysicsManager::PhysicsManager(float gravity_acceleration) {
    m_collision_config = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collision_config);
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_config);
    m_world->setGravity(btVector3(0, gravity_acceleration, 0));
}

PhysicsManager::~PhysicsManager() {
    for (auto body: m_btBoxes) {
        m_world->removeCollisionObject(body);
        btMotionState *state = body->getMotionState();
        btCollisionShape *shape = body->getCollisionShape();
        delete body;
        delete state;
        delete shape;
    }
    delete m_collision_config;
    delete m_dispatcher;
    delete m_broadphase;
    delete m_solver;
    delete m_world;
}

void PhysicsManager::addObjects(Scene &sc) {
    for (auto obj: sc.objects) {
        glm::vec4 origin = obj.c.transMat[3];
        this->addBox(origin.x, origin.y, origin.z, obj.mass, obj.c.halfSize);
    }
}

btRigidBody *PhysicsManager::addBox(float x, float y, float z, float mass, float size) {
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));

    btBoxShape *box = new btBoxShape(btVector3(size, size, size));
    btVector3 inertia(0, 0, 0);

    if (mass != 0.0) {
        box->calculateLocalInertia(mass, inertia);
    }

    btMotionState *state = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, state, box, inertia);
    btRigidBody *body = new btRigidBody(info);

    m_btBoxes.push_back(body);
    m_world->addRigidBody(body, 1, 1);
    return body;
}

void PhysicsManager::stickBox(btRigidBody *pBodyA, btRigidBody *pBodyB){
    btTransform frameInA, frameInB;
    frameInA = btTransform::getIdentity();
    frameInB = btTransform::getIdentity();
    frameInA.setOrigin(btVector3(1., 0., 0.));
    frameInB.setOrigin(btVector3(-1., 0., 0.));

    btFixedConstraint* pFixConstraint = new btFixedConstraint(*pBodyA, *pBodyB, frameInA, frameInB);
    m_world->addConstraint(pFixConstraint, true);
    return;
}

void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    // int numManifolds = world->getDispatcher()->getNumManifolds();
    // printf("numManifolds = %d\n",numManifolds);
    return;
}

void PhysicsManager::step(Scene &sc, double last_update) {
    for (unsigned long i = 0; i < sc.objects.size(); i++)
        this->updateBox(m_btBoxes[i], sc.objects[i].c);
    double step_time = (clock() - last_update) / (double) CLOCKS_PER_SEC;
    m_world->stepSimulation(step_time);
    m_world->setInternalTickCallback(myTickCallback);
}

void PhysicsManager::updateBox(btRigidBody *bt_box, Cube &cube) {
    btTransform t;
    // Get rotation and translation in quaternion form
    bt_box->getMotionState()->getWorldTransform(t);
    // Convert quaternion to transMat

    btScalar bt_transMat[16];
    t.getOpenGLMatrix(bt_transMat);
    cube.transMat = glm::make_mat4(bt_transMat);
}

btDynamicsWorld *PhysicsManager::getDynamicsWorld(){
    return m_world;
}