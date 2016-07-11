#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>

//set window size
int winSize[2] = {500, 500};
double horIndices[3] = {1.0, 0.0, 0.0};
double vertIndices[3] = {0.0, 1.0, 0.0};

//create window
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Indexer");
    setFixedSize(winSize[0], winSize[1]);

    setMouseTracking(true);
}

//paint axis on window
void MainWindow::paintEvent(QPaintEvent *)
{
    int winCentre[2] = {winSize[0]/2, winSize[1]/2};

    QPainter painter(this);

    /*
    QColor backgroundColor = palette().light().color();
    backgroundColor.setAlpha(200);

    painter.fillRect(rect(),backgroundColor);
    */

    this->setWindowOpacity(0.5);
    painter.drawLine(winCentre[0]/4, winCentre[1], (7*winCentre[0])/4, winCentre[1]);
    painter.drawLine(winCentre[0], winCentre[1]/4, winCentre[0], (7*winCentre[1])/4);
    painter.drawLine(winCentre[0]/2, winCentre[1]/2, winCentre[0]/2, (3*winCentre[1])/2);
    painter.drawLine((3*winCentre[0])/2, (3*winCentre[1])/2, (3*winCentre[0])/2, winCentre[1]/2);
    painter.drawLine(winCentre[0]/2, winCentre[1]/2, (3*winCentre[0])/2, winCentre[1]/2);
    painter.drawLine((3*winCentre[0])/2, (3*winCentre[1])/2, winCentre[0]/2, (3*winCentre[1])/2);

}

//run when mouse moves
void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    //flip coordinates so that positive y axis is up
    QTransform t;
    t.scale(1, -1);
    t.translate(0, -winSize[1]+1);
    //get coordinates
    QPoint pos = ev->pos() * t;

    //set origin point to centre of window
    int x = pos.x()-round(0.5*((double)winSize[0]));
    int y = pos.y()- round(0.5*((double)winSize[1]));

    //calculate mouse angle in radians
    double a = calcAngle(x, y);

    //convert angle to degrees
    int angle = round((a / M_PI) * 180);

    //transform coordinates using angle
    double indices[3];

    for (int i = 0; i < 3; i++)
    {
        indices[i] =  transform(a, i);
    }

    //refactor indicies
    double newIndices[3];

    for (int i = 0; i < 3; i++)
    {
        newIndices[i] = refactorIndices(i, indices[0], indices[1], indices[2]);
    }

    //update text fields
    ui->label_2->setText(QString("%1, %2").arg(x).arg(y));
    ui->label_4->setText(QString("%1").arg(angle));
    ui->label_6->setText(QString("%1, %2, %3").arg(newIndices[0]).arg(newIndices[1]).arg(newIndices[2]));

}

//calculate angle function
double MainWindow::calcAngle(int x, int y)
{

    double position[2];
    position[0] = (double)x;
    position[1] = (double)y;

    double tanAngle;
    double radAngle;
    double angle;

    //calculate angle based on what quadrant cursor is in
    if((position[0] > 0) & (position[1] >= 0))
    {
        tanAngle = position[1] / position[0];
        radAngle = atan(tanAngle);
        angle = radAngle;
    }
    else if((position[0] <= 0) & (position[1] > 0))
    {
        tanAngle = -position[0] / position[1];
        radAngle = atan(tanAngle);
        angle = radAngle + M_PI/2;
    }
    else if((position[0] < 0) & (position[1] <= 0))
    {
        tanAngle = position[1] / position[0];
        radAngle = atan(tanAngle);
        angle = radAngle + M_PI;
    }
    else if((position[0] >= 0) & (position[1] < 0))
    {
        tanAngle = -position[0] / position[1];
        radAngle = atan(tanAngle);
        angle = radAngle +(3*M_PI)/2;
    }
    else
    {
        angle = 0;
    }

    return angle;
}

//coordinate transformation
double MainWindow::transform(double a, int i)
{

    //calculate cross product of axis vectors to find axis of rotation
    double rotVector[3];

    rotVector[0] = -(horIndices[1]*vertIndices[2] - horIndices[2]*vertIndices[1]);
    rotVector[1] = -(horIndices[2]*vertIndices[0] - horIndices[0]*vertIndices[2]);
    rotVector[2] = -(horIndices[0]*vertIndices[1] - horIndices[1]*vertIndices[0]);


    //adjusting rotVector so that x^2 + y^2 + z^2 = 1
    double mag = pow(pow(rotVector[0],2) + pow(rotVector[1],2) + pow(rotVector[2],2), 0.5);

    double rotAxis[3];

    rotAxis[0] = (1/mag)*rotVector[0];
    rotAxis[1] = (1/mag)*rotVector[1];
    rotAxis[2] = (1/mag)*rotVector[2];

    //transformation matrix
    double transMatrix[3][3];

    transMatrix[0][0] = pow(rotAxis[0], 2)*(1 - cos(a)) + cos(a);
    transMatrix[1][0] = (rotAxis[0] * rotAxis[1])*(1 - cos(a)) - rotAxis[2] * sin(a);
    transMatrix[2][0] = (rotAxis[0] * rotAxis[2])*(1 - cos(a)) + rotAxis[1] * sin(a);
    transMatrix[0][1] = (rotAxis[1] * rotAxis[0])*(1 - cos(a)) + rotAxis[2] * sin(a);
    transMatrix[1][1] = pow(rotAxis[1], 2)*(1 - cos(a)) + cos(a);
    transMatrix[2][1] = (rotAxis[1] * rotAxis[2])*(1 - cos(a)) - rotAxis[0] * sin(a);
    transMatrix[0][2] = (rotAxis[2] * rotAxis[0])*(1 - cos(a)) - rotAxis[1] * sin(a);
    transMatrix[1][2] = (rotAxis[2] * rotAxis[1])*(1 - cos(a)) + rotAxis[0] * sin(a);
    transMatrix[2][2] = pow(rotAxis[2], 2)*(1 - cos(a)) + cos(a);


    //multiply transformation matrix and axis vector
    double indices[3];
    for (int j = 0; j < 3; j++)
    {
        indices[j] = (transMatrix[j][0] * horIndices[0]) + (transMatrix[j][1] * horIndices[1]) + (transMatrix[j][2] * horIndices[2]);
    }

    //round up indices to integer^1000
    indices[i] = round(indices[i]*10);

    return indices[i];
}

//refactor indices function
double MainWindow::refactorIndices(int i, double a, double b, double c)
{
    double indices[3];
    double newIndices[3];

    indices[0] = a;
    indices[1] = b;
    indices[2] = c;

    int factor;

    //find gcd of x and y indices
    if (indices[0] >= indices[1])
    {
        factor = gcd(indices[0], indices[1]);
    }
    else
    {
        factor = gcd(indices[1], indices[0]);
    }

    //find gdc of factor and z index
    if (factor >= indices[2])
    {
        factor = abs(gcd(factor, indices[2]));

        //change indices to lowest integers
        newIndices[0] = indices[0]/factor;
        newIndices[1] = indices[1]/factor;
        newIndices[2] = indices[2]/factor;
    }
    else
    {
        factor = abs(gcd(indices[2], factor));

        //change indices to lowest integers
        newIndices[0] = indices[0]/factor;
        newIndices[1] = indices[1]/factor;
        newIndices[2] = indices[2]/factor;
    }

    return newIndices[i];
}

//Euclidean algorithm function
double MainWindow::gcd(double a, double b)
{
    int c = round(a);
    int d = round(b);

    //loop Euclidean algorithm until there is no remainder left
    while (d != 0)
    {
        int temp;

        temp = d;
        d = round(fmod((double)c, (double)d));
        c = temp;
    }

    a = (double)(c);

    return a;
}

MainWindow::~MainWindow()
{
    delete ui;
}
