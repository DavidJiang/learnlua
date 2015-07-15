#include "strconvert_win.h"
#include <Windows.h>
#include <assert.h>
#include <wchar.h>

bool t_strConverter::W2U(const wchar_t *p_pSrc, char *p_pDst, int &p_iDstLen)
{
	int nLen = wcslen(p_pSrc);
	ConversionResult res = ConvertUTF16toUTF32((const unsigned short **)&p_pSrc, (const unsigned short *)(p_pSrc + nLen), (unsigned long **)&p_pDst, (unsigned long *)(p_pDst + p_iDstLen), strictConversion);
	if( res == conversionOK )
	{
		return true;
	}

	return false;
}

bool t_strConverter::U2W(const char *p_pSrc, int p_iSrcLen, wchar_t *p_pDst, int &p_iDstLen)
{
	ConversionResult res = ConvertUTF32toUTF16((const unsigned long **)&p_pSrc, (const unsigned long *)(p_pSrc + p_iSrcLen), (unsigned short **)&p_pDst, (unsigned short *)(p_pDst + p_iDstLen), strictConversion);
	if( res == conversionOK )
	{
		return true;
	}

	return false;
}

bool t_strConverter::W2C(const wchar_t *p_pSrc, char *p_pDst, int &p_iDstLen, int p_iEncodingType)
{
	switch( p_iEncodingType )
	{
	case -1:
	case ec_ucs2:
		{
			wcsncpy_s((wchar_t *)p_pDst, p_iDstLen / sizeof(wchar_t), p_pSrc, p_iDstLen / sizeof(wchar_t) - 1);
			p_iDstLen = wcslen((wchar_t *)p_pDst) * sizeof(wchar_t);
		}
		break;
	case ec_gbk:
		{
			p_iDstLen = ::WideCharToMultiByte(936, 0, p_pSrc, -1, p_pDst, p_iDstLen, 0, 0);
			if( p_iDstLen > 0 )
			{
#pragma message("此处需要再调查，该函数返回值是否包含末尾\0")
				--p_iDstLen;
			}
		}
		break;
	case ec_utf_8:
		{
			p_iDstLen = ::WideCharToMultiByte(CP_UTF8, 0, p_pSrc, -1, p_pDst, p_iDstLen, 0, 0);
			if( p_iDstLen > 0 )
			{
#pragma message("此处需要再调查，该函数返回值是否包含末尾\0")
				--p_iDstLen;
			}
		}
		break;
	case ec_ucs4:
		{
			W2U(p_pSrc, p_pDst, p_iDstLen);
		}
		break;
	default:
		{
			p_iDstLen = 0;
		}
		break;
	}

	return true;
}

bool t_strConverter::C2W(const char *p_pSrc, int p_iSrcLen, wchar_t *p_pDst, int &p_iDstLen, int p_iEncodingType)
{
	switch( p_iEncodingType )
	{
	case -1:
	case ec_ucs2:
		{
			int nCopyLen = (int)(p_iSrcLen / sizeof(wchar_t)) < (int)(p_iDstLen - 1) ? p_iSrcLen / sizeof(wchar_t) : p_iDstLen - 1;
			wcsncpy_s(p_pDst, p_iDstLen, (wchar_t *)p_pSrc, nCopyLen);
			p_iDstLen = wcslen(p_pDst) * sizeof(wchar_t);
		}
		break;
	case ec_gbk:
		{
			p_iDstLen = ::MultiByteToWideChar(936, 0, p_pSrc, p_iSrcLen, p_pDst, p_iDstLen);
		}
		break;
	case ec_utf_8:
		{
			p_iDstLen = ::MultiByteToWideChar(CP_UTF8, 0, p_pSrc, p_iSrcLen, p_pDst, p_iDstLen);
		}
		break;
	case ec_ucs4:
		{
			U2W(p_pSrc, p_iSrcLen, p_pDst, p_iDstLen);
		}
		break;
	default:
		break;
	}

	return true;
}

bool t_strConverter::A2B(const char *p_pSrc, int p_iSrcLen, char *p_pDst, int &p_iDstLen, int p_iSrcEncoding, int p_iDstEncoding)
{
	return false;
}

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR	(unsigned long)0x0000FFFD
#define UNI_MAX_BMP				(unsigned long)0x0000FFFF
#define UNI_MAX_UTF16			(unsigned long)0x0010FFFF
#define UNI_MAX_UTF32			(unsigned long)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32		(unsigned long)0x0010FFFF

static const int halfShift			= 10;	/* used for shifting by 10 bits */
static const unsigned long halfBase	= 0x0010000UL;
static const unsigned long halfMask	= 0x3FFUL;

#define UNI_SUR_HIGH_START		(unsigned long)0xD800
#define UNI_SUR_HIGH_END		(unsigned long)0xDBFF
#define UNI_SUR_LOW_START		(unsigned long)0xDC00
#define UNI_SUR_LOW_END			(unsigned long)0xDFFF

t_strConverter::ConversionResult t_strConverter::ConvertUTF16toUTF32(const unsigned short** sourceStart, const unsigned short* sourceEnd, unsigned long** targetStart, unsigned long* targetEnd, ConversionFlags flags)
{
	ConversionResult result = conversionOK;
	const unsigned short* source = *sourceStart;
	unsigned long* target = *targetStart;
	unsigned long ch, ch2;
	while (source < sourceEnd) {
		const unsigned short* oldSource = source; /*  In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd) {
				ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
						+ (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				} else if (flags == strictConversion) { /* it's an unpaired high surrogate */
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
			} else { /* We don't have the 16 bits following the high surrogate. */
				--source; /* return to the high surrogate */
				result = sourceExhausted;
				break;
			}
		} else if (flags == strictConversion) {
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		}
		if (target >= targetEnd) {
			source = oldSource; /* Back up source pointer! */
			result = targetExhausted; break;
		}
		*target++ = ch;
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}

t_strConverter::ConversionResult t_strConverter::ConvertUTF32toUTF16(const unsigned long** sourceStart, const unsigned long* sourceEnd, unsigned short** targetStart, unsigned short* targetEnd, ConversionFlags flags)
{
	ConversionResult result = conversionOK;
	const unsigned long* source = *sourceStart;
	unsigned short* target = *targetStart;
	while (source < sourceEnd) {
		unsigned long ch;
		if (target >= targetEnd) {
			result = targetExhausted; break;
		}
		ch = *source++;
		if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
			/* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				if (flags == strictConversion) {
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				} else {
					*target++ = UNI_REPLACEMENT_CHAR;
				}
			} else {
				*target++ = (unsigned short)ch; /* normal case */
			}
		} else if (ch > UNI_MAX_LEGAL_UTF32) {
			if (flags == strictConversion) {
				result = sourceIllegal;
			} else {
				*target++ = UNI_REPLACEMENT_CHAR;
			}
		} else {
			/* target is a character in range 0xFFFF - 0x10FFFF. */
			if (target + 1 >= targetEnd) {
				--source; /* Back up source pointer! */
				result = targetExhausted; break;
			}
			ch -= halfBase;
			*target++ = (unsigned short)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (unsigned short)((ch & halfMask) + UNI_SUR_LOW_START);
		}
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}