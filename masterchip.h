#ifndef MASTERCHIP_H
#define MASTERCHIP_H
#include <QtSerialPort/QSerialPort>
#include "serial.h"
#include "datatype.h"
#include "hc08sprg.h"

#define ACK			0xFC
#define FDISCARD 1
#define FDRAIN 2

class MasterChip : public Serial
{
public:
	//Functions
    MasterChip();
    ~MasterChip();
	int MasterDownloadProcess();
	
	//Parameter
    unsigned char *pfile_buf;
    int file_size;
private:
    int r16(unsigned* n);
    int w16(unsigned n);
    int r24(unsigned* n);
    int w24(unsigned n);
    int r32(unsigned * n);
    int w32(unsigned n);
    int erase_area(unsigned char block, unsigned long start, unsigned long end, unsigned char byS19);
    int prg_area(unsigned char block, unsigned long start, unsigned long end);
    int mcu_info_readIdentStr(void);
    int hook_reset(void);
    int calibrate_speed(void);
    int read_mcu_info(void);
    int erase_blk(unsigned long a);
    int prg_blk(unsigned long a, int len);
    int read_blk(unsigned a, int len, BYTE *dest);
    int	erase_mem(unsigned all);
    int	prg_mem(void);
    int read_area(unsigned char block, unsigned long start, unsigned long end, BOARD_MEM* pImage);
    int	unhook(void);
    int setup_vect_tbl(unsigned org_tbl, unsigned jump_tbl);

};

#endif // MASTERCHIP_H
