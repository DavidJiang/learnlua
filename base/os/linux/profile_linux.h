#ifndef _PROFILE_LINUX_H_
#define _PROFILE_LINUX_H_

#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

class t_perfCounter
{
public:
	const static unsigned int MAX_PROFILE_INFO_LEN = 8096;

	explicit t_perfCounter(const char *p_pInfo, bool bStart = false )
	{
		m_bStarted = bStart;
		snprintf(m_szInfo, MAX_PROFILE_INFO_LEN, "%s", p_pInfo);
		if( bStart == true )
		{
			int iRet = gettimeofday(&m_startValue, NULL);
			if( iRet == -1 )
			{
				perror( strerror(errno));
			}
			m_bStarted = true;
		}
		m_bStopped = false;
	}
	explicit t_perfCounter(const char *p_pSrcFile, 
						   uint p_iFileLine, bool bStart = false )
	{
		m_bStarted = bStart;
		snprintf(m_szInfo, MAX_PROFILE_INFO_LEN, "%s:%d", p_pSrcFile, p_iFileLine);
		if( bStart == true )
		{
			int iRet = gettimeofday(&m_startValue, NULL);
			if( iRet == -1 )
			{
				perror( strerror(errno));
			}
			m_bStarted = true;
		}
		m_bStopped = false;
	}
	void Start(void)
	{
		gettimeofday(&m_startValue, NULL);
		m_bStarted = true;
	}
	void Stop(void)
	{
		assert(m_bStarted == true);
		gettimeofday(&m_stopValue, NULL);
		m_bStopped = true;
	}
	~t_perfCounter()
	{
		if( m_bStopped == false )
		{
			int iRet = gettimeofday(&m_stopValue, NULL);
			if( iRet == -1 )
			{
				perror(strerror(errno ) );
			}
		}
		//printf("stop s:%d,u:%d\tstart s:%d,u:%d\n", 
		//	   m_stopValue.tv_sec, m_stopValue.tv_usec,
		//	   m_startValue.tv_sec, m_startValue.tv_usec);
		
		int iCostUs = (m_stopValue.tv_sec - m_startValue.tv_sec) * 1000 *
			1000 + (m_stopValue.tv_usec - m_startValue.tv_usec);
		printf("%s cost time %d us\n", m_szInfo,iCostUs);
	}

	/**< todo: implement this. */
	int RedirectToFile(const char *p_pFileName);

private:
	char m_szInfo[MAX_PROFILE_INFO_LEN];
	struct timeval m_startValue;
	struct timeval m_stopValue;
	bool m_bStarted;
	bool m_bStopped;
};
#endif
