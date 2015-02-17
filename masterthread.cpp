#include <QThread>
#include <QMessageBox>
#include <QDateTime>
#include <time.h>
#include "masterthread.h"
#include "datatype.h"
#include "hc08sprg.h"

Q_DECLARE_METATYPE(QSerialPort::SerialPortError);

MasterThread::MasterThread(QObject *parent) : QThread(parent)
{
}

MasterThread::~MasterThread()
{
}

void MasterThread::setParameterAndRun(QString port,int Rate,sDownloaadInfo *ptr)
{
    Comport = port;
    Baudrate = Rate;

    info.FPGA_Enable = ptr->FPGA_Enable;
    info.Interface_Enable = ptr->Interface_Enable;
    info.ISP_Enable = ptr->ISP_Enable;
    info.Motor_Enable = ptr->Motor_Enable;
    info.Master_Enable = ptr->Master_Enable;
    info.FPGA_String = ptr->FPGA_String;
    info.Interface_String = ptr->Interface_String;
    info.ISP_String = ptr->ISP_String;
    info.Motor_String = ptr->Motor_String;
    info.Master_String = ptr->Master_String;

    if (!isRunning())
    {
        start();
    }
}

int MasterThread::Send_VISCA_Command (QSerialPort *pSerial,RS232CMDSTRUCT *SendCmd)
{
    unsigned char *uc_string;
    int index,retry = 200;
    unsigned char find_ack =0,find_complete =0;
    int retval = RET_FALSE;
    unsigned char SendBuf[16];
    unsigned char AckHeader = 0x80 + (SendCmd->ID<<4);
    memset(SendBuf, 0xFF, 16);

    SendBuf[0]	= 0x80 + SendCmd->ID;
    for (index=1; index<(SendCmd->length-1); index++)
    {
        SendBuf[index]	= SendCmd->parameter[index-1];
    }
    SendBuf[SendCmd->length-1]	= 0xFF;

    if(SendCmd->length == pSerial->write((char*)SendBuf,SendCmd->length))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            //Wait ack and complete
            responseData.clear();
            while(retry >0)
            {
                if(true == pSerial->waitForReadyRead(10))
                {
                    responseData += pSerial->readAll();
                    uc_string = (unsigned char*)responseData.data();

                    if(responseData.size() > 14)
                    {
                        qDebug()<<"Read Size error";
                        retval = RET_FALSE;
                        break;
                    }

                    for(index = 0;index <responseData.size();index++)
                    {
                        if((uc_string[index]== AckHeader) && ((uc_string[index+1]&0xF0) == 0x40) && (uc_string[index+2]== 0xFF))
                        {
                            find_ack = 1;

                        }
                        if((uc_string[index]== AckHeader) && ((uc_string[index+1]&0xF0) == 0x50) && (uc_string[index+2] == 0xFF))
                        {
                            find_complete = 1;                    
                        }
                    }
                }
                if(find_ack && find_complete)
                {
                    retval = RET_TRUE;
                    break;
                }
                retry--;
            }
        }
    }
    return retval;
}
int MasterThread::VertifyAck(QSerialPort *pSerial,int timeout)
{
    int index;
    unsigned char *uc_string;
    int retval = RET_ERROR;
    QDateTime Prev,now;
    qint64 DifferTime =0;
    Prev = QDateTime::currentDateTime();

    responseData.clear();
    while(DifferTime < (qint64)timeout)
    {
        if(true == pSerial->waitForReadyRead(1000))
        {
            responseData += pSerial->readAll();
            uc_string = (unsigned char*)responseData.data();
        }

        if(responseData.size() > (RS232_IN_QUEUE_SIZE-1))
        {
            qDebug()<<"Read Size error";
            retval = RET_ERROR;
            break;
        }

        for(index = 0;index <responseData.size();index++)
        {
            if((uc_string[index]== DOWNLOAD_FEEDBACK_HEADER) && (uc_string[index+DOWNLOAD_MODE_FEEDBACK_SIZE-1] == DOWNLOAD_MODE_END))
            {
                if(RET_TRUE == uc_string[index+1])
                {
                    retval = RET_TRUE;
                    break;
                }
                else
                {
                    retval = RET_FALSE;
                    break;
                }
            }
        }
        if(RET_ERROR != retval)
        {
            break;
        }
        now = QDateTime::currentDateTime();
        DifferTime = Prev.msecsTo(now);
    }

    return retval;
}

int MasterThread::SetFPGADownloadMode(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_TYPE;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_TYPE_P6_FPGA;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,10000);
        }
    }

    return retval;
}


int MasterThread::SetISPDownloadMode(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_TYPE;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_TYPE_P6_ISP;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,5000);
        }
    }

    return retval;
}

int MasterThread::SetISPInitial(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_CONTROL;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_CONTROL_P6_INIT;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,6000);
        }
    }

    return retval;
}

int MasterThread::SetISPChipErase(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_CONTROL;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_CONTROL_P6_CHIP_ERASE;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,50000);
        }
    }

    return retval;
}


int MasterThread::SetISPCheckSum(QSerialPort *pSerial,unsigned char checksum)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_CONTROL;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_CONTROL_P6_CHECKSUM;
    SendBuf[8] = checksum;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,30000);
        }
    }

    return retval;
}

int MasterThread::SetMotorDownloadMode(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_TYPE;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_TYPE_P6_MOTOR;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;


    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,10000);
        }
    }

    return retval;
}

int MasterThread::SetMasterDownloadMode(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_TYPE;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_TYPE_P6_MAIN;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;


    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = RET_TRUE;
        }
    }

    return retval;
}

int MasterThread::SetFPGAChipErase(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_CONTROL;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_CONTROL_P6_CHIP_ERASE;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = VertifyAck(pSerial,20000);
        }
    }

    return retval;
}

int MasterThread::SetMotorBypass(QSerialPort *pSerial)
{
    int retval = RET_FALSE;
    unsigned char SendBuf[CONTROL_CMD_PROTOCOL_SIZE+5];
    unsigned char *CheckSum = &SendBuf[9];

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    SendBuf[1] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[2] = CONTROL_CMD_PROTOCOL_P1;
    SendBuf[3] = CONTROL_CMD_PROTOCOL_P2;
    SendBuf[4] = CONTROL_CMD_PROTOCOL_P3;
    SendBuf[5] = CONTROL_CMD_PROTOCOL_P4;
    SendBuf[6] = CONTROL_CMD_PROTOCOL_P5_CONTROL;
    SendBuf[7] = CONTROL_CMD_PROTOCOL_CONTROL_P6_UART_BYPASS;
    SendBuf[8] = 0x00;
    *CheckSum = SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]+SendBuf[7]+SendBuf[8];
    SendBuf[10] = CONTROL_CMD_PROTOCOL_SIZE;
    SendBuf[CONTROL_CMD_PROTOCOL_SIZE+4] = DOWNLOAD_MODE_END;

    if((CONTROL_CMD_PROTOCOL_SIZE+5) == pSerial->write((char*)SendBuf,CONTROL_CMD_PROTOCOL_SIZE+5))
    {
        if(0 == pSerial->waitForBytesWritten(1000))
        {
            qDebug()<<"Serial port write buffer error";
            retval = RET_FALSE;
        }
        else
        {
            retval = RET_TRUE;
        }
    }

    return retval;
}

int MasterThread::RunFPGAFwDownlad(QSerialPort *pSerial,unsigned char *FileBuf,int filesize,unsigned char TransferSize,bool invert)
{
	int retval = RET_ERROR;
	unsigned char SendBuf[256];
	unsigned char *ptr,*pMsg,Data = 0;
	int fileReadSize =0;
	int VALUE_FF_Count=0,VALUE_00_Count=0,VALUE_All_Count = 0;
	int index;
    unsigned char *CheckSum;	
	int ProtocolDataSize = 0;
	int retry = 0;
	int MsgReady = 0;

	ptr= FileBuf;
	SendBuf[0] = DOWNLOAD_MODE_HEADER;

	while((fileReadSize<filesize) || (VALUE_All_Count >0))
	{
		if(fileReadSize<filesize)
		{
			VALUE_All_Count++;		
		}
		if(0x00 == *ptr)
		{
			if(fileReadSize<filesize)
			{
				VALUE_00_Count++;
			}

			if((fileReadSize == filesize) && (VALUE_All_Count == VALUE_00_Count))
			{
				//Send cout
				SendBuf[1] = 5; //Size
				SendBuf[2] = DOWNLOAD_DATA_MODE_00;
				SendBuf[3] = VALUE_00_Count>>24 & 0xFF;
				SendBuf[4] = VALUE_00_Count>>16 & 0xFF;
				SendBuf[5] = VALUE_00_Count>>8	 & 0xFF;
				SendBuf[6] = VALUE_00_Count	 & 0xFF;
				SendBuf[7] = ~(SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]);//Check sum;
				SendBuf[8] = 5; //Size
                SendBuf[9] = DOWNLOAD_MODE_END;
				VALUE_All_Count -= VALUE_00_Count;	
				VALUE_00_Count = 0;
				MsgReady = 1;
			}		
		}
		else
		{
			if(VALUE_00_Count >= 16)
			{
				//Send cout
				SendBuf[1] = 5; //Size
				SendBuf[2] = DOWNLOAD_DATA_MODE_00;
				SendBuf[3] = VALUE_00_Count>>24 & 0xFF;
				SendBuf[4] = VALUE_00_Count>>16 & 0xFF;
				SendBuf[5] = VALUE_00_Count>>8	 & 0xFF;
				SendBuf[6] = VALUE_00_Count	 & 0xFF;
				SendBuf[7] = ~(SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]);//Check sum;
				SendBuf[8] = 5; //Size
                SendBuf[9] = DOWNLOAD_MODE_END;
				VALUE_All_Count -= VALUE_00_Count;		
				MsgReady = 1;
			}
			VALUE_00_Count = 0;
		}
		
		if(0xFF == *ptr)
		{
			if(fileReadSize<filesize)
			{
				VALUE_FF_Count++;
			}
			if((fileReadSize == filesize) && (VALUE_All_Count == VALUE_FF_Count))
			{
				//Send cout
				SendBuf[1] = 5; //Size
				SendBuf[2] = DOWNLOAD_DATA_MODE_FF;
				SendBuf[3] = VALUE_FF_Count>>24 & 0xFF;
				SendBuf[4] = VALUE_FF_Count>>16 & 0xFF;
				SendBuf[5] = VALUE_FF_Count>>8	 & 0xFF;
				SendBuf[6] = VALUE_FF_Count	 & 0xFF;
				SendBuf[7] = ~(SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]);//Check sum;
				SendBuf[8] = 5; //Size
                SendBuf[9] = DOWNLOAD_MODE_END;
				VALUE_All_Count -= VALUE_FF_Count;
				VALUE_FF_Count = 0;
				MsgReady = 1;
				
			}			
		}
		else
		{
			if(VALUE_FF_Count > 15)
			{
				//Send cout
				SendBuf[1] = 5; //Size
				SendBuf[2] = DOWNLOAD_DATA_MODE_FF;
				SendBuf[3] = VALUE_FF_Count>>24 & 0xFF;
				SendBuf[4] = VALUE_FF_Count>>16 & 0xFF;
				SendBuf[5] = VALUE_FF_Count>>8	 & 0xFF;
				SendBuf[6] = VALUE_FF_Count	 & 0xFF;
				SendBuf[7] = ~(SendBuf[2]+SendBuf[3]+SendBuf[4]+SendBuf[5]+SendBuf[6]);//Check sum;
				SendBuf[8] = 5; //Size
                SendBuf[9] = DOWNLOAD_MODE_END;
				VALUE_All_Count -= VALUE_FF_Count;
				MsgReady = 1;
				
			}
			VALUE_FF_Count = 0;
		}

		if(((VALUE_All_Count != VALUE_00_Count) && (VALUE_All_Count != VALUE_FF_Count) && (0 == MsgReady)) ||((0 == MsgReady) && (fileReadSize == filesize)))
		{	
			if((VALUE_00_Count >= 15) || (VALUE_FF_Count>=15))
			{
				//Send Before data out
				ProtocolDataSize = VALUE_All_Count - VALUE_00_Count - VALUE_FF_Count;
				MsgReady = 1;
			}
			else if ((VALUE_All_Count == TransferSize) || (fileReadSize == filesize))
			{
				ProtocolDataSize = VALUE_All_Count;

				VALUE_00_Count = 0;
				VALUE_FF_Count = 0;				
				MsgReady = 1;
			}
			if(MsgReady)
			{
                SendBuf[1] = ProtocolDataSize+1; //Size
                SendBuf[4+ProtocolDataSize] = ProtocolDataSize+1;
				SendBuf[2] =  DOWNLOAD_DATA_MODE_ALL; 
                CheckSum = &SendBuf[3+ProtocolDataSize];
                SendBuf[5+ProtocolDataSize] = DOWNLOAD_MODE_END;
                pMsg = ptr-(VALUE_All_Count-1);


                *CheckSum = 0;
				*CheckSum+=SendBuf[2];
                for(index =0; index < ProtocolDataSize;index++)
		        {
		            if(true == invert)
		            {
                        Data=((*pMsg>>7)&0x01)|((*pMsg>>5)&0x02)|
		                            ((*pMsg>>3)&0x04)|((*pMsg>>1)&0x08)|
		                            ((*pMsg<<1)&0x10)|((*pMsg<<3)&0x20)|
		                            ((*pMsg<<5)&0x40)|((*pMsg<<7)&0x80);
		            }
		            else
		            {
		                Data = *pMsg;
		            }

		            SendBuf[index+3] = Data;
		            *CheckSum+=Data;
		            pMsg++;
		        }	
				*CheckSum = ~(*CheckSum);

				VALUE_All_Count -= ProtocolDataSize;
			}
			
		}
		if(fileReadSize<filesize)
		{
			fileReadSize++;		
			if(fileReadSize != filesize)
			{
				ptr++;
			}
		}

        //Send command to Camera
        if(MsgReady)
        {
        	MsgReady = 0;
	        retry = 3;			
	        while(retry > 0)
	        {
                if((SendBuf[1]+5) == pSerial->write((char*)SendBuf,SendBuf[1]+5))
	            {
	                if(0 == pSerial->waitForBytesWritten(1000))
	                {
	                    qDebug()<<"Serial port write buffer error";
	                    retval = RET_FALSE;
	                }
	                else
	                {
                        retval = VertifyAck(pSerial,5000);
	                }
	            }

	            if((RET_TRUE == retval) || (RET_FALSE == retval))
	            {
	                break;
	            }
	            retry--;
	        }
	        PercentageSignals(RUN_START,SendBuf[1]);
	        if(RET_TRUE != retval)
	        {
	            retval = RET_FALSE;
	            break;
	        }		
        }
	}
	return retval;
}

int MasterThread::RunFwDownlad(QSerialPort *pSerial,unsigned char *FileBuf,int filesize,unsigned char TransferSize,bool invert)
{
    int retval = RET_ERROR;
    unsigned char SendBuf[256];
    unsigned char *ptr,Data = 0;
    int ProtocolDataSize = TransferSize;
    unsigned char *CheckSum;
    unsigned char *Size;
    int retry = 0;
    int fileReadSize =0;
    int index;

    SendBuf[0] = DOWNLOAD_MODE_HEADER;
    CheckSum = &SendBuf[2+ProtocolDataSize];
    Size = &SendBuf[3+ProtocolDataSize];
    SendBuf[4+ProtocolDataSize] = DOWNLOAD_MODE_END;
    *CheckSum = 0;

    ptr= FileBuf;

    while(fileReadSize<filesize)
    {
        if((filesize - fileReadSize) < ProtocolDataSize)
        {
            ProtocolDataSize = filesize - fileReadSize;
            CheckSum = &SendBuf[2+ProtocolDataSize];
            Size = &SendBuf[3+ProtocolDataSize];
            SendBuf[4+ProtocolDataSize] = DOWNLOAD_MODE_END;
        }

        *CheckSum = 0;
        *Size = ProtocolDataSize;
        SendBuf[1] = ProtocolDataSize; //double check byte
        for(index =0; index < ProtocolDataSize;index++)
        {
            if(true == invert)
            {
                Data=((*ptr>>7)&0x01)|((*ptr>>5)&0x02)|
                            ((*ptr>>3)&0x04)|((*ptr>>1)&0x08)|
                            ((*ptr<<1)&0x10)|((*ptr<<3)&0x20)|
                            ((*ptr<<5)&0x40)|((*ptr<<7)&0x80);
            }
            else
            {
                Data = *ptr;
            }

            SendBuf[index+2] = Data;
            *CheckSum+=Data;
            ptr++;
        }
        *CheckSum = ~(*CheckSum);
        fileReadSize+=ProtocolDataSize;

        //Send command to Camera
        retry = 3;
        while(retry > 0)
        {
            if((*Size+5) == pSerial->write((char*)SendBuf,*Size+5))
            {
                if(0 == pSerial->waitForBytesWritten(1000))
                {
                    qDebug()<<"Serial port write buffer error";
                    retval = RET_FALSE;
                }
                else
                {
                    retval = VertifyAck(pSerial,5000);
                }
            }

            if((RET_TRUE == retval) || (RET_FALSE == retval))
            {
                break;
            }
            retry--;
        }
        PercentageSignals(RUN_START,*Size);
        if(RET_TRUE != retval)
        {
            retval = RET_FALSE;
            break;
        }
    }  
    return retval;
}

int ReadFile(FILE **fptr,int *pFileSize,unsigned char **pbuf,const char *c_string)
{
	int retval =RET_FALSE;
	int CurSize= 0,GetFileSize = 0;
	unsigned char *pCurrent;
	
	*fptr = fopen(c_string, "rb" );
	if(NULL != *fptr)
	{
	    //Get file size
	    fseek(*fptr, 0, SEEK_END);
	    *pFileSize = ftell(*fptr);
	    fseek (*fptr ,0, SEEK_SET);
	    if(*pFileSize > 0)
	    {
	        //Read data form files
	        *pbuf = (unsigned char*)malloc(*pFileSize);
	        CurSize = 0;
	        pCurrent = *pbuf;
	        while(CurSize < *pFileSize)
	        {
	            if((*pFileSize - CurSize) >= READ_FILE_BUFFER_SIZE)
	            {
	                GetFileSize = fread (pCurrent, sizeof(unsigned char), READ_FILE_BUFFER_SIZE, *fptr);
	            }
	            else
	            {
	                GetFileSize = fread (pCurrent, sizeof(unsigned char), *pFileSize - CurSize, *fptr);
	            }

	            pCurrent+=GetFileSize;
	            CurSize+=GetFileSize;
	        }
			retval =  RET_TRUE;
	    }
	    else
	    {
	        qDebug()<<"file size error\n";
	        retval =  RET_FALSE;
	    }	
		fclose(*fptr);
	}
	else
	{
		qDebug()<<"file open error\n";
		retval=RET_FALSE;
	}
	return retval;
}

void MasterThread::run()
{
    QSerialPort serial;
    RS232CMDSTRUCT CM_Bypass_Cmd;
    int retval = RET_FALSE;
    int retry;
	FILE **fptr;
	int *pFileSize;
	unsigned char **pbuf;

    totalSize= 0;

    serial.close();
    serial.setPortName(Comport);
    serial.setBaudRate(static_cast<QSerialPort::BaudRate>(Baudrate));
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    //Connect comport
    if (0 == serial.open(QIODevice::ReadWrite))
    {
        qDebug()<<"cannot open serial port\n";
    }
    else
    {
        //open and read files
        do{
            if(info.FPGA_Enable || info.Interface_Enable || info.ISP_Enable || info.Master_Enable || info.Motor_Enable)
            {                
                if(info.FPGA_Enable)
                {
                    if(NULL == info.FPGA_String)
                    {
                      qDebug()<<"FPGA file path not found\n";
                      break;
                    }
                    else
                    {
                        batyearray = info.FPGA_String.toLatin1();
                        c_string = batyearray.data();

						fptr = &pFPGAfilehdl;
						pFileSize = &FPGA_filesize;
						pbuf = &FPGAFileBuf;

						if(RET_TRUE != ReadFile(fptr,pFileSize,pbuf,c_string))
						{
							break;
						}
                    }
                    totalSize+=FPGA_filesize;
                }
                if(info.Interface_Enable)
                {
                    if(NULL == info.Interface_String)
                    {
                        qDebug()<<"Interface file path not found\n";
                        break;
                    }
                    else
                    {
                        batyearray = info.Interface_String.toLatin1();
                        c_string = batyearray.data();

						fptr = &pInterfacefilehdl;
						pFileSize = &Interface_filesize;
						pbuf = &InterfaceFileBuf;

						if(RET_TRUE != ReadFile(fptr,pFileSize,pbuf,c_string))
						{
							break;
						}

                    }
                    totalSize+=Interface_filesize;

                }
                if(info.ISP_Enable)
                {
                    if(NULL == info.ISP_String)
                    {
                        qDebug()<<"ISP file path not found\n";
                        break;
                    }
                    else
                    {
                        batyearray = info.ISP_String.toLatin1();
                        c_string = batyearray.data();

						fptr = &pIspfilehdl;
						pFileSize = &ISP_filesize;
						pbuf = &ISPFileBuf;

						if(RET_TRUE != ReadFile(fptr,pFileSize,pbuf,c_string))
						{
							break;
						}

                    }
                    totalSize+=ISP_filesize;
                }
                if(info.Motor_Enable)
                {
                    if(NULL == info.Motor_String)
                    {
                        qDebug()<<"Motor file path not found\n";
                        break;
                    }
                    else
                    {
                        batyearray = info.Motor_String.toLatin1();
                        c_string = batyearray.data();

						fptr = &pMotorfilehdl;
						pFileSize = &Motor_filesize;
						pbuf = &MotorFileBuf;

						if(RET_TRUE != ReadFile(fptr,pFileSize,pbuf,c_string))
						{
							break;
						}						

                    }
                    totalSize+=Motor_filesize;
                }
                if(info.Master_Enable)
                {
                    if(NULL == info.Master_String)
                    {
                        qDebug()<<"Master file path not found\n";
                        break;
                    }
                    else
                    {
                        batyearray = info.Master_String.toLatin1();
                        c_string = batyearray.data();

						fptr = &pMasterfilehdl;
						pFileSize = &Master_filesize;
						pbuf = &MasterFileBuf;

						if(RET_TRUE != ReadFile(fptr,pFileSize,pbuf,c_string))
						{
							break;
						}								
                    }
                    totalSize+=Master_filesize;
                }

                //======================//
                //    Start Download    //
                //======================//
                //Set Camera to download mode.
                memset(&CM_Bypass_Cmd, 0, sizeof(RS232CMDSTRUCT));
                CM_Bypass_Cmd.ID = 1 ;
                CM_Bypass_Cmd.parameter[0] = 0x01;
                CM_Bypass_Cmd.parameter[1] = 0x04;
                CM_Bypass_Cmd.parameter[2] = 0x75;
                CM_Bypass_Cmd.parameter[3] = 0xBB;
                CM_Bypass_Cmd.parameter[4] = 0xCC;
                CM_Bypass_Cmd.parameter[5] = 0xDD;
                CM_Bypass_Cmd.length = 8 ;
                retval = Send_VISCA_Command(&serial,&CM_Bypass_Cmd);
                if(RET_FALSE == retval)
                {
                    qDebug()<<"Cannot set the board to download mode !!\n";
                    break;
                }


                if(info.FPGA_Enable)
                {
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetFPGADownloadMode(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_FALSE == retval)
                    {
                        qDebug()<<"Set FPGA to donwload mode Error!!\n";
                        break;
                    }
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetFPGAChipErase(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"FPGA Erase Chip Fail!!\n";
                        break;
                    }

#if 1
                    retval = RunFPGAFwDownlad(&serial,FPGAFileBuf,FPGA_filesize,128,true);
#else
                    retval = RunFwDownlad(&serial,FPGAFileBuf,FPGA_filesize,128,true);
#endif
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"Download FPGA FW Fail\n";
                        break;
                    }
                }
                if(info.ISP_Enable)
                {
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetISPDownloadMode(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"Set ISP to donwload mode Error!!\n";
                        break;
                    }

                    //intiial
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetISPInitial(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"ISP Erase Chip Fail!!\n";
                        break;
                    }

                    //Erase
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetISPChipErase(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"ISP Erase Chip Fail!!\n";
                        break;
                    }

                    //Data //128 bytpe
                    retval = RunFwDownlad(&serial,ISPFileBuf+0x4020,ISP_filesize-0x4020,128,false);
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"Download ISP FW Fail\n";
                        break;
                    }

                    //checksum
                    retry = 3;
                    while(retry >0)
                    {
                        unsigned char checksum = 0;
                        for(int pos = 0;pos<(ISP_filesize-0x4020);pos++)
                        {
                               checksum^= *(ISPFileBuf+0x4020+pos);
                        }
                        retval = SetISPCheckSum(&serial,checksum);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"Downlad ISP write Checksum fail!!\n";
                        break;
                    }


                }
                if(info.Motor_Enable)
                {
                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetMotorDownloadMode(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_FALSE == retval)
                    {
                        qDebug()<<"Set FPGA to donwload mode Error!!\n";
                        break;
                    }

                    retry = 3;
                    while(retry >0)
                    {
                        retval = SetMotorBypass(&serial);
                        if(RET_TRUE == retval)
                        {
                            break;
                        }
                        retry--;
                    }
                    if(RET_TRUE != retval)
                    {
                        qDebug()<<"Motor Bypass Fail!!\n";
                        break;
                    }
                    Motor.pserial = &serial;
                    Motor.pfile_buf = MotorFileBuf;
                    Motor.file_size = Motor_filesize;
                    if(RET_TRUE!=Motor.MasterDownloadProcess())
                    {
                        qDebug()<<"Master FW Download Fail\n";
                        break;
                    }
                    retval = VertifyAck(&serial,10000);
                }
                if(info.Master_Enable)
                {
                    SetMasterDownloadMode(&serial);

                    Master.pserial = &serial;
                    Master.pfile_buf = MasterFileBuf;
                    Master.file_size = Master_filesize;
                    if(RET_TRUE!=Master.MasterDownloadProcess())
                    {
                        qDebug()<<"Master FW Download Fail\n";
                        break;
                    }
                }
                else
                {
                    qDebug()<<"not select download file\n";
                }
            }
        }while(0);
    }
    emit ProcessFinishSignals();

    qDebug()<<"exit==>ComPortThread::run";
}
