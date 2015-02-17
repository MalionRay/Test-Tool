#ifndef HC08SPRG
#define HC08SPRG

typedef unsigned char BYTE;

// types

#define MAX_NUM_BLOCKS	6	// max. no. of Flash blocks
#define SDID_UNDEF	-1		// end marker in SDID properties table
#define MAX_ADDRESS  0x400000
#define MAX_LENGTH   0x100
#define READ_ATTEMPS	4

#define SPEED_CALIB_MAX_ATTEMPS 20

enum{
    FAMILY_HC08=0,
    FAMILY_HCS08,
    FAMILY_CFV1,
    FAMILY_KINETIS,
    FAMILY_NEXT
};

#define BL_HC08			(1 << FAMILY_HC08)						// 00000001
#define BL_HC08_LARGE	(BL_HC08 | 0x1 << (FAMILY_HC08+1))		// 00000011

#define BL_HCS08		(1 << FAMILY_HCS08)						// 00000010
#define BL_HCS08_LONG	(BL_HCS08 | 0x1 << (FAMILY_HCS08+1))	// 00000110
#define BL_HCS08_LARGE	(BL_HCS08 | 0x2 << (FAMILY_HCS08+1))	// 00001010

#define BL_CFV1			(1 << FAMILY_CFV1)						// 00000100

#define BL_KINETIS_OLD		(1 << FAMILY_KINETIS)				// 00001000

#define BL_KINETIS			(0x01 | (1 << FAMILY_KINETIS))		// 00001001

#define BL_UNKNOWN		-1


typedef enum
{
    KINETIS_SERIES_K = 0x00,
    KINETIS_SERIES_L = 0x01,
    KINETIS_SERIES_M = 0x03,
    KINETIS_SERIES_W = 0x05,
    KINETIS_SERIES_V = 0x06,
    KINETIS_SERIES_E = 0xFF,
}KINETIS_SERIES;


#define CRC_HC08SPRG_SEED 0xffff

#define OPERATION_TIMEOUT 10

extern const char an2295Version[];

typedef struct
{
    long	sdid;
    short proto;
    const char* name;
    int noneVerAddrStart;
    int noneVerAddrEnd;
}
t_sdid;


typedef struct
{
    unsigned  mem_start[MAX_NUM_BLOCKS+1];	// start of usable flash
    unsigned  mem_end[MAX_NUM_BLOCKS+1];		// end of usable flash
    unsigned  erblk[MAX_NUM_BLOCKS];		// erase block size
    unsigned  wrblk[MAX_NUM_BLOCKS];		// write block size
    unsigned  bl_tbl;				// start of boot-loader table (private data and jump table)
    unsigned  int_vect_tbl;			// start of hard-wired interrupt vectors
    BYTE priv_data[8];					// 8 bytes of private info
    char targ_name[40];					// target name
    unsigned  num_blocks;				// number of flash memory blocks (BL protocol version 2 and up)
    unsigned  sdid;						// SDID number   (BL protocol version 2 and up)
    unsigned  sdidrev;					// SDID revision (BL protocol version 2 and up)
    unsigned  sdidsdieid;				// SDID die ID (Kinetis)
    KINETIS_SERIES sdidseriesid;		// SDID series ID (Kinetis)
    unsigned  sdidfamilyid;				// SDID family ID (Kinetis)
    unsigned  sdidsubfamilyid;			// SDID sub family ID (Kinetis)
    unsigned  sdidsramsize;				// SDIS sram size (Kinetis)
    unsigned  sdidpinId;				// SDIS pin Id (Kinetis)
} IDENT_DATA;

typedef struct {
    BYTE d[MAX_ADDRESS];	// data
    BYTE f[MAX_ADDRESS];	// valid flag 0=empty; 1=usercode; 2=systemcode
} BOARD_MEM_BLOCK;

typedef BOARD_MEM_BLOCK BOARD_MEM[MAX_NUM_BLOCKS + 1]; //The +1 is here to cover the original S08 functionality of vector redirection
                                                       //, so the last block will be used just for the original place ofinterrupt vectors
// globals
t_sdid * getSdidPtr(long sdid);
char const * Version_string(int bl_ver);
int read_s19(BOARD_MEM* pImage, IDENT_DATA * pIdent,unsigned char *buf,int size);
void reset_S19image(BOARD_MEM* pImage);
signed int S19_GetBlock(unsigned addr, IDENT_DATA * pIdent);
int	check_image(BOARD_MEM* pImage);

char const* kinetis_getPackageStr(IDENT_DATA* mcu_ident);
char const* kinetis_getRamSizeStr(IDENT_DATA* mcu_ident);
int kinetis_getDescrptionStr(char* pOutBuff, int maxSize, IDENT_DATA* mcu_ident, char delimiter, unsigned limited);
char const* getMcuNameStr(IDENT_DATA* mcu_ident, char delimiter, unsigned limited);
int is_vect_tbl_area(unsigned addr, IDENT_DATA * pIdent);

void CRC_AddByte(unsigned short *pCrc, unsigned char data);
void CRC_AddWord(unsigned short *pCrc, unsigned short data);
void CRC_Add3Bytes(unsigned short *pCrc, unsigned long data);
void CRC_AddLong(unsigned short *pCrc, unsigned long data);
void CRC_AddByteArray(unsigned short *pCrc, unsigned char* data, int size);
void CRC_ResetCRC(unsigned short *pCrc, unsigned short seed);
unsigned short CRC_GetCRC(unsigned short *pCrc);
#endif // HC08SPRG

