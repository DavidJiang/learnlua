#include "pathtemp_linux.h"
#include "file_linux.h"

t_pathtemp::t_pathtemp()
:t_path()
{

}

t_pathtemp::t_pathtemp( const wchar_t *p_pFilePath )
:t_path(p_pFilePath)
{

}

t_pathtemp::t_pathtemp( const wchar_t *p_pFirstPath, const wchar_t *p_pLastPath )
:t_path(p_pFirstPath, p_pLastPath)
{

}

t_pathtemp::t_pathtemp( const wchar_t *p_pFirstPath, const wchar_t *p_pSecondPath, const wchar_t *p_pLastPath )
:t_path(p_pFirstPath, p_pSecondPath, p_pLastPath)
{

}

t_pathtemp::t_pathtemp( const t_path &p_path )
:t_path(p_path)
{

}

t_pathtemp::~t_pathtemp()
{
	if(t_fileUtil::FileExists(*this))
	{
		t_fileUtil::RemoveFile(*this);
	}
	else if(t_fileUtil::FolderExists(*this))
	{
		t_fileUtil::RemoveDir(*this);
	}
}

void t_pathtemp::AppendFile( const wchar_t *p_szFile )
{
	m_strPath += p_szFile;
}
