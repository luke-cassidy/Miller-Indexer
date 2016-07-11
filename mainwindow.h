#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define USE_MATH_DEFINES

#include <QMainWindow>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QTransform>
#include <QLabel>
#include <QtGui>
#include <cmath>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    double calcAngle(int x, int y);
    double transform(double a, int i);
    double refactorIndices(int i, double a, double b, double c);
    double gcd(double a, double b);

};

#endif // MAINWINDOW_H
