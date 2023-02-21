#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "Audio/IStream.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( pulse::IStream* l = nullptr,
                         pulse::IStream* r = nullptr,
                         pulse::IStream* s = nullptr,
                         QWidget *parent = nullptr );
    ~MainWindow();

private slots:
    void pbLPressed();
    void pbLReleased();

    void pbRPressed();
    void pbRReleased();

    void pbSPressed();
    void pbSReleased();

private:
    Ui::MainWindow *ui;

    pulse::IStream* left;
    pulse::IStream* right;
    pulse::IStream* stereo;
};

#endif // MAINWINDOW_H
