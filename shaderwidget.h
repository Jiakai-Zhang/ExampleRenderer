#ifndef SHADERWIDGET_H
#define SHADERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions_4_5_Core>
#include "model.h"
#include <QKeyEvent>
#include <QProcess>

class ShaderWidget: public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    ShaderWidget(QWidget *parent = nullptr);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    QOpenGLShaderProgram *m_program;
    Model* m_model;
    QMatrix4x4 m_modelMatrix, m_viewMatrix, m_projectionMatrix;

    qreal m_aspect;
    int m_width, m_height;
    qreal m_zNear = 0.1, m_zFar = 10.0, m_fov = 45.0;

    float m_xAngle = 0;
    float m_yAngle = 0;
    float m_zAngle = 0;
    float m_x = 0;
    float m_y = 0;
    float m_z = 0;
    float m_pointSize = 1;

    void initializeShaderProgram();
    void initializeCamera();

    void updateProjectionMatrix();
    void updateViewMatrix();
public slots:
    //Set Fuctions
    void setRotation(QVector3D angle);
    void setRotation(float xAngle, float yAngle, float zAngle);
    void setScale(int scaleOrientation);
    void setPosition(float x, float y, float z);
    void setViewMatrix(QMatrix4x4 mat){m_viewMatrix = mat;}
    void setProjectionMatrix(QMatrix3x3 intrinsic, int width, int height, double near_plane, double far_plane);
    void saveModel(QString fileName);
    void loadModel(QString fileName);
    void saveDepth(QString fileName);

    //Rotate Model
    void rotateModel(float xAngle, float yAngle, float zAngle);
    float recoverDepthFromClippedSpace(float depth, double n, double f);
signals:
    void xRotationChanged(double angle);
    void yRotationChanged(double angle);
    void zRotationChanged(double angle);
    void sigDetected();
};

#endif // SHADERWIDGET_H
