#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "Camera.h"
#include "Light.h"
#include "qmath.h"
class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void initGL();
    void initMVP();
    void renderQuad();
    float radians(float degree){return M_PI * degree / 180;};

private:
    QOpenGLShaderProgram    *m_shader = nullptr;
    uint                     m_valid_shader = 0;
    QMatrix4x4               m_mat_model;
    QMatrix4x4               m_mat_view;
    QMatrix4x4               m_mat_proj;
    QOpenGLTexture          *m_tex_brickwall = nullptr;
    QOpenGLTexture          *m_tex_normal = nullptr;
    QOpenGLTexture          *m_tex_depth = nullptr;
    Camera                  *m_camera = nullptr;
    Light                   *m_point_light = nullptr;
};

#endif // MYGLWIDGET_H
