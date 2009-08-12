#include "blapi.h"
#include "zlib.h"

#include "localinc.h"
static int dec2int(const unsigned char* dec, unsigned char *num)
{
	int str_len = strlen((const char*)dec);
	int max_ints = (str_len + 5) /6;
	unsigned int *a = new unsigned int[max_ints+1];
	int hindex =1;
	memset(a, 0, sizeof(int)*max_ints);
	int carry = 0;
	int val = 0;
	int i, j;

	for(i=0; i<str_len; i++)
	{
		val = dec[i] - '0';
		carry = 0;
		for (j=0; j<hindex; j++)
		{
			a[j] *= 10;
			a[j] += carry + val;
			val = 0;
			carry = (a[j]>>24) & 0xFF;
			a[j] &= 0xFFFFFF;
		}
		if ( carry > 0)
		{
			a[hindex++] = carry;
		}
	}

	int offset = 0;
	hindex--;
	for(i=3; i>0 && ((a[hindex]>>(i*8))&0xFF) == 0; i--)
	{
		offset++;
	}
	if(offset == 4)
	{
		printf("error\n");
	}
	j=0;
	for(i=0; i<4-offset; i++)
	{
		num[j++] = (a[hindex]>>((3-offset-i)*8)) & 0xFF;
	}
	for(i=0; i<hindex; i++)
	{
		for(int k=0; k<3; k++)
		{
			num[j++] = (a[hindex-1-i] >> (2-k)*8) & 0xFF;
		}
	}
	delete[] a;
	return j;
}

static PRBool validateLicense(char* suser)
{
	char* smodulus   = "115801190261221214754334668902722425936509505416457970789287297728816388753627896293249501578830570324705253515546383166989625001335561947096747210280001245977114030627247212292377290543869343996595819188362915644707269064020812435233012510929338706599216007185654748959001143012936618501934698642942289379979";
    char* exponent = "65537";
	char encrypted[4096];
	unsigned char DLText[4096];

#ifdef _WIN32
	FILE *fp = fopen ("..\\license_key.txt", "r");
#else
	FILE *fp = fopen ("../license_key.txt", "r");
#endif
	if (!fp)
	{
		gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but no license key found -- ABORTING.", JAXER_BUILD_ID);
		gJaxerLog.Log(eINFO, "Cannot open license file.  Make sure your license file is named 'license_key.txt' and resides in the parent directory of jaxer.");
		return PR_FALSE;
	}
	PRUint32 nlen = fread(encrypted, 1, 4096, fp);
	fclose(fp);
	//gJaxerLog.Log(eINFO, "License length=%d", nlen);
	encrypted[nlen] = 0;

	// remove all non-digit chars
	int i, j;
	for(i=0, j=0; i<nlen; i++)
	{
		if (encrypted[i] >= '0' && encrypted[i] <= '9')
		{
			encrypted[j++] = encrypted[i];
		}
	}
	encrypted[j] = 0;

	// remove spaces
	//char *pEncrypted = encrypted;
	//while (nlen > 0 && isspace(pEncrypted[nlen-1]))
	//	pEncrypted[--nlen] = 0;
	//while (*pEncrypted && isspace(*pEncrypted))
	//	pEncrypted++;
	
    unsigned char xEncrypted[300], xModulus[300], xExponent[6];
    int nEnc = dec2int((unsigned char*) encrypted, xEncrypted);
    int nMod = dec2int((unsigned char*) smodulus, xModulus);
    int nExp = dec2int((unsigned char*) exponent, xExponent);
    
	if (nEnc > nMod)
	{
		gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		gJaxerLog.Log(eINFO, "Invalid license length.  Make sure your license_key.txt file contains the valid license text.");
		return PR_FALSE;
	}

    if (nMod > nEnc)
    {
        for (int i=nEnc-1; i>=0; i--)
            xEncrypted[i+nMod-nEnc] = xEncrypted[i];
        for (int i=0; i<nMod-nEnc; i++)
            xEncrypted[i] = 0;
        nEnc = nMod;
    }
	RSAPublicKey pk;
	
	SECItem *modulus = &pk.modulus;
	SECItem *publicExponent = &pk.publicExponent;

	pk.arena = PORT_NewArena(4096);

	SECItemType eIType = siUnsignedInteger; //siAsciiString;
	
	modulus->type = eIType;
	modulus->data = xModulus;
	modulus->len = nMod;
	
	publicExponent->type = eIType;
	publicExponent->data = xExponent;
	publicExponent->len = nExp;

	SECStatus rvx = RSA_PublicKeyOp(&pk, DLText, xEncrypted);
	PORT_FreeArena(pk.arena, PR_FALSE);

	if (rvx != SECSuccess)
	{
		gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		gJaxerLog.Log(eINFO, "Decrypting license failed.  Make sure your license_key.txt file contains the correct license text.");
		return PR_FALSE;
	}

	unsigned char *pp = DLText;
	i=0;
	//Remove any leading zero's
	while (i < nMod && *pp==0)
	{
		i++;
		pp++;
	}

	gJaxerLog.Log(eTRACE, "LT: %s", pp);
		
	// Check pp
	nlen = nMod - i;
	if (nlen < 14) //P;jaxer::<date><4-byte-hash>
	{
		gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		gJaxerLog.Log(eINFO, "Decrypting license returned invalid string length %d", nlen);
		gJaxerLog.Log(eTRACE, "%s", pp);
		return PR_FALSE;
	}
	
	nlen--;
	PRUint32 crc32Value = pp[nlen] + pp[nlen-1] * 256 + pp[nlen-2]*256*256 + pp[nlen-3] * 256 * 256 *256;
	PRUint32 crc;
	crc = crc32(0L, nsnull, 0);
	pp[nlen-3] = 0;
	
	crc = crc32(crc, pp, nlen-3);
	//gJaxerLog.Log(eINFO, "crc32value=%d crc=%d outout=%s", crc32Value, crc, pp);
	if (crc == crc32Value)
	{
		time_t tnow = TIME_NOW; //time(NULL);
	
		char *sType;
		char *sUser;
		char *sExpTime;
		char *p = (char*)pp;
		if (strncmp("P;jaxer:", p, 8) != 0)
		{
			gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		    gJaxerLog.Log(eINFO, "Invalid license string(1)");
			gJaxerLog.Log(eTRACE, "%s", pp);
			return PR_FALSE;
		}
		p += 8;
		if (*p == ':')
		{
			p++;
		}else
		{
			char *q = strstr(p, "::");
			if (!q)
			{
				gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		        gJaxerLog.Log(eINFO, "Invalid license string(2)");
				gJaxerLog.Log(eTRACE, "%s", pp);
				return PR_FALSE;
			}else
			{
				p = q + 2;
			}
		}
		
		sUser = p;
		p = strrchr(p, ';');
		if (!p)
		{
			gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		    gJaxerLog.Log(eINFO, "Invalid license string(3)");
			gJaxerLog.Log(eTRACE, "%s", pp);
			return PR_FALSE;
		}
		*p++ = 0;
		sExpTime = p;

		// Time in ms, chop off and leave only secs
		if (strlen(sExpTime) < 3)
		{
			gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		    gJaxerLog.Log(eINFO, "Invalid license string(4)");
			gJaxerLog.Log(eTRACE, "%s %s", pp, sExpTime);
			return PR_FALSE;
		}

		sExpTime[strlen(sExpTime)-3] = 0;
		time_t expireTime = (time_t)atol(sExpTime);
		    
		if (expireTime < tnow)
		{
			gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] %s (Expired) -- ABORTING.", JAXER_BUILD_ID, sUser);
			gJaxerLog.Log(eINFO, "License Expired on %s", ctime(&expireTime));
			gJaxerLog.Log(eINFO, "Jaxer Built on %s", ctime(&tnow));
			return PR_FALSE;
		}else
		{
			gJaxerLog.Log(eTRACE, "License Expires on %s.", ctime(&expireTime));
			gJaxerLog.Log(eTRACE, "Jaxer built on %s.", ctime(&tnow));
			strcpy(suser, sUser);
			return PR_TRUE;
		}
	}else
	{
		gJaxerLog.Log(eNOTICE, "[Jaxer/%s -- starting] Commercial Edition but with invalid license -- ABORTING.", JAXER_BUILD_ID);
		gJaxerLog.Log(eNOTICE, "Invalid license.  Make sure your license_key.txt file contains only the license text.");
		gJaxerLog.Log(eTRACE, "%s %d %d", pp, crc, crc32Value);
		return PR_FALSE;
	}
	return PR_FALSE;
}

