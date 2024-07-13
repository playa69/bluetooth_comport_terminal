#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>

// Constants
const QString ADC1_CODE = "41444331";
const QString ADC2_CODE = "41444332";
const QString TARGET_CODE = "227d0a0d";
const QString HEX_PREFIX = "223a2022";

// CRC16 Calculation for ModBus protocol
int calculateModBusCRC16(int* data, unsigned short length)
{
    int crc = 0xFFFF;

    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (unsigned char j = 0; j < 8; j++) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    setWindowTitle("Bluetooth.pro/qt 4.8.2");

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (const QSerialPortInfo &port : ports) {
        portNames.append(port.portName());
    }
    ui->comboBox->addItems(portNames);

    QStringList adcOptions = { "ADC1", "ADC2" };
    ui->comboBox_2->addItems(adcOptions);
}

MainWindow::~MainWindow()
{
    serialPort.close();
    delete ui;
}

// Slot for handling "Connect" button click
void MainWindow::on_pushButton_2_clicked()
{
    QString selectedPort = ui->comboBox->currentText();
    serialPort.setPortName(selectedPort);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Unable to open port in ReadWrite mode.");
        return;
    }

    if (!serialPort.isOpen()) {
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Failed to open the serial port.");
        return;
    }

    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

// Slot for handling incoming serial messages
void MainWindow::receiveMessage()
{
    QByteArray data = serialPort.readAll();
    QString hexData = data.toHex();
    buffer.append(hexData);

    int index = buffer.indexOf(TARGET_CODE);

    if (index != -1) {
        static int messageCount = 0;
        messageCount++;

        QString message = buffer.mid(4, index - 4);
        message.remove(message.indexOf(HEX_PREFIX), HEX_PREFIX.length());
        QString adcValueHex = message.right(message.size() - 8);

        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append(QString::number(messageCount) + " " + data.left(8) + " " + data.mid(1, 1) + adcValueHex + data.mid(1, 1) + " };");

        bool ok;
        double conversionFactor = 1.250000000 / 0xFFFFFF;
        double adcValue = (adcValueHex.toInt(&ok, 16) & 0xFFFFFFF) >> 3;
        double finalValue = adcValue * conversionFactor;
        QString adcFinalValue = QString::number(finalValue);

        if (message.mid(0, 8) == ADC1_CODE) {  // ADC1
            ui->label_10->setText(adcFinalValue);
        } else if (message.mid(0, 8) == ADC2_CODE) {  // ADC2
            ui->label_11->setText(adcFinalValue);
        }

        buffer.remove(0, index + TARGET_CODE.size());
    }
}

// Slot for handling "Send Message" button click
void MainWindow::on_pushButton_clicked()
{
    QString message = ui->lineEdit_2->text();
    QString selectedADC = ui->comboBox_2->currentText();

    if (selectedADC == "ADC1") {
        message.prepend("{\"ADC1\": \"");
    } else if (selectedADC == "ADC2") {
        message.prepend("{\"ADC2\": \"");
    }

    ui->textBrowser->setTextColor(Qt::darkGreen);
    ui->textBrowser->append(message + "\"}");

    serialPort.write((message + "\"}").toUtf8());
}

// Slot for handling "Disconnect" button click
void MainWindow::on_pushButton_3_clicked()
{
    serialPort.close();
    buffer.clear();
}

// Slot for handling "Clear" button click
void MainWindow::on_pushButton_5_clicked()
{
    ui->textBrowser->clear();
}

// Slot for handling "Refresh Ports" button click
void MainWindow::on_pushButton_4_clicked()
{
    ui->comboBox->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (const QSerialPortInfo &port : ports) {
        portNames.append(port.portName());
    }
    ui->comboBox->addItems(portNames);
}
