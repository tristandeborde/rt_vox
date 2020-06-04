#include "Camera.hpp"

Camera::Camera(int width, int height, float fovY, float sensitivity,
           glm::vec3 &pos, glm::vec3 &lookAt, glm::vec3 &up)
    :   m_width(width),
        m_height(height),
        m_sensitivity(sensitivity),
        m_pos(pos)
{
    m_lookDir = glm::normalize(lookAt - pos);
    m_right = glm::normalize(glm::cross(up, m_lookDir));
    m_up = glm::cross(m_right, m_lookDir);

    m_fovY = std::tan(fovY * std::acos(-1) / 180.f / 2.0f);
    m_fovX = (static_cast<float>(m_width) * m_fovY)/static_cast<float>(m_height);
}

void Camera::update(OpenGL &input, float dt)
{
    float dz, dx, dy;
    dz = dx = dy = 0.f;

    if (input.isKeyPressed(GLFW_KEY_W)) {
        dz = m_sensitivity * dt;
    } else if (input.isKeyPressed(GLFW_KEY_S)) {
        dz = -1.f * m_sensitivity * dt;
    }

    if (input.isKeyPressed(GLFW_KEY_D)) {
        dx = m_sensitivity * dt;
    } else if (input.isKeyPressed(GLFW_KEY_A)) {
        dx = -1.f * m_sensitivity * dt;
    }

    m_accRotX += input.getYPosDiff() * m_sensitivity * dt;
    m_accRotY += input.getXPosDiff() * m_sensitivity * dt;
    m_accRotY = std::fmod(m_accRotY, 360);
    m_accRotX = std::fmod(m_accRotX, 360);

    auto rotX = glm::rotate(glm::mat4(1.f), m_accRotX, glm::vec3(1.f, 0.f, 0.f));
    auto rotY = glm::rotate(glm::mat4(1.f), m_accRotY, glm::vec3(0.f, 1.f, 0.f));

    auto rotMatrix = rotY * rotX;

    m_lookDir  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    m_up  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    m_right = glm::normalize(glm::cross(m_lookDir, m_up));

    m_pos += m_movSpeed * ( dz * m_lookDir + dx * m_right);
}

glm::vec3 Camera::getEyeRay(uint x, uint y)
{
    float halfWidth = float(this->m_width) / 2.0f;
    float halfHeight = float(this->m_height) / 2.0f;

    float a = this->m_fovX * ((float(x) - halfWidth + 0.5f) / halfWidth);
    float b = this->m_fovY * ((halfHeight - float(y) - 0.5f) / halfHeight);

    glm::vec3 ray = glm::normalize(a * this->m_right + b * this->m_up + this->m_lookDir);

    return (ray);
}

glm::mat4 Camera::getViewMat() {
    return (glm::lookAt(
        getPosNonRef(), 
  		getLookDirNonRef(), 
  		getUpNonRef()
    ));
}

glm::mat4 Camera::getProjMat() {
    return (glm::perspective(
        glm::radians(45.0f),
        (float)m_width/(float)m_height,
        0.1f,
        100.0f
    ));
}

int Camera::getWidth() const
{
    return (this->m_width);
}

int Camera::getHeight() const
{
    return (this->m_height);
}

float Camera::getFovY() const
{
    return (this->m_fovY);
}

float Camera::getFovX() const
{
    return (this->m_fovX);
}

const glm::vec3& Camera::getPos() const
{
    return (this->m_pos);
}

glm::vec3 Camera::getPosNonRef() const
{
    return (this->m_pos);
}


const glm::vec3& Camera::getLookDir() const
{
    return (this->m_lookDir);
}

glm::vec3 Camera::getLookDirNonRef() const
{
    return (this->m_lookDir);
}

const glm::vec3& Camera::getUp() const
{
    return (this->m_up);
}

glm::vec3 Camera::getUpNonRef() const
{
    return (this->m_up);
}

const glm::vec3& Camera::getRight() const
{
    return (this->m_right);
}