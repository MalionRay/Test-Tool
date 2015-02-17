#include "masterchip.h"
#include "time.h"

//Serial
// see whether verify or not
int verify =1;

const char an2295Version[] = "$Version: 10.0.19.0$";

/////////////////////////////////////////////////
// bootloader commands

#define WR_DATA		'W'
#define RD_DATA		'R'
#define IDENT		'I'
#define QUIT		'Q'
#define ERASE		'E'
#define ACK			0xFC


t_sdid sdidprops[]=
{
    {0x000,	BL_HCS08,		"MC9S08CT16",						-1			,-1},
    {0x002,	BL_HCS08,		"MC9S08(GB/GT)(32-60), MC1321x",	-1			,-1},		// S08ICGV4 (GT16A)
    {0x003,	BL_HCS08,		"MC9S08(RC/RD/RE)(8/16)",			-1			,-1},		// xtal
    {0x003,	BL_HCS08,		"MC9S08(RC/RD/RE/RG)(32-60)",		-1			,-1},		// xtal
    {0x006,	BL_HCS08,		"MC3S08RX32",						-1			,-1},
    {0x007,	BL_HCS08,		"MC3S08RE16",						-1			,-1},
    {0x008,	BL_HCS08,		"MC9S08AW(16-60)",					-1			,-1},		// S08ICGV4
    {0x009,	BL_HCS08,		"MC9S08QG(4/8)",					-1			,-1},		// S08ICSV1
    {0x00A,	BL_HCS08,		"MC9S08JR12",						-1			,-1},
    {0x00C,	BL_HCS08,		"MC9S08LC(36-60)",					-1			,-1},		// S08ICGV4
    {0x00E,	BL_HCS08_LONG,	"MC9S08(DE/DN/DZ/DV/EN)(16-60)",	-1			,-1},		// S08MCGV1
    {0x010,	BL_HCS08,		"MC9S08RA16",						-1			,-1},
    {0x011,	BL_HCS08,		"MC9S08QD(4/8)",					-1			,-1},		// S08ICSV1
    {0x012,	BL_HCS08,		"MC9S08AC(8/16)",					0xFFBC		,0xFFBE},
    {0x013,	BL_HCS08_LONG,	"MC9S08(EL/SL)(8/16/32)",			-1			,-1},		// S08ICSV2
    {0x014,	BL_HCS08,		"MC9S08(SG/SH)(4/8)",				-1			,-1},		// S08ICSV2
    {0x015,	BL_HCS08_LARGE,	"MC9S08QE(64/96/128)",				-1			,-1},		// S08ICSV3
    {0x016,	BL_HCS08,		"MC9S08JM(16-60)",					-1			,-1},		// S08MCGV1
    {0x019,	BL_HCS08_LONG,	"MC9S08(DZ/DV)(96/128)",			-1			,-1},		// S08MCGV2
    {0x01A,	BL_HCS08,		"MC9S08(SG/SH)(16/32)",				-1			,-1},		// S08ICSV2
    {0x01B,	BL_HCS08_LARGE,	"MC9S08AC(96/128)",					0xFFBC		,0xFFBE},	// S08ICGV4
    {0x01C,	BL_HCS08,		"MC9S08QE(4/8)",					-1			,-1},		//
    {0x01D,	BL_HCS08,		"MC9S08AC(32-60)",					-1			,-1},		// S08ICGV4
    {0x01F,	BL_HCS08,		"MC9S08QE(16/32), MC1231x",			-1			,-1},
    {0x021,	BL_HCS08,		"MC9S08LL(8/16)",					-1			,-1},		// S08ICSV3
    {0x025,	BL_HCS08,		"MC9S08SE(4/8)",					-1			,-1},		// S08ICSV3
    {0x026,	BL_HCS08_LONG,  "MC9S08(LL/LH)(36/64)",				0xFFAC		,0xFFAF},	// S08ICSV3
    {0x027,	BL_HCS08,		"MC9S08MP(12-16)",					-1			,-1},		// S08ICSV3
    {0x028,	BL_HCS08_LARGE,	"MC1323x",							-1			,-1},		// S08ICSV3
    {0x02B,	BL_HCS08,		"MC9S08SC4",						-1			,-1},		// S08ICSV3
    {0x030,	BL_HCS08_LONG,  "MC9S08GW(32/64)",					-1			,-1},		// S08ICSV4
    {0x040,	BL_HCS08,		"MC9S08PT(60/32)",					-1			,-1},		//
    {0x041,	BL_HCS08_LARGE,	"MC13234",							-1			,-1},		// S08ICSV3
    {0xC01, BL_CFV1,		"MCF51EM32 development device",		-1			,-1},		// ICS
    {0xC02, BL_CFV1,		"MCF51EM(256/128)",					-1			,-1},		// ICS
    {0xC03, BL_CFV1,		"MCF51EM32",						-1			,-1},		// ICS
    {0xC05, BL_CFV1,		"MCF51AG(96/128)",					-1			,-1},		// ICS
    {0xC15,	BL_CFV1,		"MCF51QE(32/64/128)",				-1			,-1},		// S08ICSV3
    {0xC16,	BL_CFV1,		"MCF51JM(64/128) CAU disabled",		-1			,-1},		// S08MCGV3
    {0xC17,	BL_CFV1,		"MCF51JM(64/128) CAU enabled",		-1			,-1},		// S08MCGV3
    {0xC1B,	BL_CFV1,		"MCF51AC(128/256)",					-1			,-1},		// MCG
    {0xC2F, BL_CFV1,		"MCF51CN(64/128)",					-1			,-1},		// MCG
    {0x083,	BL_CFV1,		"MCF52254(AF80)(512)",				-1			,-1},
    {0x086,	BL_CFV1,		"MCF52252(AF80)(256)",				-1			,-1},
    {0x087,	BL_CFV1,		"MCF52254(CAF80)(512)",				-1			,-1},
    {0x08A,	BL_CFV1,		"MCF52252(CAF66)(256)",				-1			,-1},
    {0x08B,	BL_CFV1,		"MCF52258(AG80,VN80)(512)",			-1			,-1},
    {0x08E,	BL_CFV1,		"MCF52256(AG80,VN80)(256)",			-1			,-1},
    {0x08F,	BL_CFV1,		"MCF52258(CAG66,CVN66)(512)",		-1			,-1},
    {0x090,	BL_CFV1,		"MCF52256(CAG66,CVN66)(256)",		-1			,-1},
    {0x091,	BL_CFV1,		"MCF52255(CAF80)(512)",				-1			,-1},
    {0x092, BL_CFV1,		"MCF52259(CAG80,CVN80)(512)",		-1			,-1},
    {0x093,	BL_CFV1,		"MCF52268(AG80,VN80)(512)",			-1			,-1},

    {SDID_UNDEF,BL_UNKNOWN,	"unknown derivative",				-1			,-1}		// last entry do not remove
};

t_sdid sdidpropsKinetis[]=
{
    {0x000,	BL_KINETIS,		"Kinetis",							-1			,-1},
    {SDID_UNDEF,BL_UNKNOWN,	"Unknown derivative",				-1			,-1}		// last entry do not remove
};


////////////////////////////////////////////////
// global vars
IDENT_DATA ident =
{
    {0},
    {0},
    {0},
    {0},
    0,
    0,
    "",
    "",
    0,
    0,
    0,
    0,

};

BOARD_MEM  image;
int bl_version;
unsigned int msg_crc;
long warnings_cnt;
int bl_rcs;	/* read command supported flag */
int bl_crcs;	/* crc supported flag */
char defaultyes = 0;
unsigned short prog_crc;
char kinetisName[1024];
const char bl_ver_index[]=
{
    BL_HC08, BL_HC08_LARGE, BL_HCS08, BL_HCS08_LONG, BL_HCS08_LARGE, BL_CFV1, BL_KINETIS_OLD, BL_KINETIS, 0
};



char const *BL_VER_STRING[]=
{
    "HC08",			//1
    "large HC08",	//3
    "S08",			//2
    "long S08",		//6
    "large S08",	//10
    "ColdFire",		//4
    "Kinetis_Obsolete",		//8
    "Kinetis",		//9
    "unknown",		//0  protocol version 0 does not exist, also needs to be a last one = zero
};

char const *KINETIS_PINID_STRING[]=
{
     "16-pin",
     "24-pin",
     "32-pin",
     "44-pin",
     "48-pin",
     "64-pin",
     "80-pin",
     "81-pin",
     "100-pin",
     "104-pin",
     "144-pin",
     "Reserved",
     "196-pin",
     "Reserved",
     "256-pin",
     "Reserved"
};

char const *KINETIS_SRAMSIZE_STRING[]=
{
     "0.5 kB",
     "1 kB",
     "2 kB",
     "4 kB",
     "8 kB",
     "16 kB",
     "32 kB",
     "64 kB",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved"
};

#define KINETIS_SERIES_STRING_CNT 6

char const *KINETIS_SERIES_STRING[KINETIS_SERIES_STRING_CNT]=
{
     "Kinetis K",
     "Kinetis L",
     "Kinetis M",
     "Reserved",
     "Kinetis W",
     "Kinetis V",

};

#define KINETIS_K_FAM_STRING_CNT 8

char const *KINETIS_K_FAM_STRING[KINETIS_K_FAM_STRING_CNT]=
{
     "K1x Family (without tamper)",
     "K2x Family (without tamper)",
     "K3x Family or K1x/K6x Family (with tamper)",
     "K4x Family or K2x Family (with tamper)",
     "K6x Family (without tamper)",
     "K7x Family",
     "K50 and K52 Family",
     "K51 and K53 Family"
};

#define KINETIS_K_FAMILY_STRING_CNT 8

char const *KINETIS_K_FAMILY_STRING[KINETIS_K_FAMILY_STRING_CNT]=
{

     "Reserved",
     "basic",
     "USB",
     "Segment LCD",
     "USB and Segment LCD",
     "USB and ENET",
     "USB and ENET and Graphic LCD",
     "Reserved"
};

#define KINETIS_K_SUBFAMILY_STRING_CNT 8

char const *KINETIS_K_SUBFAMILY_STRING[KINETIS_K_SUBFAMILY_STRING_CNT]=
{

     "",
     "tamper detect",
     "",
     "tamper detect",
     "",
     "tamper detect",
     "",
     "Reserved"
};

#define KINETIS_L_FAMILY_STRING_CNT 8

char const *KINETIS_L_FAMILY_STRING[KINETIS_L_FAMILY_STRING_CNT]=
{

     "elementary",
     "basic",
     "USB",
     "Segment LCD",
     "USB and Segment LCD",
     "Reserved",
     "Reserved",
     "Reserved"
};

#define KINETIS_L_SUBFAMILY_STRING_CNT 8

char const *KINETIS_L_SUBFAMILY_STRING[KINETIS_L_SUBFAMILY_STRING_CNT]=
{

     "",
     "",
     "",
     "",
     "basic analog",
     "advanced analog",
     "",
     "Reserved"
};

#define KINETIS_M_FAMILY_STRING_CNT 8

char const *KINETIS_M_FAMILY_STRING[KINETIS_M_FAMILY_STRING_CNT]=
{

     "Reserved",
     "without LCD",
     "Reserved",
     "Segment LCD",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved"
};

#define KINETIS_M_SUBFAMILY_STRING_CNT 8

char const *KINETIS_M_SUBFAMILY_STRING[KINETIS_M_SUBFAMILY_STRING_CNT]=
{

     "Reserved",
     "Reserved",
     "2 AFE enabled",
     "3 AFE enabled",
     "4 AFE enabled",
     "Reserved",
     "Reserved",
     "Reserved"
};

unsigned progress = 0;
unsigned progress_parts = 1;


void CRC_AddByteArray(unsigned short *pCrc, unsigned char* data, int size)
{
    int i;

    for(i=0; i < size;i++)
        CRC_AddByte(pCrc, data[i]);
}

void CRC_AddLong(unsigned short *pCrc, unsigned long data)
{
    CRC_AddByte(pCrc, (unsigned char)((data >> 24) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)((data >> 16) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)((data >> 8) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)(data & 0xff));
}

void CRC_Add3Bytes(unsigned short *pCrc, unsigned long data)
{
    CRC_AddByte(pCrc, (unsigned char)((data >> 16) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)((data >> 8) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)(data & 0xff));
}

void CRC_AddWord(unsigned short *pCrc, unsigned short data)
{
    CRC_AddByte(pCrc, (unsigned char)((data >> 8) & 0xff));
    CRC_AddByte(pCrc, (unsigned char)(data & 0xff));
}

void CRC_AddByte(unsigned short *pCrc, unsigned char data)
{
    unsigned short temp;
    unsigned short quick;
    unsigned short tmp_crc = *pCrc;



    temp = (tmp_crc >> 8) ^ data;
    tmp_crc <<= 8;
    quick = temp ^ (temp >> 4);
    tmp_crc ^= quick;
    quick <<= 5;
    tmp_crc ^= quick;
    quick <<= 7;
    tmp_crc ^= quick;

    *pCrc = tmp_crc;
}

void CRC_ResetCRC(unsigned short *pCrc, unsigned short seed)
{
    *pCrc = seed;
}

unsigned short CRC_GetCRC(unsigned short *pCrc)
{
    return *pCrc;
}



static void Progress_Reset(unsigned progressParts)
{
    if(progressParts > 0)
        progress_parts = progressParts;
    else
        progress_parts = 1;

    progress = 0;
}

static void Progress_Set(unsigned progressPart, unsigned locProgress)
{
    unsigned tmp_progress;
    if(progressPart >= progress_parts)
        progressPart = progress_parts - 1;

    if(locProgress > 100)
        locProgress = 100;


    // compute finished parts of progress
    tmp_progress = (progressPart * 100) / progress_parts;


    tmp_progress += locProgress / progress_parts;

    if(tmp_progress != progress)
    {
        progress = tmp_progress;
    }

}

char const * Version_string(int bl_ver)
{
    int i=0;

    while(bl_ver_index[i])
    {
        if(bl_ver_index[i] == bl_ver)	// find the friendly name of the protocol
            break;
        i++;
    };

    return BL_VER_STRING[i];
}


int sdid2blver(long sdid)
{
    return getSdidPtr(sdid)->proto;
}



t_sdid * getSdidPtr(long sdid)
{
  int i=0;

  if ((bl_version == BL_KINETIS_OLD) || (bl_version == BL_KINETIS))
  {
    return &sdidpropsKinetis[0];
  }else
  {
    do
    {
        if(sdidprops[i].sdid == sdid)
            return &sdidprops[i];

    } while(sdidprops[++i].sdid != SDID_UNDEF);

    return &sdidprops[i];
  }
}


int snprintf(char* pOutBuff, int maxSize, const char * _Format, ...)
{
    int ret;
    va_list marker;

    va_start(marker, _Format);
    ret = vsnprintf(pOutBuff, maxSize, _Format, marker);
    va_end(marker);

    return ret;
}

int kinetis_getDescrptionStr(char* pOutBuff, int maxSize, IDENT_DATA* mcu_ident, char delimiter, unsigned limited)
{
    int maxSizeToPrint = maxSize;
    char * pText = pOutBuff;

    if(pOutBuff == NULL)
        return 0;

    if(maxSize == 0)
        return 0;

    if(mcu_ident == NULL)
        return 0;


    if(bl_version == BL_KINETIS_OLD)
    {
        maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis - Old version of protocol.");
    }
    else if(bl_version == BL_KINETIS)
    {
        switch(mcu_ident->sdidseriesid)
        {
        case 	KINETIS_SERIES_K:
            // check the version of Kinetis K MCU (detailed description or not)
            if((mcu_ident->sdidfamilyid) && (mcu_ident->sdidfamilyid < KINETIS_K_FAMILY_STRING_CNT) && (mcu_ident->sdidsubfamilyid < KINETIS_K_SUBFAMILY_STRING_CNT))
            {
                // detailed description
                if(limited)
                {
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis K%d%d %c SDID: 0x%08X [%s] rev:%u%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getPackageStr(&ident));
                }else
                {
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis K%d%d [%s %s]%c SDID: 0x%08X [%s] rev:%u%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        KINETIS_K_FAMILY_STRING[mcu_ident->sdidfamilyid], KINETIS_K_SUBFAMILY_STRING[mcu_ident->sdidsubfamilyid], delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getPackageStr(&ident));
                }
            }else
            {
                // old limited description of MCU
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "%s%c SDID:0x%08X [%s] rev:%u%c Package: %s.", KINETIS_K_FAM_STRING[(mcu_ident->sdid >> 4)& 0x07], delimiter, mcu_ident->sdid,
                    mcu_ident->targ_name ,ident.sdidrev, delimiter, kinetis_getPackageStr(&ident));
            }
            break;

        case 	KINETIS_SERIES_L:
            // check if the type is supported by string descriptions
            if((mcu_ident->sdidfamilyid < KINETIS_L_FAMILY_STRING_CNT) && (mcu_ident->sdidsubfamilyid < KINETIS_L_SUBFAMILY_STRING_CNT))
            {
                // detailed description
                if(limited)
                {
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis L%d%d%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
                }else
                {
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis L%d%d [%s %s]%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        KINETIS_L_FAMILY_STRING[mcu_ident->sdidfamilyid], KINETIS_L_SUBFAMILY_STRING[mcu_ident->sdidsubfamilyid], delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
                }
            }else
            {
                // limited description of MCU
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis L%d%d%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid, delimiter,
                    mcu_ident->sdid, mcu_ident->targ_name,	ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
            }
            break;

        case 	KINETIS_SERIES_M:
            // check if the type is supported by string descriptions
            if((mcu_ident->sdidfamilyid < KINETIS_M_FAMILY_STRING_CNT) && (mcu_ident->sdidsubfamilyid < KINETIS_M_SUBFAMILY_STRING_CNT))
            {
                // detailed description
                if(limited)
                {
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis M%d%d%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
                }else
                {
                    //maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis M");
                    maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis M%d%d [%s %s]%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid,
                        KINETIS_M_FAMILY_STRING[mcu_ident->sdidfamilyid], KINETIS_M_SUBFAMILY_STRING[mcu_ident->sdidsubfamilyid], delimiter, mcu_ident->sdid, mcu_ident->targ_name ,
                        ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
                }
            }else
            {
                // limited description of MCU
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis M%d%d%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", mcu_ident->sdidfamilyid, mcu_ident->sdidsubfamilyid, delimiter,
                    mcu_ident->sdid, mcu_ident->targ_name,	ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
            }
            break;

        case 	KINETIS_SERIES_W:
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis Wxx%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", delimiter, mcu_ident->sdid, mcu_ident->targ_name,
                    ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
            break;

        case 	KINETIS_SERIES_V:
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis Vxx%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", delimiter, mcu_ident->sdid, mcu_ident->targ_name,
                    ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
            break;

        case 	KINETIS_SERIES_E:
                maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis Exx%c SDID: 0x%08X [%s] rev:%u%c SRAM: %s%c Package: %s.", delimiter, mcu_ident->sdid, mcu_ident->targ_name,
                    ident.sdidrev, delimiter, kinetis_getRamSizeStr(&ident), delimiter, kinetis_getPackageStr(&ident));
            break;

        default:
            maxSizeToPrint -= snprintf(pText, maxSizeToPrint, "Kinetis - Unknown version.");
            break;

        }

    }
    return maxSize - maxSizeToPrint;
}

char const* getMcuNameStr(IDENT_DATA* mcu_ident, char delimiter, unsigned limited)
{
    if (((bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)))
    {
        kinetis_getDescrptionStr(kinetisName, 1024, mcu_ident, delimiter, limited);
        return kinetisName;
    }else
    {
        return getSdidPtr(mcu_ident->sdid)->name;
    }
}

char const* kinetis_getRamSizeStr(IDENT_DATA* mcu_ident)
{
    if ((bl_version == BL_KINETIS) && (mcu_ident->sdidsramsize != -1))
        return KINETIS_SRAMSIZE_STRING[mcu_ident->sdidsramsize];
    else
        return "Unknown";
}

char const* kinetis_getPackageStr(IDENT_DATA* mcu_ident)
{
    if (((bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) && (mcu_ident->sdidpinId != -1))
        return KINETIS_PINID_STRING[mcu_ident->sdidpinId];
    else
        return "Unknown";
}

/////////////////////////////////////////////////////////////////
// helper compare of unsigned

static unsigned umin(unsigned a, unsigned b)
{
    return a < b ? a : b;
}

//////////////////////////////////////////////////////////////////
// helper read 16 bit number
// return -1=error; 0=rd timeout; 2=success (2 bytes read)

int MasterChip::r16(unsigned* n)
{
    int ret;
    BYTE b[2];

    // we need 2 bytes (otherwise return -1 or 0)
    if((ret=rb(b, 2)) != 2)
        return ret < 0 ? ret : 0;

    // first was HI, then LO
    *n = b[0] << 8;
    *n += b[1];
    return 2;
}

/////////////////////////////////////////////////////////////////////
// helper write 16 bit number
// return -1=error; 0=wr timeout; 2=success (2 bytes written)

int MasterChip::w16(unsigned n)
{
    int ret;
    BYTE b[2];

    b[0] = (BYTE) ((n & 0xff00) >> 8);
    b[1] = (BYTE) (n & 0xff);
    if((ret=wb(b, 2)) != 2)
        return ret < 0 ? -1 : 0;

    return 2;
}


//////////////////////////////////////////////////////////////////
// helper read 24 bit number
// return -1=error; 0=rd timeout; 3=success (3 bytes read)

int MasterChip::r24(unsigned* n)
{
    int ret;
    BYTE b[3];

    // we need 3 bytes (otherwise return -1 or 0)
    if((ret=rb(b, 3)) != 3)
        return ret < 0 ? ret : 0;

    // first was HI, then LO
    *n =  b[0] << 16;
    *n += b[1] << 8;
    *n += b[2];
    return 3;
}

/////////////////////////////////////////////////////////////////////
// helper write 24 bit number
// return -1=error; 0=wr timeout; 3=success (3 bytes written)

int MasterChip::w24(unsigned n)
{
    int ret;
    BYTE b[3];

    b[0] = (BYTE) ((n & 0xff0000) >> 16);
    b[1] = (BYTE) ((n & 0xff00) >> 8);
    b[2] = (BYTE) (n & 0xff);
    if((ret=wb(b, 3)) != 3)
        return ret < 0 ? -1 : 0;

    return 3;
}

/////////////////////////////////////////////////////////////////////
// helper read 32 bit number
// return -1=error; 0=wr timeout; 3=success (3 bytes written)

int MasterChip::r32(unsigned * n)
{
    int ret;
    BYTE b[4];

    // we need 4 bytes (otherwise return -1 or 0)
    if((ret=rb(b, 4)) != 4)
        return ret < 0 ? ret : 0;

    // first was HI, then LO
    *n =  b[0] << 24;
    *n += b[1] << 16;
    *n += b[2] << 8;
    *n += b[3];
    return 3;
}

/////////////////////////////////////////////////////////////////////
// helper write 32 bit number
// return -1=error; 0=wr timeout; 4=success (4 bytes written)

int MasterChip::w32(unsigned n)
{
    int ret;
    BYTE b[4];

    b[0] = (BYTE) ((n & 0xff000000) >> 24);
    b[1] = (BYTE) ((n & 0xff0000) >> 16);
    b[2] = (BYTE) ((n & 0xff00) >> 8);
    b[3] = (BYTE) (n & 0xff);
    if((ret=wb(b, 4)) != 4)
        return ret < 0 ? -1 : 0;

    return 4;
}


//////////////////////////////////////////////////////////////
// wait for hc08 reset

// target send us ACK (0xfc) but in this time, it might be off-key up to factor 3
// so we might receive FF, FE, FC, F8, ..., 80, 0

static int could_be_ack(unsigned b)
{
    if(!b)
        return 0;

    b |= 0xff00;
    while(!(b&1))
        b = b >> 1;

    return (b & 0xff) == 0xff ? 1 : 0;
}


///////////////////////////////////////////////////////////////
// simple return-on-error macros

#define R8(var)		{if(rb(var, 1) != 1) return -1;}
#define R16(var)	{if(r16(var) <= 0) return -1;}
#define R24(var)	{if(r24(var) <= 0) return -1;}
#define R32(var)	{if(r32(var) <= 0) return -1;}

#define W8(n)		{ BYTE _x=n; if(wb(&_x, 1) != 1) return -1; }
#define W16(n)		{if(w16(n) <= 0) return -1;}
#define W24(n)		{if(w24(n) <= 0) return -1;}
#define W32(n)		{if(w32(n) <= 0) return -1;}
//////////////////////////////////////////////////////////////
// get basic info about target board


int MasterChip::mcu_info_readIdentStr(void)
{
    BYTE d;
    // read string
    unsigned int	i = 0;
    // read all name characters
    do
    {
        R8(&d);

        if(bl_crcs)
            CRC_AddByte(&prog_crc, d);

        // but store up to our buffer len chars only
        if(i < sizeof(ident.targ_name)-1)
            ident.targ_name[i++] = d;

    // zero terminated
    } while(d);

    return 0;
}

int MasterChip::read_mcu_info(void)
{
    BYTE d =0;

    // initialize memory space
    memset(&ident, 0, sizeof(ident));

    // issue command
    flush_uart(FDISCARD, FDISCARD);
    W8(IDENT);

    // first byte is protocol version
    R8(&d);
    bl_version = (int) (d & 0x0F);
    bl_rcs = (((int)d & 0x80) != 0);
    bl_crcs = (((int)d & 0x40) != 0);

    if(bl_crcs)
    {
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, 'I');
        CRC_AddByte(&prog_crc, d);
    }

    if(bl_version == BL_HC08)
    {
        // read identification structure for version 1 (HC08)
        ident.num_blocks = 1;
        R16(&ident.mem_start[0]);
        R16(&ident.mem_end[0]);
        R16(&ident.bl_tbl);
        R16(&ident.int_vect_tbl);
        R16(&ident.erblk[0]);
        R16(&ident.wrblk[0]);

        // read private data
        if(rb(&ident.priv_data, sizeof(ident.priv_data)) != sizeof(ident.priv_data))
            return -1;

        if(bl_crcs)
        {
            CRC_AddWord(&prog_crc, (unsigned short)ident.mem_start[0]);
            CRC_AddWord(&prog_crc, (unsigned short)ident.mem_end[0]);
            CRC_AddWord(&prog_crc, (unsigned short)ident.bl_tbl);
            CRC_AddWord(&prog_crc, (unsigned short)ident.int_vect_tbl);
            CRC_AddWord(&prog_crc, (unsigned short)ident.erblk[0]); //@Ray modify erblk to erblk[0]
            CRC_AddWord(&prog_crc, (unsigned short)ident.wrblk[0]); //@Ray modify erblk to wrblk[0]
            CRC_AddByteArray(&prog_crc, (unsigned char*)&(ident.priv_data), sizeof(ident.priv_data));
        }

        if(mcu_info_readIdentStr() < 0)
            return -1;

        if(bl_crcs)
        {
            R16(&msg_crc);
            if(msg_crc != prog_crc)
                return -2;
        }

    }
    else if((bl_version == BL_HCS08)
         || (bl_version == BL_HC08_LARGE)
         || (bl_version == BL_HCS08_LONG)
         || (bl_version == BL_HCS08_LARGE))
    {
        unsigned i, sdidh, sdidl;
        // read identification structure for version 2 (HCS08), 3 (large HC08), 6 (HCS08 with "non-binary" Erase/Write size)
        // 10 (HCS08 with >64kB memory), 12 (long HCS08)
        R8(&sdidh);
        R8(&sdidl);
        ident.sdid = ((sdidh & 0x0F) << 8) | (sdidl & 0xFF);
        ident.sdidrev = ((sdidh & 0xF0) >> 4);
        R8(&ident.num_blocks);
        if ((ident.num_blocks > MAX_NUM_BLOCKS) || (ident.num_blocks == 0))
            return -1;
        for(i=0; i<ident.num_blocks; i++)
        {
            if((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))	// versions 10 + 12 (HCS08 with >64kB memory) is 24-bit address here
            {
                R24(&ident.mem_start[i]);
                R24(&ident.mem_end[i]);
            }
            else
            {
                R16(&ident.mem_start[i]);
                R16(&ident.mem_end[i]);
            }

            ident.mem_end[i]--;
        }
        R16(&ident.bl_tbl);
        R16(&ident.int_vect_tbl);
        R16(&ident.erblk[0]);
        R16(&ident.wrblk[0]);

        if(ident.num_blocks > 1)
            for(i=1; i<ident.num_blocks; i++)
            {
                ident.erblk[i] = ident.erblk[0];
                ident.wrblk[i] = ident.wrblk[0];
            }

        if(bl_crcs)
        {
            CRC_AddByte(&prog_crc, sdidh);
            CRC_AddByte(&prog_crc, sdidl);
            CRC_AddByte(&prog_crc, (unsigned char)ident.num_blocks);

            for(i=0;i<ident.num_blocks;i++)
            {
                if((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                {
                    CRC_Add3Bytes(&prog_crc, ident.mem_start[i]);
                    CRC_Add3Bytes(&prog_crc, ident.mem_end[i]);
                }else
                {
                    CRC_AddWord(&prog_crc, (unsigned short)ident.mem_start[i]);
                    CRC_AddWord(&prog_crc, (unsigned short)ident.mem_end[i]);
                }
            }
            CRC_AddWord(&prog_crc, (unsigned short)ident.bl_tbl);
            CRC_AddWord(&prog_crc, (unsigned short)ident.int_vect_tbl);
            CRC_AddWord(&prog_crc, (unsigned short)ident.erblk[0]); //@Ray modify erblk to erblk[0]
            CRC_AddWord(&prog_crc, (unsigned short)ident.wrblk[0]); //@Ray modify wrblk to erblk[0]
        }

        if(mcu_info_readIdentStr() < 0)
            return -1;

        if(bl_crcs)
        {
            R16(&msg_crc);
            if(msg_crc != prog_crc)
                return -2;
        }

        if (bl_version != BL_HC08_LARGE)	// check only S08 families, not HC08
        {
            if (sdid2blver(ident.sdid) == BL_UNKNOWN)
            {
                fprintf(stderr, "\n\nWARNING! The SDID of the device [0x%03X] is not (yet) supported!\nCheck AN2295SW for update!\n\n", ident.sdid);
            }
            else
            {

                if (sdid2blver(ident.sdid) != bl_version)
                    fprintf(stderr, "\n\nWARNING! The protocol version [0x%02X] (%s) does not match the one associated\n"
                                        "with the device with following SDID [0x%08X].\n"
                                        "Protocol version for given SDID should be [0x%02X] (%s).\n"
                                        "Continuing with reported protocol version anyway.\n\n"
                                        "Something is screwed up, please report!\n\n\n",
                                        bl_version,
                                        Version_string(bl_version),
                                        ident.sdid,
                                        getSdidPtr(ident.sdid)->proto,
                                        Version_string(getSdidPtr(ident.sdid)->proto));
            }
        }
    }
    else if (bl_version == BL_CFV1)
    {
        unsigned i, sdidh, sdidl;
        // read identification structure for version 2 (HCS08), 3 (large HC08), 6 (HCS08 with "non-binary" Erase/Write size)
        // 10 (HCS08 with >64kB memory), 12 (long HCS08)
        R8(&sdidh);
        R8(&sdidl);
        ident.sdid = ((sdidh & 0x0F)<< 8)|(sdidl & 0xFF);
        ident.sdidrev = ((sdidh & 0xF0) >> 4);
        R32(&ident.num_blocks);
        if ((ident.num_blocks > MAX_NUM_BLOCKS) || (ident.num_blocks == 0))
            return -1;
        for(i=0; i<ident.num_blocks; i++)
        {
            R32(&ident.mem_start[i]);
            R32(&ident.mem_end[i]);
        }
        R32(&ident.bl_tbl);
        R32(&ident.int_vect_tbl);
        R32(&ident.erblk[0]);
        R32(&ident.wrblk[0]);

        if(ident.num_blocks > 1)
            for(i=1; i<ident.num_blocks; i++)
            {
                ident.erblk[i] = ident.erblk[0];
                ident.wrblk[i] = ident.wrblk[0];
            }
        if(bl_crcs)
        {
            CRC_AddByte(&prog_crc, sdidh);
            CRC_AddByte(&prog_crc, sdidl);
            CRC_AddLong(&prog_crc, ident.num_blocks);

            for(i=0;i<ident.num_blocks;i++)
            {
                CRC_AddLong(&prog_crc, ident.mem_start[i]);
                CRC_AddLong(&prog_crc, ident.mem_end[i]);
            }
            CRC_AddLong(&prog_crc, ident.bl_tbl);
            CRC_AddLong(&prog_crc, ident.int_vect_tbl);
            CRC_AddLong(&prog_crc, ident.erblk[0]);
            CRC_AddLong(&prog_crc, ident.wrblk[0]);
        }

        if(mcu_info_readIdentStr() < 0)
            return -1;

        if(bl_crcs)
        {
            R16(&msg_crc);
            if(msg_crc != prog_crc)
                return -2;
        }

        if (sdid2blver(ident.sdid) == BL_UNKNOWN)
        {
            fprintf(stderr, "\n\nWARNING! The SDID of the device [0x%03X] is not (yet) supported!\nCheck AN2295SW for update!\n\n", ident.sdid);
        }
        else
        {

            if (sdid2blver(ident.sdid) != bl_version)
                fprintf(stderr, "\n\nWARNING! The protocol version [0x%02X] (%s) does not match the one associated\n"
                                    "with the device with following SDID [0x%03X].\n"
                                    "Protocol version for given SDID should be [0x%02X] (%s).\n"
                                    "Continuing with reported protocol version anyway.\n\n"
                                    "Something is screwed up, please report!\n\n\n",
                                    bl_version,
                                    Version_string(bl_version),
                                    ident.sdid,
                                    getSdidPtr(ident.sdid)->proto,
                                    Version_string(getSdidPtr(ident.sdid)->proto));
        }
    }
    else if (bl_version == BL_KINETIS_OLD)
    {
        unsigned i, sdidh, sdidl;
        R8(&sdidh);
        R8(&sdidl);
        ident.sdid = ((sdidh & 0x0F)<< 8)|(sdidl & 0xFF);
        ident.sdidrev = ((sdidh & 0xF0) >> 4);
        ident.sdidsdieid = 0;				// SDID die ID (Kinetis)
        ident.sdidseriesid  = KINETIS_SERIES_K;				// SDID series ID (Kinetis)
        ident.sdidfamilyid = 0;				// SDID family ID (Kinetis)
        ident.sdidsubfamilyid = 0;			// SDID sub family ID (Kinetis)
        ident.sdidsramsize = 0;				// SDIS sram size (Kinetis)

        R32(&ident.num_blocks);
        if ((ident.num_blocks > 1) || (ident.num_blocks == 0))
            return -1;
        for(i=0; i<ident.num_blocks; i++)
        {
            R32(&ident.mem_start[i]);
            R32(&ident.mem_end[i]);
        }
        R32(&ident.bl_tbl);
        R32(&ident.int_vect_tbl);
        R32(&ident.erblk[0]);
        R32(&ident.wrblk[0]);

        if(bl_crcs)
        {
            CRC_AddByte(&prog_crc, sdidh);
            CRC_AddByte(&prog_crc, sdidl);
            CRC_AddLong(&prog_crc, ident.num_blocks);

            for(i=0;i<ident.num_blocks;i++)
            {
                CRC_AddLong(&prog_crc, ident.mem_start[i]);
                CRC_AddLong(&prog_crc, ident.mem_end[i]);
            }
            CRC_AddLong(&prog_crc, ident.bl_tbl);
            CRC_AddLong(&prog_crc, ident.int_vect_tbl);
            CRC_AddLong(&prog_crc, ident.erblk[0]);
            CRC_AddLong(&prog_crc, ident.wrblk[0]);
        }

        if(mcu_info_readIdentStr() < 0)
            return -1;

        if(bl_crcs)
        {
            R16(&msg_crc);
            if(msg_crc != prog_crc)
                return -2;
        }

        fprintf(stderr, "\n\nWARNING! The The device is using old style of Kinetis protocol. Please update device with latest sw of AN2295!");

    }
    else if (bl_version == BL_KINETIS)
    {
        unsigned i;
        R32(&ident.sdid);
        ident.sdidseriesid = (KINETIS_SERIES)((ident.sdid >> 20) & 0x0F);				// SDID series ID (Kinetis)

        switch(ident.sdidseriesid)
        {
            case KINETIS_SERIES_K:
                ident.sdidrev = ((ident.sdid >> 12) & 0x0F);
                ident.sdidsdieid = ((ident.sdid >> 7) & 0x1F);			// SDID die ID (Kinetis)
                ident.sdidfamilyid = ((ident.sdid >> 28) & 0x0F);		// SDID family ID (Kinetis)
                ident.sdidpinId = (ident.sdid & 0x0F);					// SDID pinId (Kinetis)
                ident.sdidsubfamilyid = ((ident.sdid >> 24) & 0x07);		// SDID sub family ID (Kinetis)
                ident.sdidsramsize = -1;								// SDIS sram size (Kinetis)
                break;

            case KINETIS_SERIES_L:
                ident.sdidrev = ((ident.sdid >> 12) & 0x0F);
                ident.sdidsdieid = ((ident.sdid >> 7) & 0x1F);			// SDID die ID (Kinetis)
                ident.sdidfamilyid = ((ident.sdid >> 28) & 0x0F);		// SDID family ID (Kinetis)
                ident.sdidpinId = (ident.sdid & 0x0F);					// SDID pinId (Kinetis)
                ident.sdidsubfamilyid = ((ident.sdid >> 24) & 0x0F);	// SDID sub family ID (Kinetis)
                ident.sdidsramsize = ((ident.sdid >> 16) & 0x0F);		// SDIS sram size (Kinetis)
                break;

            case KINETIS_SERIES_M:
                ident.sdidrev = ((ident.sdid >> 8) & 0x0F);
                ident.sdidsdieid = ((ident.sdid >> 4) & 0x0F);			// SDID die ID (Kinetis)
                ident.sdidfamilyid = ((ident.sdid >> 28) & 0x0F);		// SDID family ID (Kinetis)
                ident.sdidpinId = (ident.sdid & 0x0F);					// SDID pinId (Kinetis)
                ident.sdidsubfamilyid = ((ident.sdid >> 24) & 0x0F);	// SDID sub family ID (Kinetis)
                ident.sdidsramsize = ((ident.sdid >> 12) & 0x0F);		// SDIS sram size (Kinetis)
                break;

            default:
                ident.sdidrev = -1;
                ident.sdidsdieid = -1;									// SDID die ID (Kinetis)
                ident.sdidfamilyid = -1;								// SDID family ID (Kinetis)
                ident.sdidpinId = -1;									// SDID pinId (Kinetis)
                ident.sdidsubfamilyid = -1;								// SDID sub family ID (Kinetis)
                ident.sdidsramsize = -1;								// SDIS sram size (Kinetis)
                printf("This is not recognized series of Kinetis device. Becareful to use the tool with this device.");
                break;
        }



        R32(&ident.num_blocks);
        if ((ident.num_blocks > MAX_NUM_BLOCKS) || (ident.num_blocks == 0))
            return -1;

        for(i=0; i<ident.num_blocks; i++)
        {
            R32(&ident.mem_start[i]);
            R32(&ident.mem_end[i]);
            R32(&ident.erblk[i]);
            R32(&ident.wrblk[i]);
        }

        R32(&ident.bl_tbl);
        R32(&ident.int_vect_tbl);

        if(bl_crcs)
        {
            CRC_AddLong(&prog_crc, ident.sdid);
            CRC_AddLong(&prog_crc, ident.num_blocks);

            for(i=0;i<ident.num_blocks;i++)
            {
                CRC_AddLong(&prog_crc, ident.mem_start[i]);
                CRC_AddLong(&prog_crc, ident.mem_end[i]);
                CRC_AddLong(&prog_crc, ident.erblk[i]);
                CRC_AddLong(&prog_crc, ident.wrblk[i]);
            }
            CRC_AddLong(&prog_crc, ident.bl_tbl);
            CRC_AddLong(&prog_crc, ident.int_vect_tbl);
        }

        if(mcu_info_readIdentStr() < 0)
            return -1;

        if(bl_crcs)
        {
            R16(&msg_crc);
            if(msg_crc != prog_crc)
                return -2;
        }

    }
    else
    {
        fprintf(stderr, "\n\nERROR! Unknown protocol version: [0x%02X,0x%02X]! \nCheck AN2295SW for update!\n\n", bl_version,BL_KINETIS);
        return -1;
    }

    // Just to make borders for "dummy" block for original vector table
    ident.mem_start[MAX_NUM_BLOCKS] = 0;
    ident.mem_end[MAX_NUM_BLOCKS] = MAX_ADDRESS-1;


    return 0;
}

int is_vect_tbl_area(unsigned addr, IDENT_DATA * pIdent)
{
    if((bl_version == BL_HC08) || (bl_version == BL_HC08_LARGE) || (bl_version == BL_HCS08) || (bl_version == BL_HCS08_LONG) || (bl_version == BL_HCS08_LARGE ))
    {
        if(addr >= pIdent->bl_tbl && addr>= pIdent->mem_start[0])
            return 1;
    }

    else if(bl_version == BL_CFV1)
    {
        if(addr<= pIdent->mem_start[0])
            return 1;
    }

    else if((bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD))
    {
        if(addr<= pIdent->mem_start[0])
            return 1;
    }

    return 0;
}

//////////////////////////////////////////////////////////////
// masquerade vector table (work on memory image)
// the trick is to move any vector values from S19 file to
// special jump table located at ident.app_vect_tbl

int MasterChip::setup_vect_tbl(unsigned org_tbl, unsigned jump_tbl)
{
    unsigned src, dest;
    int any_hit = 0;
    int block;

    if((bl_version == BL_HC08) || (bl_version == BL_HC08_LARGE))
    {
        // start of jump table
        if((block = S19_GetBlock(jump_tbl, &ident)) < 0)
            return -1;

        dest = jump_tbl - ident.mem_start[block];



        // we store private data first
        if(ident.priv_data[0] != 0)
        {
            memcpy(&image[MAX_NUM_BLOCKS].d[dest], ident.priv_data, sizeof(ident.priv_data));
            dest += sizeof(ident.priv_data);
        }

        // and jump-table next
        for(src = 0xfffe; src >= org_tbl; src -= 2)
        {
            image[block].d[dest++] = 0xcc;	// jump instruction
            image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src];
            image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+1];

            // is the vector valid ?
            if(image[block].f[src] && image[MAX_NUM_BLOCKS].f[src+1])
                any_hit = 1;

            // original vector (from s19 image) is no longer valid
            image[MAX_NUM_BLOCKS].f[src] = 0;
            image[MAX_NUM_BLOCKS].f[src+1] = 0;
        }
    }
    else if ((bl_version == BL_HCS08) || (bl_version == BL_HCS08_LONG))
    {
        // start of jump table
        if((block = S19_GetBlock(jump_tbl, &ident)) < 0)
            return -1;

        dest = jump_tbl - ident.mem_start[block];


        // private data not stored (YET)
        // and jump-table next
        for(src = org_tbl; src <= 0xfffe ; src += 2)
        {
            // is the vector valid ?
            if(image[MAX_NUM_BLOCKS].f[dest] && image[MAX_NUM_BLOCKS].f[dest+1])
            {
                any_hit = 1;
                warnings_cnt++;
                printf("\nThe S19 image also contains redirected vectors. The automatic redirection is skipped now.");
                printf("\n");
                break;
            }
            dest += 2;


        }

        // start of jump table
        dest = jump_tbl - ident.mem_start[block];

        if(!any_hit)
            for(src = org_tbl; src <= 0xfffe ; src += 2)
            {
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src];
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+1];

                // is the vector valid ?
                if(image[MAX_NUM_BLOCKS].f[src] && image[MAX_NUM_BLOCKS].f[src+1])
                    any_hit = 1;

                // original vector (from s19 image) is no longer valid
                image[MAX_NUM_BLOCKS].f[src] = 0;
                image[MAX_NUM_BLOCKS].f[src+1] = 0;
            }
        else
            for(src = (jump_tbl + (0x10000 - org_tbl)); src <= 0xfffe ; src += 2)  // todo
            {
                // original vector (from s19 image) is no longer valid
                image[MAX_NUM_BLOCKS].f[src] = 0;
                image[MAX_NUM_BLOCKS].f[src+1] = 0;
            }
    }
    else if (bl_version == BL_HCS08_LARGE )	//at least QE family does not support the redirection, so use 'old' style
    {
// start of jump table
        if((block = S19_GetBlock(jump_tbl, &ident)) < 0)
            return -1;

        dest = jump_tbl - ident.mem_start[block];


        for(src = 0xfffe; src >= org_tbl; src -= 2)
        {
            // is the vector valid ?
            if(image[MAX_NUM_BLOCKS].f[dest] && image[MAX_NUM_BLOCKS].f[dest+1] && image[MAX_NUM_BLOCKS].f[dest+2])
            {
                warnings_cnt++;
                any_hit = 1;
                printf("The S19 image also contains redirected vectors. The automatic redirection is skipped now.");
                break;
            }
            dest += 3;


        }

        // start of jump table
        dest = jump_tbl - ident.mem_start[block];

        if(!any_hit)
            for(src = 0xfffe; src >= org_tbl; src -= 2)
            {
                image[block].d[dest++] = 0xcc;	// jump instruction
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src];
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+1];

                // is the vector valid ?
                if(image[MAX_NUM_BLOCKS].f[src] && image[MAX_NUM_BLOCKS].f[src+1])
                    any_hit = 1;

                // original vector (from s19 image) is no longer valid
                image[MAX_NUM_BLOCKS].f[src] = 0;
                image[MAX_NUM_BLOCKS].f[src+1] = 0;
            }
        else
            for(src = (jump_tbl + (((0x10000 - org_tbl)/2)*3)); src <= 0xffff ; src++)  // todo
            {
                // original vector (from s19 image) is no longer valid
                image[MAX_NUM_BLOCKS].f[src] = 0;
                image[MAX_NUM_BLOCKS].f[src+1] = 0;
            }

    }

    else if(bl_version == BL_CFV1)
    {
        // start of jump table
        if((block = S19_GetBlock(jump_tbl, &ident)) < 0)
            return -1;

        dest = jump_tbl - ident.mem_start[block];


        if(jump_tbl)
        {
            // and jump-table next
            for(src = 0x00 ; src < org_tbl ; src += 4)
            {
                image[block].d[dest++] = 0x4E;     // CFV1 jump instruction
                image[block].d[dest++] = 0xF9;
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src];
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+1];
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+2];
                image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+3];

                // is the vector valid ?
                if(image[block].f[src] && image[block].f[src+1] && image[block].f[src+2] &&  image[block].f[src+3])
                    any_hit = 1;

                // original vector (from s19 image) is no longer valid
                image[MAX_NUM_BLOCKS].f[src]   = 0;
                image[MAX_NUM_BLOCKS].f[src+1] = 0;
                image[MAX_NUM_BLOCKS].f[src+2] = 0;
                image[MAX_NUM_BLOCKS].f[src+3] = 0;
            }
        }
    }

    else if((bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD))
    {
        // start of jump table

        if((block = S19_GetBlock(jump_tbl, &ident)) < 0)
            return -1;

        dest = jump_tbl - ident.mem_start[block];


        if(jump_tbl != -1)
        {
            for(src = dest; src < dest + 0x400; src += 4)
            {
                // is the vector valid ?
                if(image[block].f[src] && image[block].f[src+1] && image[block].f[src+2] && image[block].f[src+3])
                {
                    warnings_cnt++;
                    any_hit = 1;
                    printf("The S19 image also contains redirected vectors. The automatic redirection is skipped now.");
                    break;
                }
            }

            dest = jump_tbl - ident.mem_start[block];

            if(!any_hit)
                // and jump-table next
                for(src = 0x0000 ; src < 0x0400 ; src += 4)
                {
                    image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src];
                    image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+1];
                    image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+2];
                    image[block].d[dest++] = image[MAX_NUM_BLOCKS].d[src+3];

                    // is the vector valid ?
                    if(image[MAX_NUM_BLOCKS].f[src] && image[MAX_NUM_BLOCKS].f[src+1] && image[MAX_NUM_BLOCKS].f[src+2] &&  image[MAX_NUM_BLOCKS].f[src+3])
                        any_hit = 1;

                    // original vector (from s19 image) is no longer valid
                    image[MAX_NUM_BLOCKS].f[src]   = 0;
                    image[MAX_NUM_BLOCKS].f[src+1] = 0;
                    image[MAX_NUM_BLOCKS].f[src+2] = 0;
                    image[MAX_NUM_BLOCKS].f[src+3] = 0;
                }
        }
    }

    // write the whole system area, but only if at least one vector was valid in the S19
    if(any_hit)
        memset(&image[block].f[jump_tbl - ident.mem_start[block]], 2, dest - (jump_tbl - ident.mem_start[block]));

    return 0;
}

//////////////////////////////////////////////////////////////
// erase single block around given address

int MasterChip::erase_blk(unsigned long a)
{
    int ret;
    int timeout;
    BYTE d;

    printf("\nerase_blk: 0x%08lx\n", a);

    if(S19_GetBlock(a, &ident) < 0)
        return -1;

    // issue command
    flush_uart(FDISCARD, FDISCARD);
    W8(ERASE);
    // and parameter
    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a)
    else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) W32(a)
    else W16(a);

    if(bl_crcs)
    {
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, ERASE);

        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) CRC_Add3Bytes(&prog_crc, a);
        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) CRC_AddLong(&prog_crc, a);
        else CRC_AddWord(&prog_crc, (unsigned short)a);

        W16(CRC_GetCRC(&prog_crc));
    }

    // wait for ack
    timeout = OPERATION_TIMEOUT;
    while((ret = rb(&d, 1)) == 0 && timeout--)
        ; // do nothing

    if(!ret)
        return -1;

    if(bl_crcs)
    {
        R16(&msg_crc);
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, d);

        if(msg_crc != prog_crc)
            return -2;
    }

    // it must be ACK character
    if(ret > 0 && d != ACK)
        ret = -1;

    // either -1 or 1
    return ret;
}

//////////////////////////////////////////////////////////////
// read memory block, return number of bytes read or -1

int MasterChip::read_blk(unsigned a, int len, BYTE *dest)
{
    if(len < 0 || len >= MAX_LENGTH || (S19_GetBlock(a, &ident) < 0))
        return -1;

    // issue command
    flush_uart(FDISCARD, FDISCARD);
    W8(RD_DATA);
    // and parameters
    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a)
    else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) W32(a)
    else W16(a);
    W8(len);

    if(bl_crcs)
    {
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, RD_DATA);

        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) CRC_Add3Bytes(&prog_crc, a);
        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) CRC_AddLong(&prog_crc, a);
        else CRC_AddWord(&prog_crc, (unsigned short)a);
        CRC_AddByte(&prog_crc, len);

        W16(CRC_GetCRC(&prog_crc));
    }

    // block read
    if(rb(dest, len) != len)
    {
        return -1;
    }

    if(bl_crcs)
    {
        R16(&msg_crc);
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByteArray(&prog_crc, dest, len);

        if(msg_crc != prog_crc)
        {
            return -2;
        }
    }

    // return number of bytes read (==requested)
    return len;
}

//////////////////////////////////////////////////////////////
// program single block

int MasterChip::prg_blk(unsigned long a, int len)
{
    int ret, timeout, block ;
    BYTE d;

    printf("\nprg_blk: 0x%08lx-0x%08lx\n", a, a+len-1);
    if(len < 0 || len >= MAX_LENGTH || (block = S19_GetBlock(a, &ident) < 0))
        return -1;

    // issue command
    flush_uart(FDISCARD, FDISCARD);
    W8(WR_DATA);
    // and parameters
    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a)
    else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) W32(a)
    else W16(a);
    W8(len);

    if(wb(&image[block].d[a - ident.mem_start[block]], len) != len)
        return -1;

    if(bl_crcs)
    {

        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, WR_DATA);

        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) CRC_Add3Bytes(&prog_crc, a);
        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS) || (bl_version == BL_KINETIS_OLD)) CRC_AddLong(&prog_crc, a);
        else CRC_AddWord(&prog_crc, (unsigned short)a);
        CRC_AddByte(&prog_crc, len);
        CRC_AddByteArray(&prog_crc, &image[block].d[a - ident.mem_start[block]], len);
        W16(CRC_GetCRC(&prog_crc));
    }

    // wait for ack
    timeout = OPERATION_TIMEOUT;
    while((ret = rb(&d, 1)) == 0 && timeout--)
        ; // do nothing

    if(!ret)
        return -1;

    if(bl_crcs)
    {
        R16(&msg_crc);
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, d);

        if(msg_crc != prog_crc)
            return -2;
    }

    // must be ACK character
    if(ret > 0 && d != ACK)
        ret = -1;

    // either -1 or 1
    return ret;
}

//////////////////////////////////////////////////////////////
// program one continuous area and give user a progess

int MasterChip::prg_area(unsigned char block, unsigned long start, unsigned long end)
{
    unsigned long i, wr, wr_start, wr_end, wr_next;
    unsigned long wr_one, written=0, total=0;

    int valid;

    int nvNoneVerStart = getSdidPtr(ident.sdid)->noneVerAddrStart;
    int nvNoneVerEnd = getSdidPtr(ident.sdid)->noneVerAddrEnd;


    printf("prg_area: 0x%08lx-0x%08lx\n", start, end);

    if(start == end)
        return 0;

    if((S19_GetBlock(start, &ident) < 0) || (S19_GetBlock(end, &ident) < 0))
        return -1;

    // count valid bytes
    for(i=start; i<end; i++)
        if(image[block].f[i - ident.mem_start[block]]) total++;


#define FLASHMODULO(x,y) ((unsigned long)(MAX_ADDRESS - (y) *(1 + ((MAX_ADDRESS-(unsigned long)((x)+(y))-1) / (y)))))


    // take start address as it is, but do further steps to erblk boundaries
    for(wr_start=start; wr_start<end; wr_start=wr_next)
    {
        // start of next erase block
        wr_next = FLASHMODULO(wr_start, ident.wrblk[block]);

        // anything to program in this erase block ?
        wr = wr_end = wr_start;

        valid = -1;

        for(i=wr_start; i<wr_next; i++)
        {
            // valid byte
            if(image[block].f[i - ident.mem_start[block]])
            {
                if(valid < 0)
                {
                    valid = 1;
                    wr = i;
                }

                wr_end = i + 1;
            }
        }

        // never pass after end
        if(wr_end > end)
            wr_end = end;

        // wr is now pointing to first valid byte (within current erase block)
        // wr_end is now pointing after last valid byte (within current erase block)
        if(valid > 0)
        {

            if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                printf("\rMemory programming block %d: W 0x%06lX %ld%%", block, wr, 100*written/total);
            else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                printf("\rMemory programming block %d: W 0x%08lX %ld%%", block, wr, 100*written/total);
            else
                printf("\rMemory programming block %d: W 0x%04lX %ld%%", block, wr, 100*written/total);

            fflush(stdout); printf("\n");

            wr_one = umin(wr_end, wr_next)-wr;
            if(prg_blk(wr, wr_one) < 0)
            {
                printf("\n");
                if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                    fprintf(stderr, "Can't program block %d at address 0x%06lX\n", block, wr);
                else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                    fprintf(stderr, "Can't program block %d at address 0x%08lX\n", block, wr);
                else
                    fprintf(stderr, "Can't program block %d at address 0x%04lX\n", block, wr);
                return -1;
            }

            if((bl_rcs) && (verify))	// read command implemented!
            {
                unsigned char rbuff[1024];
                unsigned int adr;

                if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                    printf("\rMemory reading block %d:     R 0x%06lX %ld%%", block, wr, 100*written/total);
                else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                    printf("\rMemory reading block %d:     R 0x%08lX %ld%%", block, wr, 100*written/total);
                else
                    printf("\rMemory reading block %d:     R 0x%04lX %ld%%", block, wr, 100*written/total);

                fflush(stdout); printf("\n");
                if (read_blk(wr, wr_one, rbuff) <0)
                {
                    printf("\n");
                    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                        fprintf(stderr, "Can't read block %d at address 0x%06lX\n", block, wr);
                    else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                        fprintf(stderr, "Can't read block %d at address 0x%08lX\n", block, wr);
                    else
                        fprintf(stderr, "Can't read block %d at address 0x%04lX\n", block, wr);
                    return -1;
                }

                for(adr = wr; adr < wr+wr_one; adr++)
                {

                    // over jump the none volatile trim address if known
                    if((nvNoneVerStart != -1) && (nvNoneVerEnd != -1))
                        if(((unsigned int)nvNoneVerStart <= adr) && ((unsigned int)nvNoneVerEnd >= adr))
                            continue;

                    if (image[block].d[adr - ident.mem_start[block]] != rbuff[adr-wr])
                    {
                        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                            fprintf(stderr, "\nVerification failed at block %d, address 0x%06X, image: [0x%02X], MCU: [0x%02X]\n", block, adr, image[block].d[adr], rbuff[adr-wr]);
                        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                            fprintf(stderr, "\nVerification failed at block %d, address 0x%08X, image: [0x%02X], MCU: [0x%02X]\n", block, adr, image[block].d[adr], rbuff[adr-wr]);
                        else
                            fprintf(stderr, "\nVerification failed at block %d, address 0x%04X, image: [0x%02X], MCU: [0x%02X]\n", block, adr, image[block].d[adr], rbuff[adr-wr]);
                        return -1;
                    }
                }
            }

        }else
        {
            wr_one = 0;
        }

        Progress_Set(block, 100*written/total);

        // the percentage-counting algorithm is not perfect, in some cases there might
        // be more than 100% achieved (if S19 file has holes within erblks = rare case)
        if((written += wr_one) > total)
            written = total;
    }

    return 0;
}

//////////////////////////////////////////////////////////////
// program one continuous area and give user a progess

int MasterChip::read_area(unsigned char block, unsigned long start, unsigned long end, BOARD_MEM* pImage)
{
    unsigned long	i, rd;
    unsigned long	rd_one, read=0, total=0;
    unsigned char	rbuff[0x100];

    unsigned long	attemps;
    int				res;

    if(!bl_rcs)	// read command implemented!
    {
        fprintf(stderr, "\nRead area failed - Read command is not supported");
        return -1;
    }

    printf("read_area: 0x%08lx-0x%08lx\n", start, end);


    if(start == end)
        return 0;

    if((S19_GetBlock(start, &ident) != block) || (S19_GetBlock(end, &ident) != block))
        return -1;

    total = end - start + 1;
    rd_one = 0xff;

    // take start address as it is, but do further steps to wr/rdblk boundaries

    for(rd = start; rd<end; rd += rd_one)
    {
        if(rd + rd_one > end)
            rd_one = end - rd;
        else
            rd_one = 0xff;

        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
            printf("\rRead Block %d: RD 0x%06lX %ld%%", block, rd, 100*read/total);
        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
            printf("\rRead Block %d: RD 0x%08lX %ld%%", block, rd, 100*read/total);
        else
            printf("\rRead Block %d: RD 0x%04lX %ld%%", block, rd, 100*read/total);
        fflush(stdout); printf("\n");

        attemps = READ_ATTEMPS;

        while(attemps-- && ((res = read_blk(rd, rd_one, rbuff)) < 0))
        {
        };

        if (res < 0)
        {
            printf("\n");
            if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                fprintf(stderr, "Can't read block %d at address 0x%06lX\n", block, rd);
            else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                fprintf(stderr, "Can't read block %d at address 0x%08lX\n", block, rd);
            else
                fprintf(stderr, "Can't read block %d at address 0x%04lX\n", block, rd);
            return -1;
        }

        for(i = 0; i < rd_one; i++)
        {
            (*pImage)[block].d[i + rd - ident.mem_start[block]] = rbuff[i];
            (*pImage)[block].f[i + rd - ident.mem_start[block]] = 0x01;
        }

        Progress_Set(block, 100*read/total);

        // the percentage-counting algorithm is not perfect, in some cases there might
        // be more than 100% achieved (if S19 file has holes within erblks = rare case)
        if((read += rd_one) > total)
            read = total;



    }

    return 0;
}

//////////////////////////////////////////////////////////////
// program one continuous area and give user a progess

int MasterChip::erase_area(unsigned char block, unsigned long start, unsigned long end, unsigned char byS19)
{
    unsigned long i, er, er_next;
    unsigned long erased=0, total=0;
    unsigned int er_block;
    printf("erase_area: 0x%08lx-0x%08lx\n", start, end);

    if(start == end)
        return 0;

    if((S19_GetBlock(start, &ident) < 0) || (S19_GetBlock(end, &ident) < 0))
        return -1;

    // count valid bytes
    for(i=start; i<end; i++)
        total++;


#define FLASHMODULO(x,y) ((unsigned long)(MAX_ADDRESS - (y) *(1 + ((MAX_ADDRESS-(unsigned long)((x)+(y))-1) / (y)))))


    // take start address as it is, but do further steps to wr/rdblk boundaries
    for(er=start; er<end; er=er_next)
    {
        // start of next erase block
        er_next = FLASHMODULO(er,ident.erblk[block]);

        if(byS19)
        {
            er_block = 0;

            for(i=er; i<er_next; i++)
            {
                // valid byte
                if(image[block].f[i - ident.mem_start[block]])
                {
                    er_block = 1;
                    break;
                }
            }

            if(er_block == 0)
            {
                continue;
            }
        }
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
            printf("\rMemory block %d erasing: E 0x%06lX %ld%%", block, er, 100*erased/total);
        else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
            printf("\rMemory block %d erasing: E 0x%08lX %ld%%", block, er, 100*erased/total);
        else
            printf("\rMemory block %d erasing: E 0x%04lX %ld%%", block, er, 100*erased/total);
        fflush(stdout); printf("\n");

        // use the first valid-byte address
        if(erase_blk(er) < 0)
        {
            printf("\n");
            if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
                fprintf(stderr, "Can't erase block %d at address 0x%06lX\n", block, er);
            else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
                fprintf(stderr, "Can't erase block %d at address 0x%08lX\n", block, er);
            else
                fprintf(stderr, "Can't erase block %d at address 0x%04lX\n", block, er);
            return -1;
        }

        Progress_Set(block, 100*erased/total);

        // the percentage-counting algorithm is not perfect, in some cases there might
        // be more than 100% achieved (if S19 file has holes within erblks = rare case)
        if((erased += ident.erblk[block]) > total)
            erased = total;
    }

    return 0;
}

//////////////////////////////////////////////////////////////
// program all

int	MasterChip::prg_mem(void)
{
    unsigned s,e, block;

    if(erase_mem(0) < 0)
        return -1;

   // Progress_Reset(1);


    for(block = 0; block <ident.num_blocks; block++)
    {
        // find first and last valid byte
        for(s=0; s<MAX_ADDRESS; s++)
            if(image[block].f[s]) break;
        for(e=MAX_ADDRESS; e>s; e--)
            if(image[block].f[e-1]) break;

        if(s < MAX_ADDRESS)
        {
            s += ident.mem_start[block];
            e += ident.mem_start[block];

            if(prg_area(block, s, e) < 0)
                return -1;


            if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
            {
                printf("\rMemory block %d programmed:              100%%                     \n", block);
                if((bl_rcs) && (verify))
                    printf("\rMemory block %d verified:                OK                    \n", block);
            }
            else if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
            {
                printf("\rMemory block %d programmed:             100%%                      \n", block);
                if((bl_rcs) && (verify))
                    printf("\rMemory block %d verified:               OK                     \n", block);
            }

            else
            {
                printf("\rMemory block %d programmed:                   100%%                \n", block);
                if((bl_rcs) && (verify))
                    printf("\rMemory block %d verified:                 OK                   \n", block);
            }
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////
// erase memory

int	MasterChip::erase_mem(unsigned all)
{
    unsigned i;
    int res;

    if(all)
    {
        Progress_Reset(ident.num_blocks);

        for(i=0;i<ident.num_blocks;i++)
        {
            if((res = erase_area(i, ident.mem_start[i], ident.mem_end[i], 0)) < 0)
            {
                printf("\rMemory is NOT erased.\n");
                return res;
            }
        }
    }else
    {
        unsigned addr_lo, addr_hi, block;

        for(block = 0; block < ident.num_blocks; block++)
        {
            Progress_Reset(1);

            // find first and last valid byte
            for(addr_lo=0; addr_lo<MAX_ADDRESS; addr_lo++)
                if(image[block].f[addr_lo]) break;
            for(addr_hi=MAX_ADDRESS; addr_hi>addr_lo; addr_hi--)
                if(image[block].f[addr_hi-1]) break;

            if(addr_lo < MAX_ADDRESS)
            {
                addr_lo += ident.mem_start[block];
                addr_hi += ident.mem_start[block];

                if((res = erase_area(block, addr_lo, addr_hi, 1)) < 0)
                {
                    printf("\rMemory is NOT erased.\n");
                    return res;
                }
            }
        }
    }
    printf("\rMemory is erased.                              \n");

    return 0;
}

//////////////////////////////////////////////////////////////
// print ID$, only printable char, max. 255 chars

void cleanprint(char * str)
{
int cnt = 255;

    while(*str && cnt--)
    {
        if (isprint(*str))
            putchar(*str);
        else
            putchar('.');

        str++;
    }
}

//////////////////////////////////////////////////////////////
// check if code goes to valid memory only

int	check_image(BOARD_MEM* pImage)
{
    unsigned a, i, f;

    unsigned s,e;

    int	b;

    char* scannm;
    char id;


    // find first and last valid byte
    for(s=0; s<MAX_ADDRESS; s++)
        if((*pImage)[0].f[s]) break;
    for(e=MAX_ADDRESS; e>s; e--)
        if((*pImage)[0].f[e-1]) break;


    // skip COM if specified and scan number and mode (optional)
    id = strnicmp((const char *)(*pImage)[0].d+s, "ID$:", 4);

    if (id == 0)
    {
        scannm = (char *)(*pImage)[0].d+s+4;	/* string ptr. after ID$ */

        printf("\r\nMCU software version: ");
        cleanprint(scannm);
        putchar('\n');
    }

    //sscanf(scannm, "%i%c%c", &portNum, &modemode, &serial_mode);


    for(i=0; i<ident.num_blocks; i++)
    {
        for(a=0; a<MAX_ADDRESS; a++)   //TBD
        {
            if((*pImage)[i].f[a]==1)
            {
                f = 0;
                if (((a + ident.mem_start[i]) >= ident.mem_start[i]) && ( (a + ident.mem_start[i]) <=ident.mem_end[i]))
                {
                    f = 1;
                    break;
                }

                if(f == 0)
                    goto err_hit;	//location at address a won't fit any memory block!
            }
        }
    }

    // now check collision with interrupt vector tables

    // find first and last valid byte of interrupt vector block
    for(s=0; s<MAX_ADDRESS; s++)
        if((*pImage)[MAX_NUM_BLOCKS].f[s]) break;
    for(e=MAX_ADDRESS; e>s; e--)
        if((*pImage)[MAX_NUM_BLOCKS].f[e-1]) break;

    if(s < MAX_ADDRESS)
    {
        b = S19_GetBlock(s, &ident);

        if((b != MAX_NUM_BLOCKS) || (b < 0 ))
            goto err_hit;


        b = S19_GetBlock(e, &ident);

        if((b != MAX_NUM_BLOCKS) || (b < 0 ))
            goto err_hit;
    }
    // no error
    return 0;

err_hit:
    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))
        fprintf(stderr, "\nWARNING! S19 image will not fit into available memory (at address 0x%06X)!\n", a);

    else if((bl_version == BL_CFV1) || (bl_version == BL_KINETIS))
        fprintf(stderr, "\nWARNING! S19 image will not fit into available memory (at address 0x%08X)!\n", a);

    else
        fprintf(stderr, "\nWARNING! S19 image will not fit into available memory (at address 0x%04X)!\n", a);
    return -1;
}

//////////////////////////////////////////////////////////////
// exit program mode, start application

int	MasterChip::unhook(void)
{
    if(bl_crcs)
    {
        CRC_ResetCRC(&prog_crc, CRC_HC08SPRG_SEED);
        CRC_AddByte(&prog_crc, QUIT);
        W8(QUIT);
        W16(CRC_GetCRC(&prog_crc));
    }else
        W8(QUIT);

    return 0;
}



MasterChip::MasterChip()
{

}

MasterChip::~MasterChip()
{

}

int MasterChip::hook_reset(void)
{
#define MAX_TIMEOUT 10

    int ret;
    int timeout = MAX_TIMEOUT;
    UINT8 d =0;
    __time64_t t0, t;

    //printf("Waiting for HC(S)08 reset ACK...");
    printf("Waiting for HC(S)08 reset ACK (timeout: %ds)...\n", MAX_TIMEOUT);
    fflush(stdout);

    _time64(&t0);

    // ignore timeouts
    while((ret=rb(&d, 1)) >= 0)
    {
        if(ret > 0)
        {
            printf("received 0x%02x ", d);
            if(could_be_ack(d))
            {
                if(d == ACK) printf("(good).\n");
                else printf("(off-key).\n");
                break;
            }
            else
            {
                printf("(ignoring).\n");
            }
        }

        _time64(&t);

        if(t > t0)
        {
            //printf(".");
            timeout--;
            printf("\rWaiting for HC(S)08/ColdFire/Kinetis reset ACK (timeout: %ds)...", timeout);
            t0 = t;
            if(timeout <= 0)
            {
                printf("\n");
                fprintf(stderr, "Error receiving reset ACK - TIMEOUT.\n");
                return -1;
            }

        }

    }

    if(ret < 0)
    {
        printf("\n");
        fprintf(stderr, "Error receiving reset ACK.\n");
        return -1;
    }

    // enter interactive mode
    if(wb(&d, 1) <= 0)
    {
        fprintf(stderr, "Error sending locking ACK.\n");
        return -1;
    }


    // wait for finish
    flush_uart(FDRAIN, 0);

    // success
    return 1;
}

///////////////////////////////////////////////////////////////
// send breaks to calibrate target's oscillator

int MasterChip::calibrate_speed(void)
{
    UINT8 b =0;
    int ret, cnt=0;

    for(;;)
    {
        // wait for ACK or timeout
        ret = rb(&b, 1);
        // read error
        if(ret < 0)
        {
            if(cnt) printf("\n");
            fprintf(stderr, "Error receiving calibrate ACK.\n");
            return -1;
        }
        // got something
        else if(ret > 0)
        {
            // it is wrong to receive anything other than ACK
            if(b != ACK)
            {
                if(cnt) printf("\n");
                fprintf(stderr, "Wrong calibrate response (0x%02x, must be 0x%02x). Target is not calibrated.\n", b, ACK);
                continue;//return -1;
            }
            // OK, exit calibration loop
            break;
        }

        b = 0;
        ret = wb(&b, 1);
        //fprintf(stderr, "Send Standard 0.\n");

        if(ret < 0)
        {
            if(cnt) printf("\n");
            fprintf(stderr, "Can't send trim pulse.\n");
            return -1;
        }

        // another break sent
        printf("\rCalibration break pulse sent. Count: %d ", ++cnt);
        fflush(stdout);

        if(cnt > SPEED_CALIB_MAX_ATTEMPS)
        {
            fprintf(stderr, "\nThe maximum attemps reached to try calibrate clocks. Try again with diffrent baudrate settings.\n");
            return -1;
        }

    };

    if(cnt) printf("\n");
    // success (got ACK)
    return 1;
}

int MasterChip::MasterDownloadProcess()
{

    if(hook_reset() < 0)
    {
         qDebug()<<"hook_reset fail";
         return 0;
    }

    //calibrate target's oscilator
    if(calibrate_speed() < 0)
    {
        qDebug()<<"calibrate_speed fail";
        return 0;
    }

    // read info
    if(!read_mcu_info())
    {
        printf("\nBootloader protocol version: 0x%02X (%s, ", bl_version , Version_string(bl_version));
        printf("read command ");
        if (!bl_rcs) printf("NOT ");
        printf("supported)\n");
        printf("CRC protocol ");
        if (!bl_crcs) printf("NOT ");
        printf("supported)\n");
        printf("Bootloader version string: %s\n", ident.targ_name);

        if (bl_version == BL_HC08) // HC08
        {
            unsigned i;
            printf("Available flash memory: 0x%04X-0x%04X\n", ident.mem_start[0], ident.mem_end[0]);
            printf("Erase block size: %3d bytes\n", ident.erblk[0]);
            printf("Write block size: %3d bytes\n", ident.wrblk[0]);
            printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);
            printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);
            printf("Bootloader data (hex): ");
            for(i=0; i<sizeof(ident.priv_data); i++)
                printf("%02x ", ident.priv_data[i]);
            printf("\n");
        }
        else if (bl_version == BL_HCS08) // S08
        {
            unsigned i;
            printf("System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, getMcuNameStr(&ident, ' ', 0) ,ident.sdidrev);
            printf("Number of memory blocks: %u\n", ident.num_blocks);
            for(i=0; i<ident.num_blocks; i++)
                printf("Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
            printf("Erase block size: %3d bytes\n", ident.erblk[0]);
            printf("Write block size: %3d bytes\n", ident.wrblk[0]);
            printf("Original vector table: 0x%04X-0xFFFF\n",ident.int_vect_tbl);
            printf("New vector table:      0x%04X-0x%04X\n",ident.bl_tbl, ident.bl_tbl+(0xFFFF-ident.int_vect_tbl));
        }
        else if (bl_version == BL_HC08_LARGE)	// large HC08
        {
            unsigned i;
            printf("Number of memory blocks: %u\n", ident.num_blocks);
            for(i=0; i<ident.num_blocks; i++)
                printf("Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
            printf("Erase block size: %3d bytes\n", ident.erblk[0]);
            printf("Write block size: %3d bytes\n", ident.wrblk[0]);
            printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);
            printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);
        }
        else if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))// large + long S08
        {
            unsigned i;
            printf("System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, getMcuNameStr(&ident, ' ', 0) ,ident.sdidrev);
            printf("Number of memory blocks: %u\n", ident.num_blocks);
            for(i=0; i<ident.num_blocks; i++)
                printf("Memory block #%u: 0x%06X-0x%06X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
            printf("Erase block size: %3d bytes\n", ident.erblk[0]);
            printf("Write block size: %3d bytes\n", ident.wrblk[0]);
            printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);
            printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);
        }
        else if (bl_version == BL_CFV1) // CFV1
        {
            unsigned i;
            printf("System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, getMcuNameStr(&ident, ' ', 0) ,ident.sdidrev);
            printf("Number of memory blocks: %u\n", ident.num_blocks);
            for(i=0; i<ident.num_blocks; i++)
                printf("Memory block #%u: 0x%08X-0x%08X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
            printf("Erase block size: %3d bytes\n", ident.erblk[0]);
            printf("Write block size: %3d bytes\n", ident.wrblk[0]);
            if (ident.int_vect_tbl != 0)
            {
                printf("Original vector table: 0x%08X-0x%08X\n",NULL, ident.int_vect_tbl-1);
                printf("New vector table: 0x%08X-0x%08X\n",ident.bl_tbl,ident.bl_tbl+ident.int_vect_tbl-1);
            } else
                printf("Vector table not relocated\n");
        }
        else if (bl_version == BL_KINETIS_OLD) // Kinetis old protocol
        {
            unsigned i;
            printf("%s\n", getMcuNameStr(&ident, '\n', 0));
            printf("Number of memory blocks: %u\n", ident.num_blocks);
            for(i=0; i<ident.num_blocks; i++)
            {
                printf("Memory block #%u: 0x%08X-0x%08X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
                printf("Erase block size: %3d bytes\n", ident.erblk[i]);
                printf("Write block size: %3d bytes\n", ident.wrblk[i]);
            }
            printf("Original vector table: 0x%08X-0x%08X\n",ident.int_vect_tbl,ident.int_vect_tbl + 0x3FF);
            printf("New vector table: 0x%08X-0x%08X\n",ident.bl_tbl, ident.bl_tbl + 0x3FF);

        }else if (bl_version == BL_KINETIS) // Kinetis current protocol
        {
            unsigned i;
            printf("%s\n", getMcuNameStr(&ident, '\n', 0));
            printf("Number of memory blocks: %u\n", ident.num_blocks);

            for(i=0; i<ident.num_blocks; i++)
                printf("Memory block #%u: 0x%08X-0x%08X\nErase block size: %3d bytes\nWrite block size: %3d bytes\n\n", i+1, ident.mem_start[i], ident.mem_end[i],ident.erblk[i], ident.wrblk[i]);

            printf("Original vector table: 0x%08X-0x%08X\n",ident.int_vect_tbl,ident.int_vect_tbl + 0x3FF);
            printf("New vector table: 0x%08X-0x%08X\n",ident.bl_tbl, ident.bl_tbl + 0x3FF);

        }
        else
        {
            fprintf(stderr, "This version [0x%02X] of protocol is not (yet) supported! Check AN2295SW for update!\n", bl_version);
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "Can't read MCU info. Could be protocol error.\n");
        fprintf(stderr, "(Or forgot to set single wire mode?)\n");
        return 0;
    }

    reset_S19image(&image);

    if(read_s19(&image, &ident,pfile_buf,file_size) < 0)
	{
        return 0;
    }
	
	// masquerade vector table
	if(setup_vect_tbl(ident.int_vect_tbl , ident.bl_tbl) < 0)
	{
		return 0;
	}
	
	// check if code goes to valid memory only - issue warning only, but continue! (r30323)
    check_image(&image);

    // program it!
    if(prg_mem() < 0)
    {
        fprintf(stderr, "Program memory failed.\n");
    }

    unhook();
    return 1;
}
