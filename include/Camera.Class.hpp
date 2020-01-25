#ifndef CAMERA_CLASS_HPP
# define CAMERA_CLASS_HPP

# include <glm/glm.hpp>

// Adapted from Java implementation by Kai;
// source: https://github.com/LWJGL/lwjgl3-wiki/wiki/2.6.1.-Ray-tracing-with-OpenGL-Compute-Shaders-%28Part-I%29

class Camera
{
private:
	float _fn;
	float _ff;
	float _fl;
	float _fr;
	float _ft;
	float _fb;

	glm::vec3 _position;
	glm::vec3 _direction;
	glm::vec3 _up;
	glm::vec3 _right;

	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _invViewProjectionMatrix;

	bool _refreshViewMatrix;
	bool _refreshProjectionMatrix;
	bool _refreshInverseProjectionViewMatrix;

	bool orthographic;


	// TODO: Check if frustum is necessary (where is frustum culling impl'd ?)
	void setFrustumNear(float frustumNear);
	void setFrustumFar(float frustumFar);
	void setFrustumLeft(float frustumLeft);
	void setFrustumTop(float frustumTop);
	void setFrustumBottom(float frustumBottom);
            
public:
    Camera();
    ~Camera();
    Camera &operator=(Camera &rhs) = delete;
    Camera(Camera &rhs) = delete;

    void setDirection(glm::vec3 direction);
	void setPosition(glm::vec3 position);
	void setUp(glm::vec3 up);

    void doRefreshInverseProjectionViewMatrix();
	glm::mat4 getInverseProjectionViewMatrix();
	void doRefreshViewMatrix();
    void doRefreshProjectionMatrix();

	glm::vec3 getEyeRay(float x, float y);
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
	glm::vec3 getDirection();
	glm::vec3 getPosition();
    glm::vec3 getUp();
	glm::vec3 getRight();

    void setLookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);
	
	// TODO: Check if frustum is necessary (where is frustum culling impl'd ?)
	void setOrthographic(bool orthographic);
	void setFrustumRight(float frustumRight);
	void setFrustumPerspective(float fovY, float aspect, float near, float far, int tilesX, int tilesY,
			int tileX, int tileY);
	void setFrustum(float near, float far, float left, float right, float top, float bottom);
	void setFrustumPerspective(float fovY, float aspect, float near, float far);
	float getFrustumFar();
    float getFrustumLeft();
	float getFrustumRight();
	float getFrustumBottom();
	float getFrustumTop();
	bool isOrthographic();
};

#endif