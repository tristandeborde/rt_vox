#include "Camera.Class.hpp"
#include <cmath>
#include <stdexcept>
#include <string>

Camera::Camera() {
    this->_fn = 0.001f;
	this->_ff = 10.0f;
	this->_fl = -0.05f;
	this->_fr = 0.05f;
	this->_ft = 0.05f;
	this->_fb = -0.05f;

	this->_position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->_right = glm::vec3(1.0f, 0.0f, 0.0f);

    this->_projectionMatrix = glm::mat4();
	this->_viewMatrix = glm::mat4();
	this->_invViewProjectionMatrix = glm::mat4();

    this->_refreshViewMatrix = true;
	this->_refreshProjectionMatrix = true;
	this->_refreshInverseProjectionViewMatrix = true;
}

Camera::~Camera() {
    return;
}

glm::vec3 Camera::getEyeRay(float x, float y) {
    // Compute corner ray, by transforming from 2d clipping point to 3d world space 
    glm::vec4 tmp(x, y, 0.0f, 1.0f);
    tmp = getInverseProjectionViewMatrix() * tmp;
    tmp /= tmp.w;
    return (glm::vec3(tmp.x, tmp.y, tmp.z));
}

glm::mat4 Camera::getInverseProjectionViewMatrix() {
    if (_refreshInverseProjectionViewMatrix) {
        doRefreshInverseProjectionViewMatrix();
    }
    return _invViewProjectionMatrix;
}

void Camera::doRefreshInverseProjectionViewMatrix() {
    _invViewProjectionMatrix = getProjectionMatrix();
    _invViewProjectionMatrix = _invViewProjectionMatrix * getViewMatrix();
    _invViewProjectionMatrix = glm::inverse(_invViewProjectionMatrix);
    _refreshInverseProjectionViewMatrix = false;
}

void Camera::doRefreshProjectionMatrix() {
    // Reset projection matrix
    if ( !orthographic ) {
        this->_projectionMatrix[0][0] = 2.0f * _fn / (_fr - _fl);
        this->_projectionMatrix[0][1] = 0.0f;
        this->_projectionMatrix[0][2] = (_fr + _fl) / (_fr - _fl); // zero if symmetric
        this->_projectionMatrix[0][3] = 0.0f;
        this->_projectionMatrix[1][0] = 0.0f;
        this->_projectionMatrix[1][1] = 2.0f * _fn / (_ft - _fb);
        this->_projectionMatrix[1][2] = (_ft + _fb) / (_ft - _fb); // zero if symmetric
        this->_projectionMatrix[1][3] = 0.0f;
        this->_projectionMatrix[2][0] = 0.0f;
        this->_projectionMatrix[2][1] = 0.0f;
        this->_projectionMatrix[2][2] = -(_ff + _fn) / (_ff - _fn);
        this->_projectionMatrix[2][3] = -2.0f * _ff * _fn / (_ff - _fn);
        this->_projectionMatrix[3][0] = 0.0f;
        this->_projectionMatrix[3][1] = 0.0f;
        this->_projectionMatrix[3][2] = -1.0f;
        this->_projectionMatrix[3][3] = 0.0f;
    } else {
        this->_projectionMatrix[0][0] = 2.0f / (_fr - _fl);
        this->_projectionMatrix[0][1] = 0.0f;
        this->_projectionMatrix[0][2] = 0.0f;
        this->_projectionMatrix[0][3] = -(_fr + _fl) / (_fr - _fl); // zero if symmetric
        this->_projectionMatrix[1][0] = 0.0f;
        this->_projectionMatrix[1][1] = 2.0f / (_ft - _fb);
        this->_projectionMatrix[1][2] = 0.0f;
        this->_projectionMatrix[1][3] = -(_ft + _fb) / (_ft - _fb); // zero if symmetric
        this->_projectionMatrix[2][0] = 0.0f;
        this->_projectionMatrix[2][1] = 0.0f;
        this->_projectionMatrix[2][2] = -2.0f / (_ff - _fn);
        this->_projectionMatrix[2][3] = -(_ff + _fn) / (_ff - _fn);
        this->_projectionMatrix[3][0] = 0.0f;
        this->_projectionMatrix[3][1] = 0.0f;
        this->_projectionMatrix[3][2] = 0.0f;
        this->_projectionMatrix[3][3] = 1.0f;
    }
    _refreshProjectionMatrix = false;
}

void Camera::doRefreshViewMatrix() {
    // _dir and _up could be normalized in setters for speed + clarity
    glm::vec3 tmp_dir = glm::normalize(this->_direction); 
    glm::vec3 tmp_up = glm::normalize(this->_up); // no tilt assumed so member is queried

    // We recompute _right, because setters (like _dir's) don't do it
    // _right = _direction x _up
    glm::vec3 tmp_right = glm::cross(tmp_dir, tmp_up);

    this->_viewMatrix[0][0] = tmp_right.x;
    this->_viewMatrix[0][1] = tmp_right.y;
    this->_viewMatrix[0][2] = tmp_right.z;
    this->_viewMatrix[0][3] = -tmp_right.x * this->_position.x - tmp_right.y * this->_position.y - tmp_right.z * this->_position.z;
    this->_viewMatrix[1][0] = tmp_up.x;
    this->_viewMatrix[1][1] = tmp_up.y;
    this->_viewMatrix[1][2] = tmp_up.z;
    this->_viewMatrix[1][3] = -tmp_up.x * this->_position.x - tmp_up.y * this->_position.y - tmp_up.z * this->_position.z;
    this->_viewMatrix[2][0] = -tmp_dir.x;
    this->_viewMatrix[2][1] = -tmp_dir.y;
    this->_viewMatrix[2][2] = -tmp_dir.z;
    this->_viewMatrix[2][3] = tmp_dir.x * this->_position.x + tmp_dir.y * this->_position.y + tmp_dir.z * this->_position.z;
    this->_viewMatrix[3][0] = 0.0f;
    this->_viewMatrix[3][1] = 0.0f;
    this->_viewMatrix[3][2] = 0.0f;
    this->_viewMatrix[3][3] = 1.0f;
    _refreshViewMatrix = false;
}

// Setters ****************************************************************
// ************************************************************************

void Camera::setLookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) {
    // Set pos
    this->setPosition(position);
    // Set up
    this->setUp(glm::normalize(up));
    // Set dir
    this->setDirection(glm::normalize(lookAt - position));
    // Set right = _direction x _up
    this->_right = glm::normalize(glm::cross(this->_direction, this->_up));

    // this->_up = glm::cross(this->_right, tmp1); Why ?? Sanity check I guess
}

void Camera::setDirection(glm::vec3 _direction) {
    if (this->_direction == (_direction)) {
        return;
    }
    this->_direction = _direction;
    _refreshViewMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setPosition(glm::vec3 _position) {
    if (this->_position == (_position)) {
        return;
    }
    this->_position = (_position);
    _refreshViewMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setUp(glm::vec3 _up) {
    if (this->_up == (_up)) {
        return;
    }
    this->_up = (_up);
    _refreshViewMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumNear(float frustumNear) {
    if (this->_fn == frustumNear) {
        return;
    }
    this->_fn = frustumNear;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumFar(float frustumFar) {
    if (this->_ff == frustumFar) {
        return;
    }
    this->_ff = frustumFar;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumLeft(float frustumLeft) {
    if (this->_fl == frustumLeft) {
        return;
    }
    this->_fl = frustumLeft;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumTop(float frustumTop) {
    if (this->_ft == frustumTop) {
        return;
    }
    this->_ft = frustumTop;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumBottom(float frustumBottom) {
    if (this->_fb == frustumBottom) {
        return;
    }
    this->_fb = frustumBottom;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setOrthographic(bool orthographic) {
    if (this->orthographic == orthographic) {
        return;
    }
    this->orthographic = orthographic;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumRight(float frustumRight) {
    if (this->_fr == frustumRight) {
        return;
    }
    this->_fr = frustumRight;
    _refreshProjectionMatrix = true;
    _refreshInverseProjectionViewMatrix = true;
}

void Camera::setFrustumPerspective(float fovY, float aspect, float near, float far, int tilesX, int tilesY,
        int tileX, int tileY) {
    if (std::isnan(aspect) || std::isinf(aspect)) {
        throw std::invalid_argument("Illegal value for aspect [" + std::to_string(aspect) + "]");
    }
    if (near >= far) {
        throw std::invalid_argument("The passed value for near [" + std::to_string(near) + "] must be greater than far ["
                + std::to_string(far) + "]");
    }
    if (tileX < 0 || tileX >= tilesX || tileY < 0 || tileY >= tilesY) {
        throw std::invalid_argument(
                "The tile indices must be at least 0 and at most the total number of tiles minus 1");
    }
    setOrthographic(false);
    float h = (float) std::tan(fovY*M_PI/180.0 * 0.5f) * near;
    float w = h * aspect;
    float left = -w + (float) tileX / tilesX * 2.0f * w;
    float right = left + (float) 1.0f / tilesX * 2.0f * w;
    float bottom = -h + (float) tileY / tilesY * 2.0f * h;
    float top = bottom + (float) 1.0f / tilesY * 2.0f * h;
    setFrustumLeft(left);
    setFrustumRight(right);
    setFrustumBottom(bottom);
    setFrustumTop(top);
    setFrustumNear(near);
    setFrustumFar(far);
}

void Camera::setFrustum(float near, float far, float left, float right, float top, float bottom) {
    setOrthographic(true);
    setFrustumNear(near);
    setFrustumFar(far);
    setFrustumLeft(left);
    setFrustumRight(right);
    setFrustumTop(top);
    setFrustumBottom(bottom);
}

void Camera::setFrustumPerspective(float fovY, float aspect, float near, float far) {
    setFrustumPerspective(fovY, aspect, near, far, 1, 1, 0, 0);
}

// Getters ****************************************************************
// ************************************************************************

glm::mat4 Camera::getViewMatrix() {
    if (_refreshViewMatrix) {
        doRefreshViewMatrix();
    }
    return _viewMatrix;
}

glm::vec3 Camera::getDirection() {
    return _direction;
}

glm::mat4 Camera::getProjectionMatrix() {
    if (_refreshProjectionMatrix) {
        doRefreshProjectionMatrix();
    }
    return _projectionMatrix;
}

glm::vec3 Camera::getPosition() {
    return _position;
}

glm::vec3 Camera::getUp() {
    return _up;
}

glm::vec3 Camera::getRight() {
    return _right;
}

float Camera::getFrustumFar() {
    return _ff;
}

float Camera::getFrustumLeft() {
    return _fl;
}

float Camera::getFrustumRight() {
    return _fr;
}

float Camera::getFrustumBottom() {
    return _fb;
}

float Camera::getFrustumTop() {
    return _ft;
}

bool Camera::isOrthographic() {
    return orthographic;
}