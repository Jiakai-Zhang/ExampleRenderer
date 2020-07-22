#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QKeyEvent>
#include "shaderwidget.h"

struct Rotation{
    QVector3D rotation = {0, 0, 0};
    int t = 0;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void on_SaveModel_clicked();

    void on_LoadModel_clicked();

    
    void on_LoadIntrinsic_clicked();

    void on_LoadExtrinsic_clicked();

private:
    Ui::MainWindow *ui;
    ShaderWidget* m_shader;

    Rotation rotation_cur;
    QPoint m_lastPos;
    bool m_dragEnable = false;
    bool m_selectLineFlag = false, m_selectTriFlag = false, m_selectPolygonFlag = false;
    bool m_lineSelectedOne = false, m_lineSelectedTwo = false;
    bool m_polygonSelected = false;
    float m_scale = 1.0;
    float m_x = 0;
    float m_y = 0;
    int m_pointOneX = -1, m_pointOneY = -1, m_pointTwoX = -1, m_pointTwoY = -1;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
signals:
    void sigSelectedOne(int x, int y);
    void sigSelectedTwo(int x, int y);
};

#endif // MAINWINDOW_H
