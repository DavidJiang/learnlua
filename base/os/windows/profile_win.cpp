#include "profile_win.h"
#include <assert.h>

t_performanceCounter::t_performanceCounter()
{
	::QueryPerformanceFrequency(&m_baseFreq);
}

t_performanceCounter::~t_performanceCounter()
{
	std::map<std::string, t_countItem *>::iterator it = m_items.begin();
	while( it != m_items.end() )
	{
		delete it->second;
		it = m_items.erase(it);
	}
}

void t_performanceCounter::Begin(std::string p_strName)
{
	std::map<std::string, t_countItem *>::iterator it = m_items.find(p_strName);
	if( it == m_items.end() )
	{
		t_countItem *i = new t_countItem;
		m_items[p_strName] = i;
		i->m_times = 0;
		i->m_totalCounter.QuadPart = 0;
		::QueryPerformanceCounter(&(i->m_beginCounter));
	}
	else
	{
		::QueryPerformanceCounter(&(it->second->m_beginCounter));
	}
}

void t_performanceCounter::End(std::string p_strName)
{
	LARGE_INTEGER c = {0};
	::QueryPerformanceCounter(&c);

	std::map<std::string, t_countItem *>::iterator it = m_items.find(p_strName);
	if( it == m_items.end() )
	{
		assert(false);
	}
	else
	{
		it->second->m_totalCounter.QuadPart += (c.QuadPart - it->second->m_beginCounter.QuadPart);
		++(it->second->m_times);
	}
}

void t_performanceCounter::Print()
{
	std::map<std::string, t_countItem *>::iterator it = m_items.begin();
	while( it != m_items.end() )
	{
		printf_s("\t%s: %.2f, %d\n", it->first.c_str(), (it->second->m_totalCounter.QuadPart * 1000.0) / m_baseFreq.QuadPart, it->second->m_times);
		++it;
	}
}

t_performanceCounter g_pcounter;