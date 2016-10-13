#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QCamera>
#include <QCameraImageCapture>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileInfo>

#define TIMER_DELAY 1000

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
    QStringList cameraNames;            // 所有摄像头设备名称
    QList<QCameraInfo> camerasInfo;     // 所有摄像头设备集合

    QCamera *camera;                    // 摄像头对象
    QCameraImageCapture *capture;       // 摄像头截图控件
    QCameraViewfinder *finder;          // 摄像头显示控件

    QTimer *timer;                      // 定时器对象，用于定时更新摄像头设备


    void initView();
    void updateCameras();               // 更新摄像头设备

private slots:
    void cameraBtnSlot();
    void clearImgBtnSlot();
    void choosePathBtnSlot();
    void captureBtnSlot();
    void openImgBtnSlot();

    void cameraCaptureSlot(int id, QImage image);
    void captureErrorSlot(int id, QCameraImageCapture::Error error, QString errorString);
    void cameraStateSlot(QCamera::State state);
    void cameraErrorSlot(QCamera::Error error);

    void timerSlot();
};

#endif // MAINWINDOW_H
