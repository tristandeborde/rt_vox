#include "Camera.Class.hpp"

Camera::Camera(int width, int height, float fovY, float sensitivity,
           glm::vec3 &pos, glm::vec3 &lookAt, glm::vec3 &up)
    :   _width(width),
        _height(height),
        _sensitivity(sensitivity),
        _pos(pos)
{
    _lookDir = glm::normalize(lookAt - pos);
    _right = glm::normalize(glm::cross(up, _lookDir));
    _up = glm::cross(_right, _lookDir);

    _fovY = std::tan(fovY * std::acos(-1) / 180.f / 2.0f);
    _fovX = (static_cast<float>(_width) * _fovY)/static_cast<float>(_height);
}

void Camera::update(OpenGL &input, float dt)
{
    float dz, dx, dy;
    dz = dx = dy = 0.f;

    if (input.isKeyPressed(GLFW_KEY_W)) {
        dz = _sensitivity * dt;
    } else if (input.isKeyPressed(GLFW_KEY_S)) {
        dz = -1.f * _sensitivity * dt;
    }

    if (input.isKeyPressed(GLFW_KEY_D)) {
        dx = _sensitivity * dt;
    } else if (input.isKeyPressed(GLFW_KEY_A)) {
        dx = -1.f * _sensitivity * dt;
    }

    _accRotY += input.getXPosDiff() * _sensitivity * dt;
    _accRotX += input.getYPosDiff() * _sensitivity * dt;
    _accRotY = std::fmod(_accRotY, 360);
    _accRotX = std::fmod(_accRotX, 360);

    auto rotX = glm::rotate(glm::mat4(1.f), _accRotX, glm::vec3(1.f, 0.f, 0.f));
    auto rotY = glm::rotate(glm::mat4(1.f), _accRotY, glm::vec3(0.f, 1.f, 0.f));

    auto rotMatrix = rotY * rotX;

    _lookDir  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    _up  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    _right = glm::normalize(glm::cross(_lookDir, _up));

    _pos += _movSpeed * ( dz * _lookDir + dx * _right);
}

glm::vec3 Camera::getEyeRay(uint x, uint y)
{
    float halfWidth = float(this->_width) / 2.0f;
    float halfHeight = float(this->_height) / 2.0f;

    float a = this->_fovX * ((float(x) - halfWidth + 0.5f) / halfWidth);
    float b = this->_fovY * ((halfHeight - float(y) - 0.5f) / halfHeight);

    glm::vec3 ray = glm::normalize(a * this->_right + b * this->_up + this->_lookDir);

    return (ray);
}

int Camera::getWidth() const
{
    return (this->_width);
}

int Camera::getHeight() const
{
    return (this->_height);
}

float Camera::getFovY() const
{
    return (this->_fovY);
}

float Camera::getFovX() const
{
    return (this->_fovX);
}

const glm::vec3& Camera::getPos() const
{
    return (this->_pos);
}

const glm::vec3& Camera::getLookDir() const
{
    return (this->_lookDir);
}

const glm::vec3& Camera::getUp() const
{
    return (this->_up);
}

const glm::vec3& Camera::getRight() const
{
    return (this->_right);
}