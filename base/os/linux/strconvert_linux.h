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
	static bool W2U(const wchar_t *p_pSrc, char *p_pDst, int & p_iDstSize);
	
	/**
	 *@brief ucs2 string to ucs4 string.
	 */
	static bool U2W(const char *p_pSrc, int p_iSrcSize, wchar_t *p_pDest, int &p_iDstSize);

	static bool U2C(const char *p_pSrc, char *p_pDest, int &p_iDstSize, int p_iEncodingType = -1);

	/**
	 *@brief
	 *@param p_iDstSize in:the p_pDest buffer size, out: the actual out buffer used.
	 */
	static bool W2C(const wchar_t *p_pSrc, char *p_pDest,
					int& p_iDstSize, int p_iEncodingType = -1);
	
	static bool C2W(const char *p_pSrc, int p_iSrcLen,
					wchar_t *p_pDst, int &p_iDstSize, int p_iEncodingType = -1);
	
	static bool A2B(const char *p_pSrc, int p_iSrcLen,
					char *p_pDst, int &p_iDstSize, 
					int p_iSrcEncoding, int p_iDstEncoding);
};
#endif
