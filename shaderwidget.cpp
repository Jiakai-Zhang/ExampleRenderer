#include "shaderwidget.h"
#include <QFile>
#include <fstream>
#include <iostream>
#include <QProcess>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

ShaderWidget::ShaderWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_program(nullptr)
{

}
void ShaderWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initializeShaderProgram();

    glEnable(GL_PROGRAM_POINT_SIZE);
    m_model = new Model("../test/faro3.ply");

    qDebug() << "initialize the model successful!";

    glCullFace(GL_FRONT_AND_BACK);
    glEnable(GL_DEPTH_TEST);
    updateViewMatrix();
}

void ShaderWidget::initializeShaderProgram(){
    m_program = new QOpenGLShaderProgram();

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/default.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/default.frag");
};

void ShaderWidget::resizeGL(int w, int h){
    m_aspect = qreal(w) / qreal(h ? h : 1);
    m_width = w;
    m_height = h;
    updateProjectionMatrix();
    qDebug() << m_width << m_height;

    m_modelMatrix.setToIdentity();
}
void ShaderWidget::paintGL(){
    initializeOpenGLFunctions();
    //glPointSize(10.f);

    m_program->link();
    m_program->bind();

    glViewport(0,0,m_width,m_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_program->setUniformValue(m_program->uniformLocation("projection"), m_projectionMatrix);
    m_program->setUniformValue(m_program->uniformLocation("view"), m_viewMatrix);
    m_program->setUniformValue(m_program->uniformLocation("model"), m_modelMatrix);
    m_program->setUniformValue(m_program->uniformLocation("scale"), m_pointSize);

    m_model->draw(*m_program);
    glFlush();
}

void ShaderWidget::updateProjectionMatrix()
{
    // Reset projection
    m_projectionMatrix.setToIdentity();

    // Set perspective projection
    m_projectionMatrix.perspective(m_fov, m_aspect, m_zNear, m_zFar);
}

void ShaderWidget::updateViewMatrix()
{
    QMatrix4x4 ret;
    ret.setToIdentity();
    ret.lookAt(QVector3D(0,0,0), QVector3D(0,0,1), QVector3D(0,1,0));
    m_viewMatrix = ret;
}

static void qNormalizeAngle(float &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void ShaderWidget::setRotation(float xAngle, float yAngle, float zAngle)
{
    QMatrix4x4 temp;
    if(xAngle != 0.0f) temp.rotate(xAngle,0,1,0);
    if(yAngle != 0.0f) temp.rotate(yAngle,1,0,0);
    if(zAngle != 0.0f) temp.rotate(zAngle,0,0,1);
    QVector4D XYZW = m_viewMatrix.column(3);
    m_viewMatrix = temp * m_viewMatrix;
    m_viewMatrix.setColumn(3,XYZW);
    qDebug() << m_viewMatrix;

    update();
}

void ShaderWidget::rotateModel(float xAngle, float yAngle, float zAngle){
    QMatrix4x4 temp;
    if(xAngle != 0.0f) temp.rotate(xAngle,0,1,0);
    if(yAngle != 0.0f) temp.rotate(yAngle,1,0,0);
    if(zAngle != 0.0f) temp.rotate(zAngle,0,0,1);
    QVector4D XYZW = m_viewMatrix.column(3);
    m_viewMatrix = temp * m_viewMatrix;
    m_viewMatrix.setColumn(3,XYZW);
    qDebug() << m_viewMatrix;

}

void ShaderWidget::setRotation(QVector3D angle)
{
    setRotation(angle.x(), angle.y(), angle.z());
}

void ShaderWidget::setScale(int scaleOriention){
    if(scaleOriention == 1) {
        m_modelMatrix.scale(QVector3D(1.1f, 1.1f, 1.1f));
    }
    else if(scaleOriention == -1) {
        m_modelMatrix.scale(QVector3D(1/1.1, 1/1.1, 1/1.1));
    }
    update();
}

void ShaderWidget::setPosition(float x, float y, float z){
    m_viewMatrix.setColumn(3,QVector4D(m_viewMatrix(0,3)+x,m_viewMatrix(1,3)+y,m_viewMatrix(2,3)+z,1));
    update();
}

void ShaderWidget::saveModel(QString fileName){
    m_model->saveModel(fileName);
}

void ShaderWidget::loadModel(QString fileName){
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebufferObject());
    m_model->loadModel(fileName.toStdString());
    update();
    doneCurrent();
}

float ShaderWidget::recoverDepthFromClippedSpace(float depth, double n, double f){
    double a = -(f - n) / (2.0 * f * n);
    double b =  (f + n) / (2.0 * f * n);
    float z=(2.0*depth)-1.0;
    return (2.0*n*f)/(f+n-(z*(f-n)));
}


void ShaderWidget::saveDepth(QString fileName){

    initializeOpenGLFunctions();
    makeCurrent();

    float *bufferDepth;
    bufferDepth = (float*) malloc(height()*width()*sizeof(float));
    //Read pixels from Id FBO
    //glReadPixels(0,0,m_width,1,GL_RGBA,GL_UNSIGNED_BYTE,&bufferId[m_width*i*4]);
    int i;
    for (i = 0; i < height(); ++i) {
        glReadPixels(0,height()-1-i,width(),1,GL_DEPTH_COMPONENT,GL_FLOAT,&bufferDepth[width()*i]);
    }

    Mat depth = Mat::zeros(height(),width(),CV_32F);

    for (i = 0; i < height(); ++i) {
        for (int j = 0; j < width(); ++j) {
            if(bufferDepth[i*width()+j] != 1){
               // linear <zNear,zFar>
                depth.at<float>(i,j) = recoverDepthFromClippedSpace(bufferDepth[i*width()+j],m_zNear, m_zFar);
            }
        }
    }

    //Store point cloud depth map using u16 PNG

    depth.convertTo(depth,CV_16UC1,1000,0);

    imwrite(fileName.toStdString(),depth);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    doneCurrent();

    free(bufferDepth);
}

void ShaderWidget::setProjectionMatrix(QMatrix3x3 intrinsic, int width, int height, double near_plane, double far_plane){
    m_zNear = near_plane;
    m_zFar = far_plane;

    float f_x = intrinsic(0,0);
    float f_y = intrinsic(1,1);

    float c_x = intrinsic(0,2);
    float c_y = intrinsic(1,2);

    QMatrix4x4 projection_matrix;
    projection_matrix.fill(0);
    projection_matrix(0,0) = 2*f_x/width;

    projection_matrix(1,1) = -2*f_y/height;

    projection_matrix(0,2) = -2*c_x/width+1;
    projection_matrix(1,2) = 2*c_y/height-1;
    projection_matrix(2,2) = -(far_plane + near_plane)/(far_plane - near_plane);
    projection_matrix(2,3) = -2.0f*far_plane*near_plane/(far_plane - near_plane);
    projection_matrix(3,2) = -1.0f;
    m_projectionMatrix = projection_matrix;
//    qDebug() << "projection matrix:";
//    qDebug() << m_projectionMatrix;
    update();
};
