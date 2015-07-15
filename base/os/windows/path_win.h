// author: helitao
#ifndef _PATH_WIN_H_
#define _PATH_WIN_H_

#include "string_win.h"

class t_path
{
public:
	t_path();
	explicit t_path( const wchar_t *p_pFirstPath,
					 const wchar_t *p_pLastPath);
	explicit t_path( const wchar_t *p_pFirstPath, 
					 const wchar_t *p_pSecondPath,
					 const wchar_t *p_pLastPath);

	t_path(const t_path &p_oPath);
	t_path(const std::wstring &p_strPath);
	t_path(const wchar_t *p_szPath);

	~t_path();

	t_path &operator=(const t_path &p_oPath);
	t_path &operator=(const std::wstring &p_strPath);
	t_path &operator=(const wchar_t *p_szPath);

	t_path &operator+=(const t_path &p_oPath);
	t_path &operator+=(const std::wstring &p_strPath);
	t_path &operator+=(const wchar_t *p_szPath);

	void NormPath() const;
	const std::wstring DirName() const;
	const std::wstring BaseName() const;
	const std::wstring &FullPath() const;

protected:
	void Normalize();
protected:
	std::wstring	m_strPath;
};

#endif	// _PATH_WIN_H_