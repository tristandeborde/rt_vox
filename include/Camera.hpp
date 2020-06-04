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
    glm::mat4 getProjMat() ;
    glm::mat4 getViewMat() ;
    const glm::vec3& getPos() const;
    glm::vec3 getPosNonRef() const;
    const glm::vec3& getLookDir() const;
    glm::vec3 getLookDirNonRef() const;
    const glm::vec3& getUp() const;
    glm::vec3 getUpNonRef() const;
    const glm::vec3& getRight() const;

    glm::vec3 getEyeRay(uint x, uint y);
    void update(OpenGL &input, float dt);

private:
    int	m_width = 1280;
    int m_height = 1024;
    float m_fovY = 0.f;
    float m_fovX = 0.f;
    float m_accRotX = 0.f;
    float m_accRotY = 0.f;
    float m_sensitivity  = 1.f;
    float m_movSpeed = 10.f;

    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
    glm::vec3 m_lookDir{ 0.f, 0.f, -1.f };
    glm::vec3 m_up{ 0.f, 1.f, 0.f };
    glm::vec3 m_right{ 1.f, 0.f, 0.f };
};

#endif