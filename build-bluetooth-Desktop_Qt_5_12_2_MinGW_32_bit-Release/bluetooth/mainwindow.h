#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include <QTimer>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();
    void receiveMessage();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    //void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

    //void deviceDiscovered(const QBluetoothDeviceInfo &device);

    //void startDeviceDiscovery();

    void on_pushButton_4_clicked();

signals:
    void error(const QString s);


private:
    Ui::MainWindow *ui;
    QTimer *tmr;
    QSerialPort serialPort;
    QSerialPortInfo info;
    QString buffer;
    QString code;
    QString x;
    QString y;
    QString x_0;
    QString y_0;
    QString hs;
    int codeSize;
    int after_hex1;
    int after_hex2;
    int after_hex3;
    int after_hex4;
    int after_hex5;
    int after_hex_wo_hs;
    int delta_x;
    int delta_y;
    int x1;
    int x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    int x8;
    int arr[8];
    bool ok;
};

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){
        QThread::usleep(usecs);
    }
    static void msleep(unsigned long msecs){
        QThread::msleep(msecs);
    }
    static void sleep(unsigned long secs){
        QThread::sleep(secs);}
};
#endif // MAINWINDOW_H
