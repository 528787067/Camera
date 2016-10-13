#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->initView();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    timer->start(TIMER_DELAY);
}

MainWindow::~MainWindow()
{
    if(timer->isActive())
        timer->stop();

    delete ui;
    delete finder;
    delete timer;
}

void MainWindow::initView()
{
    this->setWindowTitle("广东海洋大学 创客315");
    ui->savePathLEdit->setText(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0]);
    finder = new QCameraViewfinder();
    ui->cameraView->addWidget(finder);

    this->timerSlot();

    connect(ui->cameraBtn, SIGNAL(clicked(bool)), this, SLOT(cameraBtnSlot()));
    connect(ui->clearImgBtn, SIGNAL(clicked(bool)), this, SLOT(clearImgBtnSlot()));
    connect(ui->choosePathBtn, SIGNAL(clicked(bool)), this, SLOT(choosePathBtnSlot()));
    connect(ui->captureBtn, SIGNAL(clicked(bool)), this, SLOT(captureBtnSlot()));
    connect(ui->openImgBtn, SIGNAL(clicked(bool)), this, SLOT(openImgBtnSlot()));
}

void MainWindow::updateCameras()
{
    camerasInfo.clear();
    cameraNames.clear();
    cameraNames << "";
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras())
    {
        camerasInfo << cameraInfo;
        cameraNames << cameraInfo.description();
    }
}

void MainWindow::cameraBtnSlot()
{
    if(ui->cameraCBox->isEnabled())
        if(ui->cameraCBox->currentIndex() == 0)
            QMessageBox::information(this, "警告", "请选择一个摄像头");
        else
        {
            camera = new QCamera(camerasInfo[ui->cameraCBox->currentIndex()-1]);
            capture = new QCameraImageCapture(camera);

            capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
            camera->setCaptureMode(QCamera::CaptureStillImage);

            connect(capture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(cameraCaptureSlot(int,QImage)));
            connect(capture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(captureErrorSlot(int,QCameraImageCapture::Error,QString)));
            connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(cameraStateSlot(QCamera::State)));
            connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraErrorSlot(QCamera::Error)));

            camera->setViewfinder(finder);
            camera->start();

            if(camera->state() == QCamera::ActiveState)
            {
                ui->cameraCBox->setEnabled(false);
                ui->cameraBtn->setText("关闭");
                finder->show();
            }
            else
                QMessageBox::information(this, "警告", "无法启动该摄像头");
        }
    else
    {
        finder->hide();
        ui->cameraCBox->setEnabled(true);
        ui->cameraBtn->setText("启动");

        delete capture;
        delete camera;
    }
}

void MainWindow::clearImgBtnSlot()
{
    ui->imgShowLabel->clear();
}

void MainWindow::choosePathBtnSlot()
{
    QString path, choosePath;

    path = ui->savePathLEdit->text();

    if(QFileInfo(path).exists() || QFileInfo(path + ".jpg").exists())
        choosePath = path;
    else
        choosePath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];

    choosePath = QFileDialog::getExistingDirectory(this, "选择截图保存路径", choosePath);

    if(choosePath.isEmpty())
        ui->savePathLEdit->setText(path);
    else
        ui->savePathLEdit->setText(choosePath);
}

void MainWindow::captureBtnSlot()
{
    if(ui->cameraCBox->isEnabled())
        QMessageBox::information(this, "警告", "请打开摄像头后重试");
    else if(capture->isReadyForCapture())
        capture->capture(ui->savePathLEdit->text());
    else
        QMessageBox::information(this, "警告", "截图失败，请稍后重试");
}

void MainWindow::openImgBtnSlot()
{
    QString path;
    if(ui->savePathLEdit->text().isEmpty())
        path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    else
        path = ui->savePathLEdit->text();

    if(QFileInfo(path).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    else if(QFileInfo(path + ".jpg").exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path + ".jpg"));
    else
        QMessageBox::information(this, "警告", "当前路径不存在，请检查路径后重试");
}

void MainWindow::cameraCaptureSlot(int id, QImage image)
{
    Q_UNUSED(id)
    ui->imgShowLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imgShowLabel->geometry().size(), Qt::KeepAspectRatio)));
}

void MainWindow::captureErrorSlot(int id, QCameraImageCapture::Error error, QString errorString)
{
    if(id == 1 && error == QCameraImageCapture::ResourceError)
        QMessageBox::information(this, "保存失败", "截图保存失败，请检查路径后重试");
    else
        QMessageBox::information(this, "Capture error", errorString);
}

void MainWindow::cameraStateSlot(QCamera::State state)
{
    switch (state)
    {
    case QCamera::ActiveState:
        break;
    case QCamera::UnloadedState:
        if(!ui->cameraCBox->isEnabled())
        {
            this->finder->hide();
            ui->cameraCBox->setEnabled(true);
            ui->cameraBtn->setText("启动");

            delete capture;
            delete camera;
        }
        break;
    case QCamera::LoadedState:
        break;
    }
}

void MainWindow::cameraErrorSlot(QCamera::Error error)
{
    if(error != QCamera::NoError && !ui->cameraCBox->isEnabled())
    {
        this->finder->hide();
        ui->cameraCBox->setEnabled(true);
        ui->cameraBtn->setText("启动");

        delete capture;
        delete camera;

        QMessageBox::information(this, "Camera error", camera->errorString());
    }
}

void MainWindow::timerSlot()
{
    this->updateCameras();

    while(ui->cameraCBox->count() > cameraNames.size())
        ui->cameraCBox->removeItem(cameraNames.size());
    while(ui->cameraCBox->count() < cameraNames.size())
        ui->cameraCBox->addItem(NULL);
    for(int i = 0; i < cameraNames.size(); i++)
        ui->cameraCBox->setItemText(i, cameraNames[i]);
}
