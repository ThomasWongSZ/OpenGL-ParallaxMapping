#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
class Camera : public QObject
{
    Q_OBJECT
public:
    enum Camera_Movement{
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    typedef struct Sense{
        float senseX;
        float senseY;
    }MouseSensitivity;

    explicit Camera(QVector3D position, QVector3D target, QVector3D worldup);
    explicit Camera(QVector3D position, QVector3D worldup, float pitch, float yaw);
    ~Camera();

    // direction parameters //
    QVector3D position;     //center position of camera
    QVector3D worldup;      //up direction in world-coordinate

    /**
     * @brief euler Angle. angle between Forward Vector and Z-X plane.unit is radius.
     */
    float pitch;
    /**
     * @brief euler Angle.angle between Forward Vector and Z-Y plane(same as <Proj.Of Front in Z-X plane, Z-axis>).unit is radius.
     */
    float yaw;
    //by the way, the roll angle is the angle between Right-Vector and X-Z plane

    MouseSensitivity mouseSensitivity;
    float MovementSpeed;  
    float speedZ = 0;		//speed of camera
    float speedX = 0;
    float speedY = 0;

    //default camera settings
    const float SPEED       =  2.5f;

    QMatrix4x4 getViewMatrix();
    void updateCameraVectors();
    void processKeyboard(Camera_Movement direction, double deltaTime);
    void processMovement(float offsetX, float offsetY, bool constrainPitch = true);
    void updateCameraPos();

private:
    QVector3D forward;      //front direction of camera(pointing towards the target position)
    QVector3D right;        //right direction of camera
    QVector3D up;           //up direction of camera

};

#endif // CAMERA_H
