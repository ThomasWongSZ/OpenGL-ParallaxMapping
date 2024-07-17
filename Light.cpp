#include "Light.h"

Light::Light(QObject *parent)
    : QObject{parent}
{

}

Light::Light(QVector3D pos, QObject *parent)
    : QObject{parent}
    , m_position(pos)
{

}
