#ifndef _PROFILE_WIN_H_
#define _PROFILE_WIN_H_

#include <stdio.h>
#include <Windows.h>
#include <map>
#include <string>

/**
 *@brief 用于统计函数的性能，将性能信息打印到visual studio的output窗口
 *目前时间评估是以us为单位
 *@author: cxwshawn@yeah.net
 */
class t_perfCounter
{
public:
	const static unsigned int MAX_PROFILE_INFO_LEN = 8096;

	explicit t_perfCounter(const char *p_pInfo, bool bStart = false )
	{
		m_bStarted = bStart;
		_snprintf_s(m_szInfo, MAX_PROFILE_INFO_LEN, "%s", p_pInfo);
		QueryPerformanceFrequency(&m_liFreq);
		if( bStart == true )
		{
			QueryPerformanceCounter(&m_liStartCounter);
			m_bStarted = true;
		}
		m_bStopped = false;
	}
	explicit t_perfCounter(const char *p_pSrcFile, 
		int p_iFileLine, bool bStart = false )
	{
		m_bStarted = bStart;
		_snprintf_s(m_szInfo, MAX_PROFILE_INFO_LEN, "%s:%d", p_pSrcFile, p_iFileLine);
		QueryPerformanceFrequency(&m_liFreq);
		if( bStart == true )
		{
			QueryPerformanceCounter(&m_liStartCounter);
			m_bStarted = true;
		}
		m_bStopped = false;
	}

	void Start(void)
	{
		QueryPerformanceCounter(&m_liStartCounter);
	}
	void Stop(void)
	{
		QueryPerformanceCounter(&m_liEndCounter);
		m_bStopped = true;
	}

	~t_perfCounter()
	{
		if ( m_bStopped == false )
		{
			QueryPerformanceCounter(&m_liEndCounter);
		}
		
		_snprintf_s(m_szInfo, 1024, "%s cost time %d us\n", m_szInfo, (m_liEndCounter.QuadPart - m_liStartCounter.QuadPart) * 
			1000 * 1000 / m_liFreq.QuadPart );
		OutputDebugStringA(m_szInfo);
	}
protected:
	LARGE_INTEGER m_liFreq;
	LARGE_INTEGER m_liStartCounter;
	LARGE_INTEGER m_liEndCounter;
	char m_szInfo[MAX_PROFILE_INFO_LEN];
	bool m_bStarted;
	bool m_bStopped;
};

/**
 *@brief 用于统计代码片段的性能，以PCBEGIN开始，PCEND结束，没有PCBEGIN/PCEND的项目（不配对）不计入最终结果
 *所有项目统计完毕后，调用PCRESULT打印结果到控制台输出窗口（注意统计本身的性能问题）
 *目前时间评估是以ms为单位
 *@author: helitao
 */
class t_performanceCounter
{
public:
	typedef struct _CountItem
	{
		LARGE_INTEGER m_beginCounter;
		LARGE_INTEGER m_totalCounter;
		int m_times;
	}t_countItem;

	t_performanceCounter();
	~t_performanceCounter();

	void Begin(std::string p_strName);
	void End(std::string p_strName);
	void Print();

private:
	std::map<std::string, t_countItem *> m_items;
	LARGE_INTEGER m_baseFreq;
};

extern t_performanceCounter g_pcounter;
#define PCBEGIN(x)	g_pcounter.Begin(x)
#define PCEND(x)	g_pcounter.End(x)
#define PCRESULT()	g_pcounter.Print()

#endif
