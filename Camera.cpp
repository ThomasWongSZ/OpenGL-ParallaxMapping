#include "Camera.h"
#include <qmath.h>
#include <QMatrix4x4>

#define MOUSE_SENSE 0.001f
/**
 * @brief Initialize the camera's Forward Vector by target coordinate and camera coordinate
 * @param position
 * @param target
 * @param worldup
 * @param parent
 */
Camera::Camera(QVector3D position, QVector3D target, QVector3D worldup)
{
    this->position  = position;
    this->worldup = worldup;
    this->forward = (target - position).normalized();

    this->right = QVector3D::crossProduct(forward, worldup).normalized();
    this->up = QVector3D::crossProduct(right, forward).normalized();

    this->MovementSpeed = SPEED;
    this->mouseSensitivity.senseX = MOUSE_SENSE;
    this->mouseSensitivity.senseY = MOUSE_SENSE;
}

/**
 * @brief Initialize the camera's Forward Vector by airplane angle
 * @param position
 * @param worldup
 * @param pitch     angle between Forward Vector and it's projection on the Z-X plane
 * @param yaw       angle between Forward Vector and it's projection on the Z-Y plane
 * @param parent
 */
Camera::Camera(QVector3D position, QVector3D worldup, float pitch, float yaw)
{
    this->position = position;
    this->worldup = worldup;
    this->pitch = pitch;
    this->yaw = yaw;

    this->forward.setX(qCos(pitch) * qSin(yaw));
    this->forward.setY(qSin(pitch));
    this->forward.setZ(qCos(pitch) * qCos(yaw));
    this->forward.normalize();

    this->right = QVector3D::crossProduct(this->forward, this->worldup).normalized();
    this->up = QVector3D::crossProduct(this->right, this->forward).normalized();

    this->MovementSpeed = SPEED;
    this->mouseSensitivity.senseX = MOUSE_SENSE;
    this->mouseSensitivity.senseY = MOUSE_SENSE;

}

Camera::~Camera(){}

/**
 * @brief Return the view matrix of this camera.You could change the view of this camera by changing the direction parameters
 * @return
 */
QMatrix4x4 Camera::getViewMatrix(){
    QMatrix4x4 matView;
    matView.setToIdentity();
    matView.lookAt(this->position, this->position + this->forward, this->worldup);
    return matView;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors(){
    this->forward.setX(qCos(pitch) * qSin(yaw));
    this->forward.setY(qSin(pitch));
    this->forward.setZ(qCos(pitch) * qCos(yaw));
    this->forward.normalize();

    this->right = QVector3D::crossProduct(this->forward, this->worldup).normalized();
    this->up = QVector3D::crossProduct(this->right, this->up).normalized();
}

// processes input received from any keyboard-like input system.
//Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//deltaTime is measured in second.
void Camera::processKeyboard(Camera_Movement direction, double deltaTime)
{
    //qDebug() << "camera position" << position;
    float step = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        position += forward * step;
    if (direction == BACKWARD)
        position -= forward * step;
    if (direction == LEFT)
        position -= right * step;
    if (direction == RIGHT)
        position += right * step;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMovement(float offsetX, float offsetY, bool constrainPitch){


    offsetX *= mouseSensitivity.senseX;
    offsetY *= mouseSensitivity.senseY;

    this->yaw += offsetX;
    this->pitch += offsetY;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if(constrainPitch){
        if(this->pitch > 2 * 3.14f){
            this->pitch = 2 * 3.14f;
        }

        if(this->pitch < -2 * 3.14f){
            this->pitch = -2 * 3.14f;
        }
    }

    if(this->yaw > 2 * 3.14f){
        this->pitch = -2 * 3.14f;
    }

    if(this->pitch < -2 * 3.14f){
        this->pitch = 2 * 3.14f;
    }

    updateCameraVectors();
}

void Camera::updateCameraPos(){
    position += forward * speedZ * 0.01f + right * speedX * 0.01f + up * speedY;
}


