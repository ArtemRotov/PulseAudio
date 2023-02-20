#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(pulse::RecordingStream* l, pulse::RecordingStream* r, pulse::RecordingStream* s, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , left(l)
    , right(r)
    , stereo(s)
{
    ui->setupUi(this);

    connect(this->ui->pbLeft, &QAbstractButton::pressed, this, &MainWindow::pbLPressed);
    connect(this->ui->pbLeft, &QAbstractButton::released, this, &MainWindow::pbLReleased);


    connect(this->ui->pbRight, &QAbstractButton::pressed, this, &MainWindow::pbRPressed);
    connect(this->ui->pbRight, &QAbstractButton::released, this, &MainWindow::pbRReleased);

    connect(this->ui->pbStereo, &QAbstractButton::pressed, this, &MainWindow::pbSPressed);
    connect(this->ui->pbStereo, &QAbstractButton::released, this, &MainWindow::pbSReleased);

    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pbLPressed()
{
    if(left)
        left->resume();
}

void MainWindow::pbLReleased()
{
    if(left)
       left->pause();
}

void MainWindow::pbRPressed()
{
    if(right)
       right->resume();
}

void MainWindow::pbRReleased()
{
    if(right)
        right->pause();
}

void MainWindow::pbSPressed()
{
   if(stereo)
       stereo->resume();
}

void MainWindow::pbSReleased()
{
    if(stereo)
       stereo->pause();
}
