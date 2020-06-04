#include "PhysicsDebugDrawer.hpp"

PhysicsDebugDrawer::PhysicsDebugDrawer()
{
	mDebugMode = (DebugDrawModes)(DebugDrawModes::DBG_DrawAabb | DebugDrawModes::DBG_FastWireframe);
}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{
}

void PhysicsDebugDrawer::DoDebugDraw()
{
	// set up the line shader and then draw the buffer
	unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_lines), &m_lines[0], GL_STATIC_DRAW);


	//load the vertex data info
	glVertexAttribPointer(0,  // cause location = 0 for aPos in shader
		3,	// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		4*sizeof(float),  // how many floats to the next one(be aware btVector3 uses 4 floats)
		(GLfloat*)&m_lines[0]  // where do they start as an index); // use 3 values, but add stride each time to get to the next
	);

	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, m_lines.size()*2);
	m_lines.clear();
}

void PhysicsDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor)
{
	// draws a simple line of pixels between points but stores them for later draw
	lineValues line;
	line.p1 = from;
	line.p2 = to;

	m_lines.push_back(line);
}

void PhysicsDebugDrawer::setDebugMode(int debugMode)
{
	mDebugMode = DebugDrawModes(debugMode);
}

int PhysicsDebugDrawer::getDebugMode() const
{
	return mDebugMode;
}