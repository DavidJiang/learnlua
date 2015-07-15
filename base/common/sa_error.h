#ifndef _SA_ERROR_H_
#define _SA_ERROR_H_

#include <stdio.h>
#include <stdio.h>
#include <string>

/**
*@brief 记录调用函数中出现的错误信息
*@remark
*_StreamType interface rule:
* 1. must implement operator + interface.
*/
template<typename _StreamType>
class t_errorRecord
{
public:
	t_errorRecord & operator<<(const wchar_t *p_pStr);
	t_errorRecord & operator<<(const wchar_t p_ch);
	t_errorRecord & operator<<(int p_iN);
	void Dump(void);
	_StreamType & Detail();
	void Reset();

protected:
private:
	_StreamType m_errorStream;
};

template<typename _StreamType>
t_errorRecord<_StreamType>  & t_errorRecord<_StreamType>::operator<<(const wchar_t *p_pStr)
{
	m_errorStream += p_pStr;

	return *this;
}

template<typename _StreamType>
t_errorRecord<_StreamType>  & t_errorRecord<_StreamType>::operator<<(const wchar_t p_ch)
{
	m_errorStream += p_ch;

	return *this;
}

template<typename _StreamType>
t_errorRecord<_StreamType>  & t_errorRecord<_StreamType>::operator<<(int p_iN)
{
	wchar_t szInt[16] = {0};
	swprintf(szInt, L"%d", p_iN);
	
	m_errorStream += szInt;

	return *this;
}

template<typename _StreamType>
void t_errorRecord<_StreamType>::Dump(void)
{
	wprintf( m_errorStream.c_str() );
}

template<typename _StreamType>
_StreamType & t_errorRecord<_StreamType>::Detail()
{
	return m_errorStream;
}

template<typename _StreamType>
void t_errorRecord<_StreamType>::Reset()
{
	m_errorStream = L"";
}

typedef t_errorRecord<std::wstring> t_error;

#endif
