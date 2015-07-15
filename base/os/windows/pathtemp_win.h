#ifndef _PATH_TEMP_WIN_H_
#define _PATH_TEMP_WIN_H_

#include "path_win.h"

class t_pathtemp : public t_path
{
public:
	t_pathtemp();
	explicit t_pathtemp( const wchar_t *p_pFirstPath,
					 const wchar_t *p_pLastPath);
	explicit t_pathtemp( const wchar_t *p_pFirstPath, 
					 const wchar_t *p_pSecondPath,
					 const wchar_t *p_pLastPath);

	t_pathtemp(const t_path &p_oPath);
	t_pathtemp(const std::wstring &p_strPath);
	t_pathtemp(const wchar_t *p_szPath);

	~t_pathtemp();

	void	AppendFile( const wchar_t *p_szFile );
};

#endif	// _PATH_TEMP_WIN_H_