#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(pulse::RecordingStream* str, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stream(str)
{
    ui->setupUi(this);
    connect(this->ui->pushButton, &QAbstractButton::pressed, this, &MainWindow::buttonPressed);
    connect(this->ui->pushButton, &QAbstractButton::released, this, &MainWindow::buttonReleased);
    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonPressed()
{
    static bool fl = 0;
    if (!fl)
    {
        stream->resume();
        fl = 1;
    }
    else
    {
        stream->pause();
        fl = 0;
    }
}

void MainWindow::buttonReleased()
{
//    stream->pause();
}
