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
    explicit MainWindow(pulse::RecordingStream* l, pulse::RecordingStream* r, pulse::RecordingStream* s, QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void pbLPressed();
    void pbLReleased();

    void pbRPressed();
    void pbRReleased();

    void pbSPressed();
    void pbSReleased();

private:
    pulse::RecordingStream* left;
    pulse::RecordingStream* right;
    pulse::RecordingStream* stereo;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
