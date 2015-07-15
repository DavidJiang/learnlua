#ifndef _PATH_LINUX_H_
#define _PATH_LINUX_H_

#include <string>
#include <utility>

using namespace std;

/**
 * @brief referenced to python os.path module.
 */
class t_path
{
public:
	t_path();
	t_path( const wchar_t *p_pFilePath);
	explicit t_path( const wchar_t *p_pFirstPath,
					 const wchar_t *p_pLastPath);
	explicit t_path( const wchar_t *p_pFirstPath, 
					 const wchar_t *p_pSecondPath,
					 const wchar_t *p_pLastPath);

	void operator=( const wchar_t *p_pFilePath );
	t_path(const t_path &p_path);
	
	~t_path(){}

	/**
	 *@brief 返回文件所在目录
	 */
	const wstring DirName(void) const;
	/**
	 *@brief 返回路径中的文件名
	 */
	const wstring BaseName(void) const;
	
    /**
	 *@brief 将路径合法化，比如linux下将'\\'代替为'/'，去掉非法
	 *字符等等
	 */
	void NormPath(void) const;
	/**
	 *@brief 将路径和文件名组合为一个全路径
	 *@param p_pDirName 父路径名
	 *@param p_pBaseName 文件名
	 */
	void Join(const wchar_t *p_pDirName, const wchar_t *p_pBaseName);
	
	/**
	 *@brief 将路径分隔为父目录和文件名
	 */
	pair<wstring, wstring>& Split(void);
	
	const wstring &FullPath(void) const 
	{
		return m_strPath;
	}

	t_path &operator+=(const wchar_t *p_pPath);
	
protected:
	mutable wstring m_strPath;
	pair<wstring, wstring> m_pathInfo; /**< record dir path and file name. */
};



#endif
