#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QString>


#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>

int ModBusCRC16( int* p,
    unsigned short n)
{

    int w, i;
    unsigned char j;

    for (w = 0xFFFF, i = 0; i < n; i++)
    {
        w ^= p[i];
        for (j = 0; j < 8; j++) if (w & 1)
        {
                w >>= 1;
                w ^= 0xA001;
        }
            else w >>= 1;

    }
    return w;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    QWidget::setWindowTitle("Bluetooth.pro/qt 4.8.2");

    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox->addItems(stringPorts);
    QList<QString> ADCs;
    ADCs.append("ADC1");
    ADCs.append("ADC2");

    ui->comboBox_2->addItems(ADCs);
    //ui->comboBox_2->addItems("ADC2");
    //startDeviceDiscovery();
    //deviceDiscovered(const QBluetoothDeviceInfo &device);
}

MainWindow::~MainWindow()
{

    serialPort.close();
    delete ui;

}

/*
 void MainWindow::startDeviceDiscovery()
{

    // Create a discovery agent and connect to its signals
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    // Start a discovery
    discoveryAgent->start();

    //...
}

// In your local slot, read information about the found devices
void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo &device)
{

    ui->textBrowser->append( device.name() + device.address().toString() );

}
*/

void MainWindow::on_pushButton_2_clicked()
{
   /* QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    int numb = 0;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
        //ui->textBrowser->append(ports.at(i).manufacturer());
        //if(ports.at(i).manufacturer() == "Silicon Labs")
        if(ports.at(i).portName()== "COM4")
           numb = i;
    }
    serialPort.setPortName(ports.at(numb).portName()); */


    QString portName = ui->comboBox->currentText();

    serialPort.setPortName(portName);

    if(!serialPort.open(QIODevice::ReadWrite)){ //Проблема тут
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Не могу open(QIODevice::ReadWrite)");

    }

    if(!serialPort.isOpen()){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Проблема с serialPort.isOpen()");
    }
    else {

        //Defaultные параметры компорта
        serialPort.setBaudRate(QSerialPort::Baud115200);

        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setStopBits(QSerialPort::OneStop);
        serialPort.setParity(QSerialPort::NoParity);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);


        //code = ui->lineEdit->text();
        //codeSize = code.size();

        // Коннектимся к receiveMessage чтобы у нас все работало
        connect(&serialPort,SIGNAL(readyRead()),this,SLOT(receiveMessage()));

    }

}


static  int ij = 0;

void MainWindow::receiveMessage()
{
    QByteArray dataBA;
    //Sleeper::msleep(100);
    //Sleeper::msleep(500);
    dataBA = serialPort.readAll();

   // QString data(dataBA.toHex());
    //buffer.append(data);
    //int index = buffer.indexOf(code);
       //
   // ui->textBrowser->append(dataBA);


    QString data(dataBA.toHex()); // cast? QByteArray to QString
    buffer.append(data);
    code = "227d0a0d";
    int index = buffer.indexOf(code);

   if(index != -1){
       ij ++;

       QString message = buffer.mid(4,index-4);
       message.remove(message.indexOf("223a2022"),8);

       //ui->textBrowser->setTextColor(Qt::blue);
      // ui->textBrowser->append(message + "  ");

       QString ADC = message.right(message.size() - 8);

       ui->textBrowser->setTextColor(Qt::blue);
       ui->textBrowser->append(QString::number(ij) + "  " + dataBA.mid(0,8)  +"   " + dataBA.mid(1,1) + ADC + dataBA.mid(1,1) + " };");

       double nv; nv = 1.250000000/0xFFFFFF;
       double value; value = (ADC.toInt(&ok,16)&(0xFFFFFFF))>>3;
       double vnv; vnv = value*nv;
       QString ADC_final = QString::number(vnv);

       if(message.mid(0,8) == "41444331"){  //ADC1
        ui->label_10->setText(ADC_final);

       }
       else if(message.mid(0,8) == "41444332"){  //ADC2
        ui->label_11->setText(ADC_final);

       }


       /*

       else if(message.mid(0,8)=="?"){  //TMP1
      //ui->label_? ->setText(ADC);   // добавь лейбл в форме mainwindow.ui, чекни название и впиши
       }
       else if(message.mid(0,8)=="?"){  //TMP2
        ui->label_?->setText(ADC);
       }

       */

       buffer.remove(0,index+code.size());
  }

    /*
    if(buffer.length() > 19){
       QString message = buffer.mid(0,20);
       //STRING
       // можно юзать >>8, но я сделал так чтобы получить из пакета нужные переменные
       //в нужном типе данных
       x = buffer.mid(2,2) + buffer.mid(0,2);
       y = buffer.mid(6,2) + buffer.mid(4,2);
       x_0 = buffer.mid(10,2) + buffer.mid(8,2);
       y_0 = buffer.mid(14,2) + buffer.mid(12,2);
       hs = buffer.mid(18,2) + buffer.mid(16,2);
       QString after = x +" " + y +" " + x_0 + " " + y_0 + " " + hs;
       //integers
       // если хочешь посмотреть что что выводится юзай  ui->textBrowser->append(Qstring + QString::number( число ))
       after_hex1 = after.split(" ")[0].toInt(&ok,16);
       after_hex2 = after.split(" ")[1].toInt(&ok,16);
       after_hex3 = after.split(" ")[2].toInt(&ok,16);
       after_hex4 = after.split(" ")[3].toInt(&ok,16);
       after_hex5 = after.split(" ")[4].toInt(&ok,16);
       //integers
       x1 = buffer.mid(0,2).toInt(&ok,16);
       x2 = buffer.mid(2,2).toInt(&ok,16);
       x3 = buffer.mid(4,2).toInt(&ok,16);
       x4 = buffer.mid(6,2).toInt(&ok,16);
       x5 = buffer.mid(8,2).toInt(&ok,16);
       x6 = buffer.mid(10,2).toInt(&ok,16);
       x7 = buffer.mid(12,2).toInt(&ok,16);
       x8 = buffer.mid(14,2).toInt(&ok,16);
       int y_0_10 = y_0.toInt(&ok, 16);
       int x_0_10 = x_0.toInt(&ok, 16);
        // нужен массив для работы функции ModBusCRC16
       arr[0] = x1; arr[1] = x2;
       arr[2] = x3; arr[3] = x4; arr[4] = x5;
       arr[5] = x6; arr[6] = x7; arr[7] = x8;
       int MBCRC16 = ModBusCRC16(arr, 8);
       //integers
       delta_x = after_hex1 - after_hex3;
       delta_y = after_hex2 - after_hex4;
       QString x_s = QString::number(delta_x,10); //delta x
       QString y_s = QString::number(delta_y,10); //delta y
       QString x_0_st = QString::number(x_0_10); // x 0
       QString y_0_st = QString::number(y_0_10); // y 0

       // ---------------------------------------------------

       //ui->textBrowser->setTextColor(Qt::blue);
       //ui->textBrowser->append( "int  " + QString::number(MBCRC16,16));

       // ---------------------------------------------------

       ij++;
       // чекаем контрольную сумму
       if (MBCRC16 == after_hex5){
           ui->textBrowser->setTextColor(Qt::blue);
           ui->textBrowser->append("Package " + QString::number(ij) + " : " +message + " ---> " + after);
           ui->textBrowser->setTextColor(Qt::black);
           ui->textBrowser->append(".....................................................................................");
           ui->label_10->setText(x_s);
           ui->label_11->setText(y_s);
           ui->label_12->setText(y_0_st);
           ui->label_14->setText(x_0_st);
           buffer.remove(0,20);
          // ui->textBrowser->setTextColor(Qt::green);
          // ui->textBrowser->append("Truly CRC16");

       }
       else {
           Sleeper::msleep(50); // нужно поспать немного на случай если пришла большая посылка
           ui->textBrowser->setTextColor(Qt::red);
           ui->textBrowser->append("FAIL IN MBCRC16!!!");
           QString message2 = buffer.mid(0,100);
           ui->textBrowser->append(message2 );
            buffer.remove(0,100);
       }  // after_hex5 - это моя исходная хэш-сумма.


      // buffer.remove(0,100); //убьет если посылка пришла больше 10 байт. если пришла меньше 10 байт то она
       // не пройдет по хэшсумме

        // посылка может быть длиннее или короче. хеш-сумма может не биться. сделать
       // так чтобы дальнейшие были нормальные
       //чекаем осталось ли что-нибудь в буфере
       if(buffer.size()!=0){
           ui->textBrowser->setTextColor(Qt::red);

           ui->textBrowser->append("buffer not empty!!!");
           QString message2 = buffer.mid(0,100);
           ui->textBrowser->append(message2);
       }

    }
 */

}

void MainWindow::on_pushButton_clicked()
{
    QString message = ui->lineEdit_2->text();

    //int size = message.size();




    //QString tmplt = QString::number(0xbf) + QString::number(0x00)+ QString::number(0x00);
    //QString tmplt2 = QString::number(0xff);
    //QString mess = tmplt + QString::number(message.size(),16) + message + tmplt2;
    //{"TEST":   "30313233" }
    ui->textBrowser->setTextColor(Qt::darkGreen); // Color of message to send is green.

    QString text = ui->comboBox_2->currentText();
    if(text == "ADC1")
        message = message.prepend("{\"ADC1\": \"");
    else if(text == "ADC2")
        message = message.prepend("{\"ADC2\": \"");

   // ui->textBrowser->append(QString::number(0xbf) + QString::number(size,16)   + message +"\"}" +tmplt2);
    ui->textBrowser->append( message +"\"}" );


    // ЗАПИСЫВАЕМ ИНФОРМАЦИЮ
   //serialPort.write((QString::number(0xbf) + QString::number(size,16)  + message +"\" }" +tmplt2).toUtf8());
    serialPort.write((message +"\"}").toUtf8());

}


// Button of Disconnect
void MainWindow::on_pushButton_3_clicked()
{
    serialPort.close();
    buffer.remove(0,100);
    //tmr->stop();
}

/*
// Button of Refresh Ports
void Widget::on_pushButton_4_clicked()
{
    ui->comboBox->clear();
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){       // РЕФРЕШИМ ПОРТЫ
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox->addItems(stringPorts);
}
*/
// Button of Clear
void MainWindow:: on_pushButton_5_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_pushButton_4_clicked()
{



        ui->comboBox->clear();
        QList<QSerialPortInfo> ports1 = info.availablePorts();
        QList<QString> stringPorts1;
        for(int i = 0 ; i < ports1.size() ; i++){
            stringPorts1.append(ports1.at(i).portName());
        }
        ui->comboBox->addItems(stringPorts1);

}
