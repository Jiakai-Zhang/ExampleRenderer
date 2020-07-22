#include "model.h"
#include <fstream>
#include <iostream>
#include <happly.h>
Model::Model(string path)
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    loadModel(path);
}


//load PLYRGB files
bool Model::loadModel(string path){
    m_vertices.clear();
        m_indices.clear();
        happly::PLYData plyIn(path,true);
        std::vector<std::array<double, 3>> positions = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> colors = plyIn.getVertexColors();
        std::vector<std::vector<size_t>> faces;
        if (plyIn.hasElement("face"))
            faces = plyIn.getFaceIndices<size_t>();
        for(int i = 0;i<positions.size();i++){
            Vertex vertex;
            vertex.position.setX(positions[i][0]);
            vertex.position.setY(positions[i][1]);
            vertex.position.setZ(positions[i][2]);
            vertex.color.setX(colors[i][0]);
            vertex.color.setY(colors[i][1]);
            vertex.color.setZ(colors[i][2]);

            m_vertices.push_back(vertex);
        }
        if(faces.size() != 0){
        m_faceSize = faces[0].size();
            for (unsigned int i = 0; i < faces.size(); ++i) {
                for (unsigned int j = 0; j < m_faceSize; ++j) {
                    m_indices.push_back(faces[i][j]);
                }
            }
        }else{
            for (unsigned int i = 0; i < positions.size(); ++i) {
                m_indices.push_back(i);
            }
        }
        //put origin to model center.
        m_center = QVector3D(0,0,0);
        for(unsigned int i = 0; i < m_vertices.size() ; ++i) {
            m_center += m_vertices.at(i).position/m_vertices.size();
        }
        qDebug() << "center is " << m_center;
        for(unsigned int i = 0; i < m_vertices.size() ; ++i) {
            m_vertices.at(i).position -= m_center;
        }

        processVertices();
        return true;
}

void Model::saveModel(QString path){
    std::vector<std::array<double, 3>> meshVertexPositions;
    std::vector<std::array<uchar, 3>> meshVertexColors;
    std::vector<std::vector<size_t>> meshFaceIndices;

    // Create an empty object
    happly::PLYData plyOut;
    for(unsigned int i = 0; i < m_vertices.size() ; ++i) {
        QVector3D pos = m_vertices.at(i).position;
        std::array<double, 3> posArray;
        posArray[0] = pos.x();
        posArray[1] = pos.y();
        posArray[2] = pos.z();
        meshVertexPositions.push_back(posArray);
    }

    for(unsigned int i = 0; i < m_vertices.size() ; ++i) {
        QVector3D color = m_vertices.at(i).color;
        std::array<uchar, 3> colorArray;
        colorArray[0] = color.x();
        colorArray[1] = color.y();
        colorArray[2] = color.z();
        meshVertexColors.push_back(colorArray);
    }


    // Add mesh data (elements are created automatically)
    plyOut.addVertexPositions(meshVertexPositions);
    plyOut.addVertexColors(meshVertexColors);


    // Write the object to file
    plyOut.write(path.toStdString(), happly::DataFormat::Binary);
}

void Model::processVertices(){

    //glGenBuffers(1, &IDS);
    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex Colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    //Bind to Nothing
    glBindVertexArray(0);
}

void Model::draw(QOpenGLShaderProgram &program){

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
