#ifndef SERIAL_H
#define SERIAL_H
#include <QtSerialPort/QSerialPort>
#include "datatype.h"
#include <QThread>

class Serial : public QObject
{
    Q_OBJECT
public:
	//functions
    Serial();
    ~Serial();
    int wb(const void* data, int len);
    int rb(void* dest, int len);
    int flush_uart(int out, int in);

	//Parameter
    QSerialPort *pserial;
signals:
    void PercentageSignals(char type,int value);
};

#endif // SERIAL_H
