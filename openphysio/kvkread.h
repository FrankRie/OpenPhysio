/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
**               2026 Dr. Frankl Riechert 
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#ifndef KVKREAD_H_
#define KVKREAD_H_

#include <QtCore>
#include "config.h"
#include "mydb.h"

// FIXME: Actually we could MCARD API with Windows, too. But it's tricky with mingw ...
#ifndef Q_OS_WIN  // we only use MCARD with Linux and Mac (for Windows we have CT API)
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
#endif

#define STAT_OPEN   1
#define STAT_CLOSED 2

#define MAX_DATA_LEN        256

#define MIN_RECORD_LEN       72
#define MAX_RECORD_LEN      214

#define TEMPLATE_BOUNDARY   127
#define OFS_TEMPLATE_V1       2
#define OFS_TEMPLATE_V2       3

#define TAG_Template        0x60
#define TAG_InsuranceName   0x80
#define TAG_IK              0x81
#define TAG_InsurantID      0x82
#define TAG_InsurantStatus  0x83
#define TAG_Title           0x84
#define TAG_Forename        0x85
#define TAG_AddName         0x86
#define TAG_Surname         0x87
#define TAG_Birthday        0x88
#define TAG_Street          0x89
#define TAG_Country         0x8A
#define TAG_Zip             0x8B
#define TAG_City            0x8C
#define TAG_ValidUntil      0x8D
#define TAG_Checksum        0x8E
#define TAG_CardNumber      0x8F
#define TAG_AddStatus       0x90

#define	CTBCS_CLA 			0x20
#define	CTBCS_INS_RESET			0x11
#define	CTBCS_INS_REQUEST		0x12
#define CTBCS_P1_INTERFACE1		0x01
#define CTBCS_P2_RESET_GET_ATR		0x01


class KvkRead
{
private:
    int stat, r_id;
    cReaderDevice readerDevice;

#ifndef Q_OS_WIN
    // MCARD stuff
    SCARDCONTEXT ScardContext;
    SCARDHANDLE CardHandle;
    SCARDCONTEXT hMCardContext;
    SCARDHANDLE hMCard;
    DWORD ActiveProtocol;
    DWORD dwDLLVersion;
    char szReaderName [256];
    BYTE byCardType;

    typedef LONG (*pfnMCardInitialize) (SCARDCONTEXT hSCardContext, char* szReaderName, PSCARDCONTEXT phMCardContext, PDWORD pdwDllVersion);
    typedef LONG (*pfnMCardShutdown) (SCARDCONTEXT hMCardContext);
    typedef LONG (*pfnMCardConnect) (SCARDCONTEXT hMCardContext, DWORD dwConnectMode, BYTE byCardType, PSCARDHANDLE phMCard);
    typedef LONG (*pfnMCardDisconnect) (SCARDHANDLE hMCard, DWORD dwDisposition);
    typedef LONG (*pfnMCardGetAttrib) (SCARDHANDLE hMCard, DWORD dwAttrId, LPBYTE pbAttr, LPDWORD pcbAttrLen);
    typedef LONG (*pfnMCardReadMemory) (SCARDHANDLE hMCard, BYTE bMemZone, DWORD dwOffset, LPBYTE pbReadBuffer, LPDWORD pbReadLen);

    pfnMCardInitialize pMCardInitialize;
    pfnMCardShutdown pMCardShutdown;
    pfnMCardConnect pMCardConnect;
    pfnMCardDisconnect pMCardDisconnect;
    pfnMCardGetAttrib pMCardGetAttrib;
    pfnMCardReadMemory pMCardReadMemory;
#endif

    // CT-API stuff
#ifdef Q_OS_WIN
    typedef char(__stdcall *pCT_init)(unsigned short term, unsigned short port);
    typedef char(__stdcall *pCT_data)(unsigned short term, unsigned char *dad,
                                                        unsigned char *sad, unsigned short cmd_len,
                                                        unsigned char *cmd, unsigned short *rsp_len,
                                                        unsigned char *rsp);
    typedef char(__stdcall *pCT_close)(unsigned short term);
#else
    typedef char(*pCT_init)(unsigned short term, unsigned short port);
    typedef char(*pCT_data)(unsigned short term, unsigned char *dad,
                                                        unsigned char *sad, unsigned short cmd_len,
                                                        unsigned char *cmd, unsigned short *rsp_len,
                                                        unsigned char *rsp);
    typedef char(*pCT_close)(unsigned short term);
#endif
    pCT_init func_CT_init;
    pCT_data func_CT_data;
    pCT_close func_CT_close;

    void translate(unsigned char *d, int len);
    bool getAvailableReaders();
	
public:
    KvkRead();
    virtual ~KvkRead();
    void reader_set(cReaderDevice& dev);
    int reader_init();
    int reader_cleanup();
    int reader_data_ctapi(unsigned char *data, int *l);
    int reader_data_mcard(unsigned char *data, int *l);
    int read_kvk(cPatient *p);

#ifdef OPA
    QString test_reader(cReaderDevice& dev);
#endif
};

#endif /*KVKREAD_H_*/
