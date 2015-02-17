#ifndef MASTERTHREAD_H
#define MASTERTHREAD_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include "masterchip.h"
#include "datatype.h"


class MasterThread : public QThread
{
    Q_OBJECT

public:
    MasterThread(QObject *parent = 0);
    ~MasterThread();
    void setParameterAndRun(QString port,int Rate,sDownloaadInfo *ptr);
    int Send_VISCA_Command (QSerialPort *pSerial,RS232CMDSTRUCT *SendCmd);
    int VertifyAck(QSerialPort *pSerial,int timeout);
    int SetFPGADownloadMode(QSerialPort *pSerial);
    int SetISPDownloadMode(QSerialPort *pSerial);
    int SetISPInitial(QSerialPort *pSerial);
    int SetISPChipErase(QSerialPort *pSerial);
    int SetISPCheckSum(QSerialPort *pSerial,unsigned char checksum);
    int SetMotorDownloadMode(QSerialPort *pSerial);
    int SetMotorBypass(QSerialPort *pSerial);
    int SetFPGAChipErase(QSerialPort *pSerial);
    int SetMasterDownloadMode(QSerialPort *pSerial);
    int RunFwDownlad(QSerialPort *pSerial,unsigned char *DspFileBuf,int DSP_filesize,unsigned char TransferSize,bool invert);
	int RunFPGAFwDownlad(QSerialPort *pSerial,unsigned char *DspFileBuf,int DSP_filesize,unsigned char TransferSize,bool invert);

    MasterChip Master,Motor;
private:    
    QString Comport;
    int Baudrate;
    sDownloaadInfo info;
    QByteArray responseData;
    FILE *pFPGAfilehdl,*pInterfacefilehdl,*pIspfilehdl,*pMotorfilehdl,*pMasterfilehdl;
    int FPGA_filesize,Interface_filesize,ISP_filesize,Motor_filesize,Master_filesize;
    unsigned char *FPGAFileBuf,*InterfaceFileBuf,*ISPFileBuf,*MotorFileBuf,*MasterFileBuf;
    QByteArray batyearray;
    const char *c_string;
    unsigned int totalSize;
signals:
    void ProcessFinishSignals();
    void PercentageSignals(char type,int value);

protected :
    void run();
};

#endif // MASTERTHREAD_H
