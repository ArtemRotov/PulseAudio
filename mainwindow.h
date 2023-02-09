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
    explicit MainWindow(pulse::RecordingStream* str, QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void buttonPressed();
    void buttonReleased();

private:
    pulse::RecordingStream* stream;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
