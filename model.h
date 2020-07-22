#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLExtraFunctions>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLShaderProgram>
#include <string>

using namespace std;

struct Vertex {
    // position
    QVector3D position;
    QVector3D color;
};

class Model : public QOpenGLExtraFunctions
{
public:
    Model(string path);

    GLuint VAO;
    //GLuint IDS;
    void draw(QOpenGLShaderProgram &program);
    vector<Vertex> m_vertices;
    void processVertices();

    bool loadModel(string path);
    void saveModel(QString path);


private:
    GLuint VBO, EBO;
    vector<unsigned int> m_indices;
    unsigned int m_faceSize;

    string m_header;
    QVector3D m_center;
};

#endif // MODEL_H
