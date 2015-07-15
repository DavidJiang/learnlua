// author: helitao
#include "sa_filetext.h"
#include <assert.h>
#include "os/sa_config.h"
#include "os/sa_strconvert.h"

#include <stdarg.h>

char t_filetext::s_aBOMLength[c_encMax] = {0, 3, 2, 2, 4, 4};
const char *t_filetext::s_aReturnBits[c_encMax] = {"\n", "\n", "\n\0", "\0\n", "\n\0\0\0", "\0\0\0\n"};
char t_filetext::s_aReturnBitLengths[c_encMax] = {1, 1, 2, 2, 4, 4};
char t_filetext::s_aEncodingMap[c_encMax] = {t_strConverter::ec_gbk, t_strConverter::ec_utf_8, t_strConverter::ec_ucs2, t_strConverter::ec_ucs2, t_strConverter::ec_ucs4, t_strConverter::ec_ucs4};
unsigned char t_filetext::s_aBOMs[c_encMax][4] = {{0}, {0xEF, 0xBB, 0xBF, 0x00}, {0xFF, 0xFE, 0x00, 0x00}, {0xFE, 0xFF, 0x00, 0x00}, {0xFF, 0xFE, 0x00, 0x00}, {0x00, 0x00, 0xFE, 0xFF}};

t_filetext::t_filetext()
{
	m_locker.Open();
	m_cEncoding = c_encGBK;
	m_pROContent = 0;
	m_nROHead = 0;
	m_nROTail = 0;
	m_nROLength = 0;
	m_bROEOF = false;
}

t_filetext::t_filetext(const wchar_t *p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	m_locker.Open();
	Open(p_fileName, p_mode, p_cEncoding);
}

t_filetext::t_filetext(const std::wstring &p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	m_locker.Open();
	Open(p_fileName.c_str(), p_mode, p_cEncoding);
}

t_filetext::t_filetext(const t_path &p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	m_locker.Open();
	Open(p_fileName.FullPath().c_str(), p_mode, p_cEncoding);
}

t_filetext::~t_filetext()
{
	Close();
	m_locker.Close();
}

bool t_filetext::Open(const wchar_t *p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	bool bRet = t_file::Open(p_fileName, p_mode);

	if( !bRet )
	{
		return false;
	}

	// 不同的打开标记，应用不同的encoding来源
	// ec_in,			文件
	// ec_out,			p_cEncoding
	// ec_app,			文件
	// ec_in|ec_out,	文件
	// ec_trunc,		p_cEncoding

	if( p_mode == ec_in || p_mode == ec_app || p_mode == (ec_in|ec_out) )
	{
		m_cEncoding = ReadEncoding();
		m_nROLength = GetSize() - s_aBOMLength[m_cEncoding];
		m_nROHead = 0;
		m_nROTail = 0;
		m_bROEOF = false;
		m_pROContent = new char[m_nROLength + 1];
		Seek(s_aBOMLength[m_cEncoding], ec_seekSet);
		FeedContent();
	}
	else if( p_mode == ec_out || p_mode == (ec_out | ec_trunc) )
	{
		m_cEncoding = p_cEncoding;
		// 写入BOM
		int nWritten = 0;
		Write(s_aBOMs[m_cEncoding], s_aBOMLength[m_cEncoding], nWritten);
		assert(s_aBOMLength[m_cEncoding] == nWritten);
	}

	return true;
}

bool t_filetext::Open(const std::wstring &p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	return Open(p_fileName.c_str(), p_mode, p_cEncoding);
}

bool t_filetext::Open(const t_path &p_fileName, int p_mode, e_fileEncoding p_cEncoding)
{
	return Open(p_fileName.FullPath().c_str(), p_mode, p_cEncoding);
}

bool t_filetext::Flush()
{
	m_locker.Lock();

	std::vector<wchar_t *>::iterator it = m_vWriteLines.begin();
	while ( it != m_vWriteLines.end() )
	{
		int nLen = (wcslen(*it) + 1) * sizeof(wchar_t);
		char *p = new char[nLen];
		t_strConverter::W2C(*it, p, nLen, s_aEncodingMap[m_cEncoding]);
		p[nLen] = 0;
		int nWritten = 0;
		Write((unsigned char *)p, nLen, nWritten);
		delete[] p;
		delete[] (*it);
		++it;
	}

	m_vWriteLines.clear();

	m_locker.Unlock();

	return true;
}

bool t_filetext::Close()
{
	if( m_pROContent )
	{
		delete[] m_pROContent;
	}

	m_pROContent = 0;
	m_nROHead = 0;
	m_nROTail = 0;
	m_nROLength = 0;
	m_bROEOF = false;

	Flush();
	ClearWriteLines();

	return t_file::Close();
}

wchar_t *t_filetext::ReadLine(wchar_t *p_szLine, int p_nLineSize)
{
	if( !IsOpen() || !m_pROContent || !p_szLine || p_nLineSize < 0 || m_openMode == ec_out || m_openMode == ec_trunc )
	{
		return 0;
	}

	if( m_nROHead >= m_nROTail )
	{
		return 0;
	}

	// 从ROHead向ROTail搜索换行符，如果ROTail-ROHead不足（2-4）个字节，就从文件里补充上
	int nHead = m_nROHead;
	while( nHead < m_nROTail )
	{
		if( !m_bROEOF && (nHead + s_aReturnBitLengths[m_cEncoding] >= m_nROTail) )
		{
			FeedContent();
		}

		if( s_aReturnBitLengths[m_cEncoding] == 1 && m_pROContent[nHead] == s_aReturnBits[m_cEncoding][0] )
		{
			break;
		}
		if( s_aReturnBitLengths[m_cEncoding] == 2 && m_pROContent[nHead] == s_aReturnBits[m_cEncoding][0] && m_pROContent[nHead + 1] == s_aReturnBits[m_cEncoding][1] )
		{
			break;
		}
		if( s_aReturnBitLengths[m_cEncoding] == 4 && m_pROContent[nHead] == s_aReturnBits[m_cEncoding][0] && m_pROContent[nHead + 1] == s_aReturnBits[m_cEncoding][1] && m_pROContent[nHead + 2] == s_aReturnBits[m_cEncoding][2] && m_pROContent[nHead + 3] == s_aReturnBits[m_cEncoding][3] )
		{
			break;
		}

		++nHead;
	}

	t_strConverter::C2W(m_pROContent + m_nROHead, nHead - m_nROHead, p_szLine, p_nLineSize, s_aEncodingMap[m_cEncoding]);
	p_szLine[p_nLineSize / sizeof(wchar_t)] = 0;

	m_nROHead = nHead + s_aReturnBitLengths[m_cEncoding];

	return p_szLine;
}

bool t_filetext::WriteLine(const wchar_t *p_szFormat, ...)
{
	if( !p_szFormat )
	{
		t_file::m_szLastError = L"参数错误";
		return false;
	}

	va_list args;
	va_start(args, p_szFormat);

#ifdef SA_WIN
	int len = _vscwprintf(p_szFormat, args);
#else
	int len = 2048;
#endif

	wchar_t *p = new wchar_t[len + 1];
	int rev = vswprintf(p, len + 1, p_szFormat, args);
	// 如果被截断
	if( rev == -1 )
	{
		rev = len;
	}
	p[len] = 0;
	va_end(args);

	m_locker.Lock();

	m_vWriteLines.push_back(p);

	m_locker.Unlock();

	return true;
}

const wchar_t *t_filetext::GetLastError()
{
	return t_file::GetLastError();
}

t_filetext::e_fileEncoding t_filetext::GetEncoding()
{
	return m_cEncoding;
}

t_filetext::e_fileEncoding t_filetext::ReadEncoding()
{
	// 读取BOM，判断文件编码。如果没有BOM，则默认以gbk格式打开
	// 参考网址：http://www.unicode.org/faq/utf_bom.html
	// Bytes			Encoding Form
	// 00 00 FE FF		UTF-32, big-endian
	// FF FE 00 00		UTF-32, little-endian
	// FE FF			UTF-16, big-endian
	// FF FE			UTF-16, little-endian
	// EF BB BF			UTF-8

	// 读取前4个字节
	unsigned char bom[4] = {0};
	int nRead = 0;
	if( !Read(bom, 4, nRead) )
	{
		return c_encGBK;
	}

	e_fileEncoding encoding = c_encGBK;

	if( nRead >= 4 && bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF )
	{
		encoding = c_encUtf32BE;
	}
	else if( nRead >= 4 && bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00 )
	{
		encoding = c_encUtf32LE;
	}
	else if( nRead >= 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF )
	{
		encoding = c_encUtf8;
	}
	else if( nRead >= 2 && bom[0] == 0xFE && bom[1] == 0xFF )
	{
		encoding = c_encUtf16BE;
	}
	else if( nRead >= 2 && bom[0] == 0xFF && bom[1] == 0xFE )
	{
		encoding = c_encUtf16LE;
	}
	else
	{
		encoding = c_encGBK;
	}

	return encoding;
}

bool t_filetext::FeedContent()
{
	if( m_bROEOF )
	{
		return false;
	}

	int nRead = m_nROLength - m_nROTail;
	if( nRead <= 1024 )
	{
		m_bROEOF = true;
	}
	else
	{
		nRead = 1024;
	}

	if( !Read((unsigned char *)m_pROContent + m_nROTail, nRead, nRead) )
	{
		return false;
	}

	m_nROTail += nRead;

	return true;
}

void t_filetext::ClearWriteLines()
{
	std::vector<wchar_t *>::iterator it = m_vWriteLines.begin();
	while ( it != m_vWriteLines.end() )
	{
		delete[] *it;
		++it;
	}
	m_vWriteLines.clear();
}
