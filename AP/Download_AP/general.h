//==============================================================================
//
// Title:       general.h
// Purpose:     A short description of the interface.
//
// Created on:  2013/9/10 at ¤U¤È 01:22:42 by sysman.
// Copyright:   . All Rights Reserved.
//
//==============================================================================

#ifndef __general_H__
#define __general_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "../../MCU_AppCode/src/VISCA/Download.h"

//==============================================================================
// Constants
#define RS232_IN_QUEUE_SIZE		1024
#define RS232_OUT_QUEUE_SIZE	1024

#define RET_TRUE	1 
#define RET_FALSE	0
#define RET_ERROR 	2
		
//===============================================//
//	      Stellaris Serial Flash Loader         //
//===============================================//	
#define PROGRAM_ADDRESS_OFFECT 0x1000
#define READ_FILE_BUFFER_SIZE  1024
#define TRANFER_SIZE 		   76		
#define STELLARIES_COMMAND_ACK 0xCC
#define STELLARIES_COMMAND_NAK 0x33
		
#define COMMAND_RET_SUCCESS			0x40
#define COMMAND_RET_UNKNOWN_CMD		0x41
#define COMMAND_RET_INVALID_CMD		0x42
#define COMMAND_RET_INVALID_ADDR	0x43
#define COMMAND_RET_FLASH_FAIL		0x44	

//DSP
#define EEPRON_start_address	0x80
#define EEPRON_end_address		0x6000
#define RoundLoop				382
		
#define DSP_FW 0 
#define CM_FW  1
#define CB_FW  2

//==============================================================================
// Types
typedef struct
{
	unsigned char ID;
	unsigned char parameter[14];
	int length;
} RS232CMDSTRUCT;

typedef struct
{
	unsigned char buf;
	unsigned char use;
}def_SW_UART_RING_BUF;

//==============================================================================
// External variables
		

//==============================================================================
// Global functions
		
void CVICALLBACK COMCallbackFunction (int COMPort, int eventMask, void *callbackData);

int Declare_Your_Functions_Here (int x);


//===============================================//
//	      Stellaris Serial Flash Loader         //
//===============================================//
int Send_VISCA_Command (int ComPort, RS232CMDSTRUCT *SendCmd);
int SendDownloadCmdtoUART(int ComPort,unsigned char *SendBuf,int size);
int VertifyWriteData(unsigned char *SendBuf,int timeout);


//========================//
//      Processing        //
//========================//

#define PROCESSING_STR_START		1
#define PROCESSING_STR_STOP			2 
#define PROCESSING_STR_RUNNING		3
#define PROCESSING_STR_PING			4
#define PROCESSING_STR_LOADING		5
#define PROCESSING_STR_CHIP_ERASE	6

//Speed 
#define PROCESSING_STR_SPEED	6  




#ifdef __cplusplus
    }
#endif

#endif  /* ndef __general_H__ */
