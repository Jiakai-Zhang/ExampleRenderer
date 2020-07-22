#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model.h"
#include <QSizePolicy>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_shader = new ShaderWidget();
    //setCentralWidget(m_shader);
    ui->openGLWidgetContainer->layout()->addWidget(m_shader);
    //m_shader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    int x = m_lastPos.x();
    int y = ui->openGLWidgetContainer->height()-m_lastPos.y();
//    qDebug() << "openGL container size is " << ui->openGLWidgetContainer->width() << ui->openGLWidgetContainer->height();
//    qDebug() << "openGL container XY is " << ui->openGLWidgetContainer->x() << ui->openGLWidgetContainer->y();
//    qDebug() << "Global mouse position is " << m_lastPos.x() << m_lastPos.y();
//    qDebug() << "openGL mouse position is " << m_lastPos.x() << ui->openGLWidgetContainer->height()-m_lastPos.y();
    if ((m_lastPos.x() >= m_shader->pos().x()) &&
        (m_lastPos.y() >= m_shader->pos().y()) &&
        (m_lastPos.x() <= m_shader->pos().x() + m_shader->size().width()) &&
            (m_lastPos.y() <= m_shader->pos().y() + m_shader->size().height()))
    m_dragEnable = true;
    else m_dragEnable = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragEnable == false) return;

    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotation_cur.rotation.setX(dx);
        rotation_cur.rotation.setY(dy);
        m_shader->setRotation(rotation_cur.rotation);
    } else if (event->buttons() & Qt::RightButton) {
        //rotation_cur.rotation.setX(dy);
        rotation_cur.rotation.setZ(dx);
        m_shader->setRotation(rotation_cur.rotation);
    } else if (event->buttons() & Qt::MidButton) {
        m_x += 0.1f*dx;
        m_y -= 0.1f*dy;
        m_shader->setPosition(0.1f*dx,-0.1f*dy,0.0f);
    }

    m_lastPos = event->pos();

}
void MainWindow::wheelEvent(QWheelEvent *event){
    if(event->delta() > 0){                    // 当滚轮远离使用者时
            m_shader->setScale(1);               // 进行放大
        }else if(event->delta() < 0){                                     // 当滚轮向使用者方向旋转时
            m_shader->setScale(-1);              // 进行缩小
        }
};

void MainWindow::on_SaveModel_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Please select the file path to store ply", tr("*.ply"));
    if(fileName.isEmpty())
   {
        return;
   }
   else
   {
        m_shader->saveModel(fileName);
   }
}

void MainWindow::on_LoadModel_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Please select the file path to store ply", tr("*.ply"));
    if(fileName.isEmpty())
   {
        return;
   }
   else
   {
        m_shader->loadModel(fileName);
   }
}

void MainWindow::on_LoadIntrinsic_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择内参"));
    fileDialog->setDirectory(".");
    QString fileName;
    if(fileDialog->exec() == QDialog::Accepted) {
        fileName = fileDialog->selectedFiles()[0];
    }
    qDebug() << fileName;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)){
        qDebug() << "open file";
        QTextStream in(&file);
        in.readLine();
        float width,height;
        in >> width >> height;
        //setFixedSize(width+50,height+100);
        //ui->openGLWidgetContainer->setFixedSize(width, height+100);
        m_shader->setFixedSize(width, height);
        in.readLine();
        in.readLine();
        in.readLine();
        //import intrinsics
        QMatrix3x3 intrinsic_matrix;
        int count = 0;
        while(!in.atEnd()){
            count++;
            if(count > 9){
                break;
            }
            QString temp;
            in >> temp;
            int row = (count-1)/3;
            int col = count-1 - 3*row;
            intrinsic_matrix(row,col) = temp.toFloat();
        }
        qDebug() << intrinsic_matrix;
        //Transform to openGL projection matrix
        m_shader->setProjectionMatrix(intrinsic_matrix,width,height,1,10);
    }
}

void MainWindow::on_LoadExtrinsic_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择外参"));
    fileDialog->setDirectory(".");
    QString fileName;
    if(fileDialog->exec() == QDialog::Accepted) {
        fileName = fileDialog->selectedFiles()[0];
    }
    qDebug() << fileName;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        QMatrix4x4 pose;
        int count = 0;
        while(!in.atEnd()){
            count++;
            if(count > 16){
                break;
            }
            QString temp;
            in >> temp;
            int row = (count-1)/4;
            int col = count-1 - 4*row;
            pose(row,col) = temp.toFloat();
        }
        //May have a transform for the different model coordinate with openGL coordinate
        QMatrix4x4 transform(1,0,0,0,
                             0,1,0,0,
                             0,0,-1,0,
                             0,0,0,1);
        //qDebug() << pose.inverted();
        //We input the pose, but we need an inverted pose (Extrinsic)
        m_shader->setViewMatrix(transform*pose.inverted());
    }
}
