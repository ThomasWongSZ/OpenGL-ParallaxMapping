#include "MyGLWidget.h"
#include <QMatrix4x4>
#include <QTimer>
#include <QMouseEvent>
#define TEX_CH_BRICK    0
#define TEX_CH_NORMAL   1
#define TEX_CH_DEPTH    2
GLuint quadVAO = 0;
GLuint quadVBO = 0;
MyGLWidget::MyGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    this->setMouseTracking(true);
}

MyGLWidget::~MyGLWidget()
{
    if(m_point_light){
        delete m_point_light;
    }
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();    // let the function pointers such as glClearColor point towards the actual address.
    m_point_light = new Light(QVector3D(0.2f, 0.3f, 0.3f));

    // texture
    m_tex_brickwall = new QOpenGLTexture(QImage(":/image/wood.png"));
    m_tex_brickwall->bind(TEX_CH_BRICK);

    m_tex_normal = new QOpenGLTexture(QImage(":/image/toy_box_normal.png"));
    m_tex_normal->bind(TEX_CH_NORMAL);

    m_tex_depth = new QOpenGLTexture(QImage(":/image/toy_box_disp.png"));
    m_tex_depth->bind(TEX_CH_DEPTH);

    m_tex_brickwall->setMinificationFilter(QOpenGLTexture::Nearest);    //缩小
    m_tex_brickwall->setMagnificationFilter(QOpenGLTexture::Linear);    //放大
    m_tex_brickwall->setWrapMode(QOpenGLTexture::Repeat);               //环绕模式
    m_tex_normal->setMinificationFilter(QOpenGLTexture::Nearest);
    m_tex_normal->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex_normal->setWrapMode(QOpenGLTexture::Repeat);
    m_tex_depth->setMinificationFilter(QOpenGLTexture::Nearest);
    m_tex_depth->setMagnificationFilter(QOpenGLTexture::Linear);
    m_tex_depth->setWrapMode(QOpenGLTexture::Repeat);


}

void MyGLWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void MyGLWidget::paintGL()
{
    initGL();
    initMVP();
    renderQuad();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);

    float halfWidth = (float)width() / 2;
    float halfHeight = (float)height() / 2;
    int reverseY = height() - event->pos().y();

    float lightPosX = ((float)event->pos().x() - halfWidth) / halfWidth;
    float lightPosY = ((float)reverseY - halfHeight) / halfHeight;
    m_point_light->setPosition(QVector3D(lightPosX, lightPosY, 0.3f));
    update();
}

void MyGLWidget::initGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // framewire of fill
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);    // turn on texture otherwise there will be no effect when you call glBindTexture().

    // shader
    if(!m_valid_shader){
        m_shader = new QOpenGLShaderProgram();
        m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/normal_mapping.vert");
        m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/normal_mapping.frag");
        m_valid_shader = m_shader->link();
        if(!m_valid_shader){
            qDebug() << "ERR:" << m_shader->log();
        }
    }

}

void MyGLWidget::initMVP()
{
    if(!m_camera){
        m_camera = new Camera(QVector3D(0.0f, 0.0f, 3.5f), QVector3D(0.0f, 1.0f, 0.0f), radians(0.0f), radians(180.0f));
    }

    static float model_angle_x = 0;
    static float model_angle_y = 0;
    static float model_angle_z = 0;

    // model (rotZ->rotY->rotX)
    m_mat_model.setToIdentity();
    m_mat_model.rotate(model_angle_x, QVector3D(1.0f, 0.0f, 0.0f));
    m_mat_model.rotate(model_angle_y, QVector3D(0.0f, 1.0f, 0.0f));
    m_mat_model.rotate(model_angle_z, QVector3D(0.0f, 0.0f, 1.0f));

    m_mat_view = m_camera->getViewMatrix();
    m_mat_proj.setToIdentity();
    m_mat_proj.perspective(45.0f, 800 / 600, 0.1f, 100.0f);
}

void MyGLWidget::renderQuad()
{
    if(quadVAO == 0){

        // positions
        QVector3D pos1(-1.0f,  1.0f, 0.0f);
        QVector3D pos2(-1.0f, -1.0f, 0.0f);
        QVector3D pos3( 1.0f, -1.0f, 0.0f);
        QVector3D pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        QVector2D uv1(0.0f, 1.0f);
        QVector2D uv2(0.0f, 0.0f);
        QVector2D uv3(1.0f, 0.0f);
        QVector2D uv4(1.0f, 1.0f);
        // normal vector
        QVector3D nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        QVector3D tangent1, bitangent1;
        QVector3D tangent2, bitangent2;
        // triangle 1
        // ----------
        QVector3D edge1 = pos2 - pos1;
        QVector3D edge2 = pos3 - pos1;
        QVector2D deltaUV1 = uv2 - uv1;
        QVector2D deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

        tangent1.setX(f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x()));
        tangent1.setY(f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y()));
        tangent1.setZ(f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z()));

        bitangent1.setX(f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x()));
        bitangent1.setY(f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y()));
        bitangent1.setZ(f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z()));

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

        tangent2.setX(f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x()));
        tangent2.setY(f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y()));
        tangent2.setZ(f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z()));


        bitangent2.setX(f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x()));
        bitangent2.setY(f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y()));
        bitangent2.setZ(f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z()));


        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x(), pos1.y(), pos1.z(), nm.x(), nm.y(), nm.z(), uv1.x(), uv1.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),
            pos2.x(), pos2.y(), pos2.z(), nm.x(), nm.y(), nm.z(), uv2.x(), uv2.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),
            pos3.x(), pos3.y(), pos3.z(), nm.x(), nm.y(), nm.z(), uv3.x(), uv3.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),

            pos1.x(), pos1.y(), pos1.z(), nm.x(), nm.y(), nm.z(), uv1.x(), uv1.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z(),
            pos3.x(), pos3.y(), pos3.z(), nm.x(), nm.y(), nm.z(), uv3.x(), uv3.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z(),
            pos4.x(), pos4.y(), pos4.z(), nm.x(), nm.y(), nm.z(), uv4.x(), uv4.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z()
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    if(m_valid_shader){
        m_shader->setUniformValue("diffuseMap", TEX_CH_BRICK);
        m_tex_brickwall->bind(TEX_CH_BRICK);
        m_shader->setUniformValue("normalMap", TEX_CH_NORMAL);
        m_tex_normal->bind(TEX_CH_NORMAL);
        m_shader->setUniformValue("depthMap", TEX_CH_DEPTH);
        m_tex_depth->bind(TEX_CH_DEPTH);

        m_shader->bind();
        m_shader->setUniformValue("model", m_mat_model);
        m_shader->setUniformValue("view", m_mat_view);
        m_shader->setUniformValue("projection", m_mat_proj);
        m_shader->setUniformValue("lightPos", m_point_light->position());
        m_shader->setUniformValue("viewPos", m_camera->position);
        m_shader->setUniformValue("height_scale", 0.1f);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}













