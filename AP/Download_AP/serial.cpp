#include "serial.h"
#include "masterchip.h"

Serial::Serial()
{

}

Serial::~Serial()
{

}

int Serial::wb(const void* data, int len)
{
    DWORD wrtn;
    BOOL succ;
    int ret;

    wrtn = pserial->write((char*)data,len);
    succ = pserial->waitForBytesWritten(1000);
    PercentageSignals(RUN_START,len);

    ret = succ ? wrtn : -1;
    return ret;
}

int Serial::rb(void* dest, int len)
{
    int retry = 500;
    int rd =0,readSize =0;
    BOOL succ;
    int ret;

    do{
        pserial->waitForReadyRead(10);
        readSize = pserial->read((char*)dest+rd,len-rd);
        if(readSize < 0)
        {
            break;
        }
        else
        {
            rd+=readSize;
            if((rd == len) || (0 == readSize))
            {
                break;
            }
        }
        retry--;
    }while(retry>0);

    if(rd <0)
    {
        succ = 0;
    }
    else
    {
        succ = 1;
    }
    ret = succ ? rd : -1;
    return ret;
}

int Serial::flush_uart(int out, int in)
{
    if(out==FDISCARD)
    {
        pserial->clear(QSerialPort::Output);
    }
    else if(out)
    {
        pserial->waitForReadyRead(1000);
    }
    if(in)
    {
       // pserial->clear(QSerialPort::Input);
        pserial->readAll();
    }
    return 0;
}
