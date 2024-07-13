# Bluetooth Serial Port Terminal

## Описание

Проект представляет собой терминал для работы с последовательным портом через Bluetooth, написанный с использованием фреймворка Qt. Программа позволяет подключаться к Bluetooth-устройствам, отправлять и получать сообщения через последовательный порт, а также отображать и обрабатывать данные с устройств ADC (Analog-to-Digital Converter).

## Основные функции

- Подключение к Bluetooth-устройствам через последовательный порт
- Отправка и получение сообщений через последовательный порт
- Обработка данных от устройств ADC
- Визуализация данных в интерфейсе пользователя

## Технологии

- C++
- Qt Framework
- Qt Serial Port
- Qt Bluetooth

## Как использовать

### Подготовка окружения

1. Убедитесь, что у вас установлен Qt версии 5.12.2 и MinGW 32-bit.
2. Склонируйте репозиторий:

   ```bash
   git clone https://github.com/playa69/bluetooth_comport_terminal.git
   ```
3. Перейдите в директорию проекта: cd bluetooth_comport_terminal/build-bluetooth-Desktop_Qt_5_12_2_MinGW_32_bit-Release/release
4. Юзай файл bluetooth.exe ./bluetooth.exe


## Упаковка приложения с помощью linuxdeployqt

### Установите linuxdeployqt:

```bash
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage
```


## Логика работы
### Инициализация интерфейса:
При запуске программы инициализируется графический интерфейс, настраивается окно и заполняется выпадающий список доступных последовательных портов и устройств ADC.

### Подключение к последовательному порту:
Пользователь выбирает последовательный порт и нажимает кнопку "Connect". Программа пытается открыть выбранный порт в режиме ReadWrite. При успешном открытии настраиваются параметры порта и устанавливается обработчик для приема данных.

### Прием сообщений:
В обработчике принимаемых сообщений данные считываются и конвертируются в шестнадцатеричный формат. Программа ищет в буфере определенный код (TARGET_CODE) и, если находит его, обрабатывает сообщение, извлекая значение ADC и выводя его в интерфейс.

### Отправка сообщений:
Пользователь вводит сообщение и выбирает устройство ADC из выпадающего списка. При нажатии кнопки "Send Message" сообщение отправляется через последовательный порт.

### Отключение и очистка:
Пользователь может отключиться от последовательного порта, очистить текстовый вывод или обновить список доступных портов.

### В качестве примера кода:
```
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
```
