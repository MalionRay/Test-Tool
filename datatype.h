#ifndef DATATYPE
#define DATATYPE
#include <qdebug.h>
#include "../../MCU_AppCode/src/VISCA/Download.h"
//#include "../../../branches/Arkai_Dev/MCU_AppCode/r971_isp_visca_download/src/VISCA/Download.h"


typedef unsigned char UINT8;
typedef unsigned int DWORD;
typedef bool BOOL;

#define RUN_BEGIN 0
#define RUN_START 1
#define RUN_STOP  2


// Constants
#define RS232_IN_QUEUE_SIZE		1024
#define RS232_OUT_QUEUE_SIZE	1024

#define RET_TRUE	1
#define RET_FALSE	0
#define RET_ERROR 	2

#define PROGRAM_ADDRESS_OFFECT 0x1000
#define READ_FILE_BUFFER_SIZE  1024
#define TRANFER_SIZE 		   76
#define STELLARIES_COMMAND_ACK 0xCC
#define STELLARIES_COMMAND_NAK 0x33

typedef struct tRS232CMDSTRUCT
{
    UINT8 ID;
    UINT8 parameter[14];
    int length;
} RS232CMDSTRUCT;


typedef struct tDownloadInfo
{
    bool FPGA_Enable;
    bool Interface_Enable;
    bool ISP_Enable;
    bool Motor_Enable;
    bool Master_Enable;
    QString FPGA_String;
    QString Interface_String;
    QString ISP_String;
    QString Motor_String;
    QString Master_String;

}sDownloaadInfo;

#endif // DATATYPE

