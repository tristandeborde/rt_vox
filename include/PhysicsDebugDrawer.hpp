#pragma once

#include <bullet/LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

struct lineValues
{
	btVector3 p1;
	btVector3 p2;
		
};

class PhysicsDebugDrawer : public btIDebugDraw
{
	DebugDrawModes 			mDebugMode;
	std::vector<lineValues> m_lines;

public:
	PhysicsDebugDrawer();
	virtual ~PhysicsDebugDrawer();

	void DoDebugDraw();

	virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor);

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {};

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {};

	virtual void reportErrorWarning(const char* warningString) {};

	virtual void draw3dText(const btVector3& location, const char* textString) {};

	virtual void setDebugMode(int debugMode);

	virtual int getDebugMode() const;
};
