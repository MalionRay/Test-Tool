/**********************************************************************
* HEADER_START                                                        *
*                                                                     *
*      Name:           S19.C                                          *
*      Project:        Developper's HC08 Bootloader Master            *
*      Description:    S19 manipulation routines file                 *
*      Platform:       Linux/Win32                                    *
*      Revision:       1.02 (also read S2/S3)                         *
*      Date:           Apr 8 2002                                     *
*      Author:         Michal Hanak                                   *
*      Company:        Freescale                                      *
*      Security:       General Business                               *
*                                                                     *
* =================================================================== *
* Copyright (c):       FREESCALE Inc., 2004, All rights reserved.     *
*                                                                     *
* =================================================================== *
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY              *
* EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  *
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL FREESCALE OR             *
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT        *
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        *
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)            *
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, *
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED *
* OF THE POSSIBILITY OF SUCH DAMAGE.                                  *
* =================================================================== *
*
* HEADER_END
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hc08sprg.h"

////////////////////////////////////////////////////////////////
// module vars
static unsigned short s19_crc= 0;
static unsigned int total= 0;
static unsigned int m_maxlen = 0;
static unsigned int warningCnt= 0;

///////////////////////////////////////////////////////////////
// read S19 file and store data into global memory image

signed int S19_GetBlock(unsigned addr, IDENT_DATA * pIdent)
{
    unsigned int i = 0;

    for(i=0; i< pIdent->num_blocks; i++)
    {
        if(addr >= pIdent->mem_start[i] && addr <= pIdent->mem_end[i])
            return i;
    }

    if(is_vect_tbl_area(addr, pIdent))
        return MAX_NUM_BLOCKS;

    return -1;
}

void reset_S19image(BOARD_MEM * pImage)
{
    unsigned i;

    m_maxlen = 0;

    //initialize image
    for(i=0; i<= MAX_NUM_BLOCKS; i++) // Clear also the special compatibility block of non image original vector table
    {
        memset(&((*pImage)[i].d[0]), 0xff, MAX_ADDRESS);
        memset(&((*pImage)[i].f[0]), 0, MAX_ADDRESS);
    }
    warningCnt = 0;
}

int read_s19(BOARD_MEM* pImage, IDENT_DATA * pIdent,unsigned char *buf,int size)
{
    char *ptr;
    char c;
    char* pc;
    char afmt[7];
    unsigned int u, b, sum, len, alen;
    unsigned long addr, addr_lo = MAX_ADDRESS, addr_hi = 0;
    int line = 0, terminate = 0, block;
	int count = 0;

    total = 0;

    CRC_ResetCRC(&s19_crc, 0x0000);


    ptr = (char*)buf;
    while(!terminate && (pc = count< size ? ptr : NULL != NULL))
    {
        ptr=(char*)memchr(ptr,0x0D0A,strlen(ptr))+1;
        count+=(ptr-pc);
        line++;

        // S-records only
        if(*pc++ != 'S')
            continue;

        // record type
        switch(c = *pc++)
        {
        case '0':	// S0 is skipped
            continue;
        case '1':	// S1 is accepted
            alen = 4;
            break;
        case '2':	// S2 is accepted
            alen = 6;
            break;
        case '3':	// S3 is accepted
            alen = 8;
            break;
        case '9':	// S9 terminates
        case '8':	// S8 terminates
        case '7':	// S7 terminates
            terminate = 1;
            continue;
        default:	// others are reported and skipped
            fprintf(stderr, "Skipping S%c record at line %d", c, line);
            continue;
        }

        // prepare len & addr scan format
        sprintf(afmt, "%%2x%%%dx", alen);

        // scan len & address
        if(sscanf(pc, afmt, &len, &addr)!=2 || len >= 0x100)
        {
            fprintf(stderr, "S-record parse error at line %d\n", line);
            return -1;
        }

        block = S19_GetBlock(addr, pIdent);

        if(block < 0)
        {
            fprintf(stderr, "Address out of range at line %d\n", line);
            return -1;
        }


        printf("\rParsing S-record line %d ", line);
        // skip len & address
        pc += alen+2;

        // init checksum
        sum = len;
        for(u=0; u<4; u++)
            sum += (addr >> (u*8)) & 0xff;

        // len & addr processed in record
        len -= alen/2 + 1;

        // remember the longest record (see output)
        if(len > m_maxlen)
            m_maxlen = len;

        for(u=0; u<len; u++)
        {
            if(sscanf(pc, "%2x", &b) != 1 || b >= 0x100)
            {
                fprintf(stderr, "S-record data parse error at line %d\n", line);
                return -1;
            }
            pc += 2;

            if((*pImage)[block].f[addr+u - pIdent->mem_start[block]] == 1)
            {
                if(!warningCnt)
                {
                    fprintf(stderr, "\rS-record data parse warning at line %d, address 0x%lX. Rewriting exist data in image (bad image?).\r\n", line, addr+u);
                    fprintf(stderr, "\r\n");
                }
                else if(warningCnt < 2)
                {
                    fprintf(stderr, "\rS-record data parsing operation found more than one warning. Rewriting exist data in image (bad image?).\r\n");
                    fprintf(stderr, "\r\n");
                }

                warningCnt++;
            }
            (*pImage)[block].d[addr+u - pIdent->mem_start[block]] = b;
            (*pImage)[block].f[addr+u - pIdent->mem_start[block]] = 1;
            sum += b;
            total++;

            CRC_AddByte(&s19_crc, b);

            if (addr+u < addr_lo)
                addr_lo = addr+u;
            if (addr+u > addr_hi)
                addr_hi = addr+u;
        }

        printf("Bytes: %d", total);

        // test CS
        if(sscanf(pc, "%2x", &b) != 1 || b >= 0x100 || ((sum+b)&0xff) != 0xff)
        {
            fprintf(stderr, "S-record checksum error at line %d\n", line);
            return -1;
        }
    }

    if (total == 0)
    {
        fprintf(stderr, "S-record contains no valid data!\n");
        return -1;
    }
    else
    {
        printf("\rParsed S-record lines: %d  Bytes total: %d\n", line, total);
        if (addr_hi >= 0x1000000)
            printf("Source address range: 0x%08lX-0x%08lX\n\n", addr_lo, addr_hi);
        else if (addr_hi >= 0x10000)
            printf("Source address range: 0x%06lX-0x%06lX\n\n", addr_lo, addr_hi);
        else
            printf("Source address range: 0x%04lX-0x%04lX\n\n", addr_lo, addr_hi);
    }
    return 0;
}

