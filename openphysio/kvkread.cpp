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

#include "kvkread.h"
#include <iostream>
#include <QDebug>

//#define MAXBYTES 256
#define MAXBYTES 300
#define CTACS_READ_BINARY 0xB0

#define SAD_HOST    2
#define SAD_REMOTE  5
#define DAD_SLOT    0
#define DAD_TERM    1 

#define MCARD_READ_OFFSET 30

KvkRead::KvkRead()
{
    stat = STAT_CLOSED;
    readerDevice = myConf.getReader();

    func_CT_init = NULL;
    func_CT_data = NULL;
    func_CT_close = NULL;

#ifndef Q_OS_WIN
    pMCardInitialize = NULL;
    pMCardShutdown = NULL;
    pMCardConnect = NULL;
    pMCardDisconnect = NULL;
    pMCardGetAttrib = NULL;
    pMCardReadMemory = NULL;
#endif

}


KvkRead::~KvkRead()
{
}


void KvkRead::reader_set(cReaderDevice& dev)
{
    readerDevice = dev;
}


int KvkRead::reader_init()
{
    if (readerDevice.getAPI() == cReaderDevice::CTAPI) {
        func_CT_init = (pCT_init)QLibrary::resolve(readerDevice.getLib(), "CT_init");
        func_CT_data = (pCT_data)QLibrary::resolve(readerDevice.getLib(), "CT_data");
        func_CT_close = (pCT_close)QLibrary::resolve(readerDevice.getLib(), "CT_close");

        if (func_CT_init == 0 || func_CT_data == 0 || func_CT_close == 0) return  -1;

        char ret = func_CT_init(readerDevice.getTerminal(), readerDevice.getPort());
        if (ret != 0) return -2;

        stat = STAT_OPEN;
        return 0;
    } else if (readerDevice.getAPI() == cReaderDevice::MCARD) {
#ifndef Q_OS_WIN
        pMCardInitialize = (pfnMCardInitialize)QLibrary::resolve(readerDevice.getLib(), "MCardInitialize");
        pMCardShutdown = (pfnMCardShutdown)QLibrary::resolve(readerDevice.getLib(), "MCardShutdown");
        pMCardConnect = (pfnMCardConnect)QLibrary::resolve(readerDevice.getLib(), "MCardConnect");
        pMCardDisconnect = (pfnMCardDisconnect)QLibrary::resolve(readerDevice.getLib(), "MCardDisconnect");
        pMCardGetAttrib = (pfnMCardGetAttrib)QLibrary::resolve(readerDevice.getLib(), "MCardGetAttrib");
        pMCardReadMemory = (pfnMCardReadMemory)QLibrary::resolve(readerDevice.getLib(), "MCardReadMemory");
        if (pMCardInitialize == NULL || pMCardShutdown == NULL || pMCardConnect == NULL ||
            pMCardDisconnect == NULL || pMCardGetAttrib == NULL || pMCardReadMemory == NULL) {
            qDebug() << __FUNCTION__ << ": Failed to load MCARD library functions from" << readerDevice.getLib();
            return -1;
        }

        CardHandle = 0;
        ActiveProtocol = 0;
        byCardType = 0x00;

        DWORD dwDLLVersion;
        LONG lReturn;

        lReturn = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &ScardContext);
        if (lReturn != SCARD_S_SUCCESS) {
            qDebug() << "Function SCardEstablishContext returned error code" << lReturn;
            return -2;
        }

        DWORD dwReaders = 0;
        LPSTR mszReaders = NULL;

#ifdef linux // Q_OS_MACX
// we have some link issues on Mac (SCardFreeMemory isn't a defined symbol for whatever reason)
#undef SCARD_AUTOALLOCATE
#endif
#ifdef SCARD_AUTOALLOCATE
        dwReaders = SCARD_AUTOALLOCATE;
        SCardListReaders(ScardContext, NULL, (LPSTR)&mszReaders, &dwReaders);
#else
        SCardListReaders(ScardContext, NULL, NULL, &dwReaders);
        mszReaders = (char*)calloc(dwReaders, sizeof(char));
        SCardListReaders(ScardContext, NULL, mszReaders, &dwReaders);
#endif
        char *ptr;
        int nbReaders = 0;
        ptr = mszReaders;
        while (*ptr != '\0') {
            ptr += strlen(ptr) + 1;
            nbReaders++;
        }

        if (nbReaders == 0) {
            qDebug() << "SCardListReaders: No reader found";
            return -2;
        }

        // simply use the first reader found
        lReturn = pMCardInitialize(ScardContext, mszReaders, &hMCardContext, &dwDLLVersion);
        if (lReturn != SCARD_S_SUCCESS) {
            qDebug() << "MCardInitialize: failed";
#ifdef SCARD_AUTOALLOCATE
            if (mszReaders) SCardFreeMemory(ScardContext, mszReaders);
#else
            free(mszReaders);
#endif
            return -2;
        }
        qDebug() << "MCardInitialize: done";

        lReturn = pMCardConnect(hMCardContext, 1 /*INTELLIGENT_MODE*/, byCardType, &hMCard);
        if (lReturn != SCARD_S_SUCCESS) {
            qDebug() << "MCardConnect: failed";
            switch(lReturn) {
                case SCARD_E_UNKNOWN_CARD: qDebug() << "Unbekannte Karte"; break;
 //               case SCARD_E_NOT_INITIALIZED: qDebug() << "MCardInitialize failed before"; break;
                case SCARD_W_REMOVED_CARD: qDebug() << "Karte wurde entfernt"; break;
                default: qDebug() << "Unbekannter Fehler";
            }
#ifdef SCARD_AUTOALLOCATE
            if (mszReaders) SCardFreeMemory(ScardContext, mszReaders);
#else
            free(mszReaders);
#endif
            return -2;
        }
        qDebug() << "MCardConnect: done";

        stat = STAT_OPEN;
#ifdef SCARD_AUTOALLOCATE
            if (mszReaders) SCardFreeMemory(ScardContext, mszReaders);
#else
            free(mszReaders);
#endif

#endif
        return 0;
    }

    return -1;
}


int KvkRead::reader_cleanup()
{
    QString qs;

    if (readerDevice.getAPI() == cReaderDevice::CTAPI) {
        if (stat == STAT_OPEN) {
            char ret = 0;
            if (func_CT_close != NULL) ret = func_CT_close(readerDevice.getTerminal());
            if (ret != 0)  return -1;
        }
    } else if (readerDevice.getAPI() == cReaderDevice::MCARD) {
#ifndef Q_OS_WIN
        LONG lReturn = SCARD_S_SUCCESS;
        if (pMCardDisconnect != NULL) pMCardDisconnect(hMCard, 0);
        if (pMCardShutdown != NULL) lReturn = pMCardShutdown(hMCardContext);
        if (lReturn != SCARD_S_SUCCESS) qDebug() << "MCardShutdown: failed";
        SCardReleaseContext(ScardContext);
#endif
    }
    return 0;
}


int KvkRead::reader_data_ctapi(unsigned char *data, int *l)
{
    unsigned char dad, sad;
    unsigned short cmd_len;
    unsigned short rsp_len;
    unsigned char cmd[MAXBYTES];
    unsigned char rsp[MAXBYTES];
    unsigned short term = readerDevice.getTerminal();
    char ret;
	
    if (func_CT_data == NULL) return -1;

    if (stat == STAT_OPEN) {
	sad = SAD_HOST;
	dad = DAD_TERM;
	rsp_len = MAXBYTES;
	cmd[0] = CTBCS_CLA;
	cmd[1] = CTBCS_INS_RESET;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd_len = 5;
	ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
	if (ret != 0) {
            qDebug() << "CT_data failed";
	    return -1;
	}
        qDebug() << "RESET CT done";
	//for (int x = 0; x < rsp_len; x++) printf("%02x ", rsp[x]);
	//printf("\n");

	//
	// Now the chipcard must be inserted
	//
		
	sad = SAD_HOST;
	dad = DAD_TERM;
	rsp_len = MAXBYTES;
	cmd[0] = CTBCS_CLA;
	cmd[1] = CTBCS_INS_REQUEST;
	cmd[2] = 0x01;
	cmd[3] = 0x01;
	cmd[4] = 0x00;
	cmd_len = 5;
	ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
	if (ret != 0) {
            qDebug() << "CT_data failed";
	    return -1;
	}
        qDebug() << "RESET/REQUEST ICC done";
	//for (int x = 0; x < rsp_len; x++) printf("%02x ", rsp[x]);
	//printf("\n");

/*
		sad = SAD_HOST;
		dad = DAD_TERM; 
		cmd[0] = CTBCS_CLA;
		cmd[1] = CTBCS_INS_REQUEST;
		cmd[2] = CTBCS_P1_INTERFACE1;
		cmd[3] = CTBCS_P2_REQUEST_GET_ATR;
		cmd[4] = 0x00;
		cmd_len = 5;
		ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
		if (ret != 0) {
			qDebug() << "CT_data failed";
		    return -1;
		}
		qDebug() << "Get ATR done";
		printf("\n");
		for (int x = 0; x < rsp_len; x++) printf("%02x ", rsp[x]);
		printf("\n");
*/

   if (readerDevice.getSelect()) {
	    sad = SAD_HOST;
	    dad = DAD_SLOT;
	    cmd[0] = 0x00;
	    cmd[1] = 0xa4; // select file
 /*       cmd[2] = 0x00;
        cmd[3] = 0x00;
        cmd[4] = 0x02;
        cmd[5] = 'P';
        cmd[6] = 'D';
     cmd[7] = 'A';
       cmd[7] = 0x00;
        cmd_len =8; */

           /* cmd[2] = 0x04;
            cmd[3] = 0x00;
            cmd[4] = 0x08;
            cmd[5] = 0xD2;
            cmd[6] = 0x76;
            cmd[7] = 0x00;
            cmd[8] = 0x00;
            cmd[9] = 0x05;
            cmd[10] = 0x10;
            cmd[11] = 0x00;
            cmd[12] = 0x06;
            cmd_len = 13; */

	    //cmd[2] = 0x04;
			
	    // for cyberJack
	    // (instead I would like use 0x04 which prevent using a read offset when calling READ BINARY
        // [at least for Cherry and Omnikey]) /*
       cmd[2] = 0x00;
	    cmd[3] = 0x00;
	    cmd[4] = 0x02;
	    cmd[5] = 0x3f; // complete memory
	    cmd[6] = 0x00;
        cmd_len = 7;

	    rsp_len = MAXBYTES;
	    ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
	    if (ret != 0) {
                qDebug() << "CT_data (SELECT FILE) failed";
		return -1;
    }
        qDebug() << "SELECT FILE done";
            //for (int x = 0; x < rsp_len; x++) printf("%02x ", rsp[x]);
            //printf("\n");
   }

	sad = SAD_HOST;
	dad = DAD_SLOT;
	rsp_len = MAXBYTES;
	cmd[0] = 0x00;
	cmd[1] = CTACS_READ_BINARY;
	cmd[2] = 0x00;
        qDebug() << "offset:" << readerDevice.getReadOffset();
    cmd[3] = readerDevice.getReadOffset();
	cmd[4] = 0x00;
	cmd_len = 5;
	ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
	if (ret != 0) {
            qDebug() << "CT_data (READ BINARY) failed";
	    return -1;
	}
        qDebug() << "READ BINARY done";
        qDebug() << rsp_len;
        for (int x = 0; x < rsp_len; x++) printf("%02x ", rsp[x]);
        printf("\n");
		
	*l = rsp_len;
	if (rsp_len > 0) memcpy(data, rsp, rsp_len);

	sad = SAD_HOST;
	dad = DAD_TERM;
	rsp_len = MAXBYTES;
	cmd[0] = CTBCS_CLA;
	cmd[1] = 0x14; // DEACTIVATE ICC
	cmd[2] = 0x01;
	cmd[3] = 0x00;
	cmd_len = 4;
	ret = func_CT_data(term, &dad, &sad, cmd_len, cmd, &rsp_len, rsp);
	if (ret != 0) {
            qDebug() << "CT_data failed";
	    return -1;
	}
        qDebug() << "DEACTIVATE ICC done";
    }
    return 0;
}


int KvkRead::reader_data_mcard(unsigned char *data, int *l)
{
#ifndef Q_OS_WIN
    if (pMCardGetAttrib == NULL || pMCardReadMemory == NULL) return -1;

    LONG lReturn;
    DWORD dwLen = 0;

    // read size of memory zone
    /*SCARD_MEMORY*/ QString memzone;
    dwLen = 0;
    //int ofs = readerDevice.getReadOffset();
    int ofs = MCARD_READ_OFFSET;

    lReturn = pMCardGetAttrib(hMCard,  1 /*SCARD_ATTR_MEMORY*/, (unsigned char*)&memzone, &dwLen);
    if (lReturn == SCARD_S_SUCCESS) {
        qDebug() << "memzone size:" << memzone.size();     // dwSize;
        dwLen = memzone.size() /* dwSize */ - ofs - 1;
        if (dwLen <= 0) return -1;
        lReturn = pMCardReadMemory(hMCard, 0, ofs, data, &dwLen);
        if (lReturn != SCARD_S_SUCCESS) {
            qDebug() << "MCardReadMemory: failed";
            return -1;
        }
        *l = dwLen;
        return 0;
    } else {
        qDebug() << "MCardGetAttrib: failed";
        return -1;
    }
#endif
    return -1;
}


//#ifndef OPA
void KvkRead::translate(unsigned char *d, int len)
{
    for (int i = 0; i < len; i++) {
	switch(d[i]) {
	    case 0x5B:
                d[i] = 196; //'Ä';
		break;
	    case 0x5C:
                d[i] = 214; //'Ö';
		break;
	    case 0x5D:
                d[i] = 220; //'Ü';
		break;
	    case 0x7B:
                d[i] = 228; //'ä';
		break;
	    case 0x7C:
                d[i] = 246; //'ö';
		break;
	    case 0x7D:
                d[i] = 252; //'ü';
		break;
            case 0x7E:
                d[i] = 223; //'ß';
                break;
            default:
		break;
	}
    }
}


int KvkRead::read_kvk(cPatient *p)
{
    unsigned char d[MAX_DATA_LEN + 1];
    int len;
    int done = 0;
    QString qs;
    QDate date_valid;
    int i = 0;

#ifndef OPA
    if ( reader_init() != 0) {
	reader_cleanup();
	return -1;
    }
#endif
    len = 0;
    if (readerDevice.getAPI() == cReaderDevice::CTAPI) {
        reader_data_ctapi((unsigned char*)&d, &len);
    } else if (readerDevice.getAPI() == cReaderDevice::MCARD) {
        reader_data_mcard((unsigned char*)&d, &len);
    }
#ifndef OPA
    reader_cleanup();
#endif
    //qDebug() << "rsp_len: " << QString::number(len);

    //printf("\n");
    //for (int x = 0; x < len; x++) printf("%02x ", d[x]);
    //printf("\n");
	
    // sanity checks
    if (len < MIN_RECORD_LEN) return -2; // according to spec: min=70 max=212 (for KVK template tag len) => add 2
    if (d[0] != TAG_Template) return -2; // does data really start with KVK template tag?

    // according to spec: d[1] = LVDT
    if (d[1] <= TEMPLATE_BOUNDARY) i = OFS_TEMPLATE_V1;
    else i = OFS_TEMPLATE_V2;
	
    while (i < len)
    {
	switch(d[i]) {
	    case TAG_InsuranceName: // insurance name
		//qDebug() << "insurance name";
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_IK: // IK
		//qDebug() << "IK";
		qs = QString::fromLatin1((const char *)&d[i + 2], d[i + 1]);
		if (qs.length() == 7) qs = "10" + qs;
		p->setInsurance(qs);
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_InsurantID: // ID of insurant
		//qDebug() << "ID of insurant";
		p->setVNR(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_InsurantStatus: // status of insurant
		//qDebug() << "status of insurant";
		p->setStatus(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		//qDebug() << QString::fromLatin1((const char *)&d[i + 2], d[i + 1]);
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Title: // title
		//qDebug() << "title";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setTitle(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Forename: // forename
		//qDebug() << "forename";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setForename(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_AddName: // add name
		//qDebug() << "add name";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setAddName(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Surname: // surname
		//qDebug() << "surname";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setName(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Birthday: // birthday
		//qDebug() << "birthday";
		qs = QString::fromLatin1((const char *)&d[i + 2], d[i + 1]);
		p->setBirthday(QDate::fromString(qs, "ddMMyyyy"));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Street: // street
		//qDebug() << "street";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setStreet(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Country: // country code
		//qDebug() << "country";
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Zip: // ZIP
		//qDebug() << "ZIP";
		p->setZipcode(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_City: // city
		//qDebug() << "city";
		translate((unsigned char *)&d[i + 2], d[i + 1]);
		p->setCity(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
	    case 0x8D: // kvk valid until
                //qDebug() << "kvk valid until";
		qs = QString::fromLatin1((const char *)&d[i + 2], d[i + 1]);
		qs = "01" + qs.insert(2,"20");
                date_valid = QDate::fromString(qs, "ddMMyyyy");
                date_valid = date_valid.addDays(date_valid.daysInMonth() - 1);
                p->setValidKVK(date_valid);
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_CardNumber:
		//qDebug() << "VKNR";
		p->setKVK(QString::fromLatin1((const char *)&d[i + 2], d[i + 1]));
		i = i + (int)d[i + 1] + 1;
		break;
            case TAG_AddStatus: // Statusergänzung
		//qDebug() << "Statusergaenzung";
		i = i + (int)d[i + 1] + 1;
		break;
	    case TAG_Checksum: // checksum
		//qDebug() << "checksum";
		// TODO: we should calculate the checksum
		done = 1;
		break;
	    default:
		//qDebug() << "STRANGE !!!";
		break;
	}
	i++;
	if (done) break;
    }

    if (done) return 0;
    return -3;
}
//#endif // OPA


#ifdef OPA
QString KvkRead::test_reader(cReaderDevice& dev)
{
    int res;
    QStringList msgs;

    reader_set(dev);
    res = reader_init();
    switch (res) {
	case 0:
            msgs << QString::fromUtf8("Bibliotheken erfolgreich geladen");
            msgs << QString::fromUtf8("Lesegerät erfolgreich initialisiert");
	    break;
	case -1:
            msgs << QString::fromUtf8("Bibliothek konnte nicht geladen werden");
	    break;
	case -2:
            msgs << QString::fromUtf8("Lesegerät konnte nicht initialisiert werden");
	    break;
	default: break;
    }
    if (res == 0) {
        cPatient p;
        if (read_kvk(&p) < 0) {
            msgs << QString::fromUtf8("Kartendaten konnten nicht gelesen werden");
        } else {
            msgs << QString::fromUtf8("Kartendaten wurden gelesen");
            msgs << QString::fromUtf8("  Patientenname: ") + p.getForename() + " " + p.getName();
            msgs << QString::fromUtf8("  Geburtsdatum: ") + p.getBirthday().toString("dd.MM.yyyy");
        }
	res = reader_cleanup();
	switch (res) {
	    case 0:
                msgs << QString::fromUtf8("Lesegerät wurde wieder erfolgreich geschlossen");
		break;
	    case -1:
                msgs << QString::fromUtf8("Kommunikation mit Lesegerät konnte nicht ordnungsgemäß beendet werden");
		break;
	    default: break;
	}
    } else reader_cleanup();

    return msgs.join(QString::fromUtf8("\n"));
}
#endif
