// author: helitao
#ifndef _FILETEXT_WIN_H_
#define _FILETEXT_WIN_H_

#include <vector>
#include "os/sa_file.h"
#include "os/sa_mutex.h"
class t_filetext : protected t_file
{
public:
	enum e_fileEncoding
	{
		c_encGBK = 0,
		c_encUtf8,
		c_encUtf16LE,
		c_encUtf16BE,
		c_encUtf32LE,
		c_encUtf32BE,
		c_encMax,
	};

	t_filetext();
	t_filetext(const wchar_t *p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	t_filetext(const std::wstring &p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	t_filetext(const t_path &p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	~t_filetext();

	bool Open(const wchar_t *p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	bool Open(const std::wstring &p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	bool Open(const t_path &p_fileName, int p_mode, e_fileEncoding p_cEncoding);
	bool Flush();
	bool Close();

	wchar_t *ReadLine(wchar_t *p_szLine, int p_nLineSize);
	bool WriteLine(const wchar_t *p_szFormat, ...);

	const wchar_t *GetLastError();
	e_fileEncoding GetEncoding();

private:
	e_fileEncoding ReadEncoding();
	bool FeedContent();
	void ClearWriteLines();

private:
	static char s_aBOMLength[c_encMax];
	static const char *s_aReturnBits[c_encMax];
	static char s_aReturnBitLengths[c_encMax];
	static char s_aEncodingMap[c_encMax];
	static unsigned char s_aBOMs[c_encMax][4];

	e_fileEncoding m_cEncoding;
	char *m_pROContent;	// read only content buffer
	int m_nROHead;
	int m_nROTail;
	int m_nROLength;
	bool m_bROEOF;
	
	t_threadMutex m_locker;

	std::vector<wchar_t *> m_vWriteLines;
};

#endif	// _FILETEXT_WIN_H_
