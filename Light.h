#ifndef LIGHT_H
#define LIGHT_H

#include "qvector3d.h"
#include <QObject>

class Light : public QObject
{
    Q_OBJECT
public:
    Light(QObject *parent = nullptr);
    Light(QVector3D pos, QObject *parent = nullptr);

    inline QVector3D position(){return m_position;};
    inline void setPosition(const QVector3D &pos){m_position = pos;};
private:
    QVector3D       m_position;
};

#endif // LIGHT_H
