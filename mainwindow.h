#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "Audio/RecordingStream.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( pulse::RecordingStream* l = nullptr,
                         pulse::RecordingStream* r = nullptr,
                         pulse::RecordingStream* s = nullptr,
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

    pulse::RecordingStream* left;
    pulse::RecordingStream* right;
    pulse::RecordingStream* stereo;
};

#endif // MAINWINDOW_H
