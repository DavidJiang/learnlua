#include "strconvert_linux.h"
#include <iconv.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include <stdio.h>
#include <errno.h>

void AnalyzeEncodingType(int p_iEncodingType, char *p_pEncoding)
{
	if( p_iEncodingType  == -1 )
	{
		char *pDefaultEncoding = getenv("LANG");
		char *pDot = strchr(pDefaultEncoding, '.');
		assert( pDot != NULL );
		strcpy(p_pEncoding, pDot + 1);
	}
	else if ( p_iEncodingType == t_strConverter::ec_utf_8 )
	{
		strcpy(p_pEncoding, "UTF-8");
	}
	else if ( p_iEncodingType == t_strConverter::ec_gbk )
	{
		strcpy(p_pEncoding, "GBK");
	}
	else if( p_iEncodingType == t_strConverter::ec_ucs2 )
	{
		strcpy(p_pEncoding, "UCS-2LE");
	}
	else if( p_iEncodingType == t_strConverter::ec_ucs4 )
	{
		strcpy(p_pEncoding, "UCS-4LE");
	}
	else
	{
		//todo:
	}
}

bool t_strConverter::W2U(const wchar_t *p_pSrc, char *p_pDst, int &p_iDstSize)
{
	iconv_t convd = iconv_open( "UCS-2LE", "UCS-4LE");

	if( convd == (iconv_t) -1)
	{
		assert(false);
		return false;
	}
	size_t inlen = wcslen(p_pSrc) * sizeof(wchar_t);
	char *psrc = reinterpret_cast<char*>( const_cast<wchar_t *>(p_pSrc) );
	size_t outlen = p_iDstSize;
	char *pdst = p_pDst;
	assert( psrc != NULL && pdst != NULL );
	size_t ret = iconv( convd, &psrc, &inlen, &pdst, &outlen );
	if( ret == (size_t)-1 )
	{
		perror( strerror(errno) );
		wprintf(p_pSrc);
		return false;
	}
	p_iDstSize = p_iDstSize - outlen;

	iconv_close(convd);
	return true;
}

bool t_strConverter::U2W(const char *p_pSrc, int p_iSrcSize, wchar_t *p_pDest, int &p_iDstSize)
{
	iconv_t convd = iconv_open("UCS-4LE", "UCS-2LE");

	if( convd == (iconv_t) -1)
	{
		assert(false);
		return false;
	}

	size_t inlen = p_iSrcSize;
	const char *psrc = p_pSrc;
	size_t outlen = p_iDstSize;
	char *pdst = reinterpret_cast<char*>( p_pDest );
	assert( psrc != NULL && pdst != NULL );
	size_t ret = iconv( convd, (char **)&psrc, &inlen, &pdst, &outlen );
	if( ret == (size_t)-1 )
	{
		perror( strerror(errno) );
		return false;
	}
	p_iDstSize = p_iDstSize - outlen;

	iconv_close(convd);
	return true;
}

bool t_strConverter::U2C(const char *p_pSrc, char *p_pDest, int &p_iDstSize, int p_iEncodingType)
{
	if( !p_pSrc || !p_pDest )
	{
		return false;
	}

	// 度量长度
	const short *pSrc = (const short *)p_pSrc;
	while ( *pSrc != 0 )
	{
		++pSrc;
	}

	int nSrcLen = (pSrc - (const short *)p_pSrc) * sizeof(short);

	return A2B(p_pSrc, nSrcLen, p_pDest, p_iDstSize, ec_ucs2, p_iEncodingType);
}

bool t_strConverter::W2C(const wchar_t *p_pSrc, char *p_pDest,
							int &p_iDstSize, int p_iEncodingType)
{
	/**< max encoding type string length is 24. */
	char szDestEncoding[30] = {0};
	
	AnalyzeEncodingType(p_iEncodingType, szDestEncoding);

	iconv_t convd = iconv_open(szDestEncoding, "UCS-4LE");

	if( convd == (iconv_t) -1)
	{
		assert(false);
		return false;
	}
	size_t inlen = wcslen(p_pSrc) * sizeof(wchar_t);
	char *psrc = reinterpret_cast<char*>( const_cast<wchar_t*>(p_pSrc) );
	size_t outlen = p_iDstSize;
	char *pdst = p_pDest;
	assert( psrc != NULL && pdst != NULL );
	size_t ret = iconv( convd, &psrc, &inlen, &pdst, &outlen );
	if( ret == (size_t)-1 )
	{
		perror( strerror(errno) );
		wprintf(p_pSrc);
		return false;
	}
	p_iDstSize = p_iDstSize - outlen;
	iconv_close(convd);
	return true;
}

bool t_strConverter::C2W(const char *p_pSrc, int p_iSrcLen,
						 wchar_t *p_pDst, int& p_iDstLen, int p_iEncodingType)
{
	/**< max encoding type string length is 24. */
	char szFromEncoding[30] = {0};
	
	AnalyzeEncodingType(p_iEncodingType, szFromEncoding);

	iconv_t convd = iconv_open("UCS-4LE", szFromEncoding);

	if( convd == (iconv_t) -1)
	{
		assert(false);
		return false;
	}
	size_t inlen = p_iSrcLen;
	char *psrc = const_cast<char*>(p_pSrc);
	size_t outlen = p_iDstLen;
	char *pdst = reinterpret_cast<char*>(p_pDst);
	assert( psrc != NULL && pdst != NULL );
	size_t ret = iconv( convd, &psrc, &inlen, &pdst, &outlen );
	if( ret == (size_t)-1 )
	{
		perror( strerror(errno) );
		return false;
	}
	p_iDstLen = p_iDstLen - outlen;
	iconv_close(convd);
	return true;
}

bool t_strConverter::A2B(const char *p_pSrc, int p_iSrcLen,
						 char *p_pDst, int &p_iDstSize, 
						 int p_iSrcEncoding, int p_iDstEncoding)
{
	char szFromEncoding[30] = {0};
	char szToEncoding[30] = {0};
	
	AnalyzeEncodingType(p_iSrcEncoding, szFromEncoding);
	AnalyzeEncodingType(p_iDstEncoding, szToEncoding);
	iconv_t convd = iconv_open(szToEncoding, szFromEncoding);

	if( convd == (iconv_t) -1 )
	{
		assert(false);
		return false;
	}
	size_t inlen = p_iSrcLen;
	char *psrc = const_cast<char*>(p_pSrc);
	assert( psrc != NULL );
	size_t outlen = p_iDstSize;
	char *pdst = p_pDst;
	assert( psrc != NULL && pdst != NULL );
	size_t ret = iconv( convd, &psrc, &inlen, &pdst, &outlen );
	if( ret == (size_t)-1 )
	{
		perror( strerror(errno) );
		return false;
	}
	p_iDstSize = p_iDstSize - outlen;

	iconv_close(convd);
	return true;
}
