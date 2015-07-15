#ifndef _PATH_TEMP_LINUX_H_
#define _PATH_TEMP_LINUX_H_

#include "path_linux.h"

class t_pathtemp : public t_path
{
public:
	t_pathtemp();
	t_pathtemp( const wchar_t *p_pFilePath);
	explicit t_pathtemp( const wchar_t *p_pFirstPath,
		const wchar_t *p_pLastPath);
	explicit t_pathtemp( const wchar_t *p_pFirstPath, 
		const wchar_t *p_pSecondPath,
		const wchar_t *p_pLastPath);

	t_pathtemp(const t_path &p_path);

	~t_pathtemp();

	void	AppendFile( const wchar_t *p_szFile );
};

#endif  // _PATH_TEMP_LINUX_H_
