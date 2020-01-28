#ifndef CAMERA_CLASS_HPP
# define CAMERA_CLASS_HPP

# include "OpenGL.Class.hpp"
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(int width, int height, float fovY, float sensitivity,
           glm::vec3 &pos, glm::vec3 &lookAt, glm::vec3 &up);

    int getWidth() const;
    int getHeight() const;
    float getFovY() const;
    float getFovX() const;
    const glm::vec3& getPos() const;
    const glm::vec3& getLookDir() const;
    const glm::vec3& getUp() const;
    const glm::vec3& getRight() const;

    glm::vec3 getEyeRay(uint x, uint y);
    void update(OpenGL &input, float dt);

private:
    int	_width = 1280;
    int _height = 1024;
    float _fovY = 0.f;
    float _fovX = 0.f;
    float _accRotX = 0.f;
    float _accRotY = 0.f;
    float _sensitivity  = 1.f;
    float _movSpeed = 10.f;

    glm::vec3 _pos{ 0.f, 0.f, 0.f };
    glm::vec3 _lookDir{ 0.f, 0.f, -1.f };
    glm::vec3 _up{ 0.f, 1.f, 0.f };
    glm::vec3 _right{ 1.f, 0.f, 0.f };
};

#endif