#ifndef _STR_CONVERTER_H_
#define _STR_CONVERTER_H_

/**
 *@brief used to convert wchar_t  to char * (with kinds of encoding type.)
 */
class t_strConverter
{
public:
	/**
	 *@brief dest char * encoding type.
	 */
	enum _eEncodingType
	{
		ec_utf_8 = 0,
		ec_gbk,
		ec_ucs2,
		ec_ucs4,
	};

	/**
	 *@brief ucs4 string to ucs2 string.
	 *@param p_iDstSize [in] p_pDest buffer size, [out] the p_pDest length
	 */
	static bool W2U(const wchar_t *p_pSrc, char *p_pDst, int &p_iDstLen);

	/**
	 *@brief ucs2 string to ucs4 string.
	 */
	static bool U2W(const char *p_pSrc, int p_iSrcLen, wchar_t *p_pDst, int &p_iDstLen);

	/**
	 *@brief unicode to any encoding.
	 */	
	static bool W2C(const wchar_t *p_pSrc, char *p_pDst, int &p_iDstLen, int p_iEncodingType = -1);
	
	/**
	 *@brief gbk to any encoding.
	 */	
	static bool C2W(const char *p_pSrc, int p_iSrcLen, wchar_t *p_pDst, int &p_iDstLen, int p_iEncodingType = -1);
	
	static bool A2B(const char *p_pSrc, int p_iSrcLen, char *p_pDst, int &p_iDstLen, int p_iSrcEncoding, int p_iDstEncoding);

private:
	typedef enum
	{
		conversionOK, 		/* conversion successful */
		sourceExhausted,	/* partial character in source, but hit end */
		targetExhausted,	/* insuff. room in target for conversion */
		sourceIllegal		/* source sequence is illegal/malformed */
	}
	ConversionResult;

	typedef enum
	{
		strictConversion = 0,
		lenientConversion
	}
	ConversionFlags;

	static ConversionResult ConvertUTF16toUTF32 (
		const unsigned short** sourceStart, const unsigned short* sourceEnd, 
		unsigned long** targetStart, unsigned long* targetEnd, ConversionFlags flags);

	static ConversionResult ConvertUTF32toUTF16 (
		const unsigned long** sourceStart, const unsigned long* sourceEnd, 
		unsigned short** targetStart, unsigned short* targetEnd, ConversionFlags flags);
};
#endif
