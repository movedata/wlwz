#pragma once

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif // UNICODE

#define THROW()	throw CException (__FUNCTION__, __LINE__, GetLastError ())



class CProcessData
{

	typedef DWORD (WINAPI *PFNTCREATETHREADEX) (
		PHANDLE                 ThreadHandle,
		ACCESS_MASK             DesiredAccess,
		LPVOID                  ObjectAttributes,
		HANDLE                  ProcessHandle,
		LPTHREAD_START_ROUTINE  lpStartAddress,
		LPVOID                  lpParameter,
		BOOL                    CreateSuspended,
		DWORD                   dwStackSize,
		DWORD                   dw1,
		DWORD                   dw2,
		LPVOID                  Unknown
		);

private:
	// 目标窗口句柄
	HWND m_hWnd;
	// 目标进程句柄
	HANDLE m_hProcess;
	// 目标进程ID
	DWORD m_dwProcessId;
	// 目标进程的线程句柄
	DWORD m_dwThreadId;
	// 目标进程空间地址
	DWORD m_dwOffset;
	// 注入DLL在目标进程中的模块基址
	DWORD m_dwModule;
	// 注入DLL导出的远程调用函数在目标进程中的地址
	PTHREAD_START_ROUTINE m_pfnThreadRtn;
	// 注入DLL的路径及文件名
	_tstring m_strLibFile;
	// 远程线程函数名
	std::string m_strFunction;
	// 注入标记
	bool m_bInject;

private:
	// 初始化成员
	void Init ();
	// 判断Windows版本
	bool IsVistaOrLater ();
	// 调用本机函数创建线程
	HANDLE NtCreateRemoteThread (PTHREAD_START_ROUTINE pThreadProc, PVOID pContext);

public:
	CProcessData (const CProcessData&) = delete;
	CProcessData& operator=(const CProcessData&) = delete;
	
	// 构造函数
	// 功能:	初始化数据成员
	CProcessData ()
	{
		//OutputDebugString (L"构造\n");
		Init ();
	};
	
	// 析构函数
	// 功能:	清理堆和关闭句柄等
	~CProcessData () {
		DetachProcess ();
		//OutputDebugString (L"析构\n");
	};

public:
	// 附加目标进程并初始化
	// 功能:	查找目标进程并打开获取进程ID和进程句柄
	// 返回:	成功等于true, 失败调用false
	// 参数:	strTitle	目标进程窗口标题
	//		strClass	目标进程窗口类名
	bool AttachProcess (const _tstring& strTitle, const _tstring& strClass);
	
	// 分离目标进程并清理
	// 功能:	查找目标进程并打开获取进程ID和进程句柄
	// 返回:	成功等于true, 失败调用false
	// 参数:	strTitle	目标进程窗口标题
	//		strClass	目标进程窗口类名
	void DetachProcess ();

	// 启用调试权限
	// 功能:	启用调试权限
	// 返回:	成功等于true, 失败调用false
	// 参数:	bEnable true启用 false禁用
	bool EnableDebugPrivilege (bool bEnable);

	// 注入DLL
	// 功能:	将指定名称的DLL注入到目标进程并保存DLL名称到数据成员
	// 返回:	成功等于true, 失败调用false
	// 参数:	strLibFile	注入DLL的路径及文件名
	bool InjectLib (const _tstring& strLibFile = TEXT ("\\InjectLib.dll"));
	
	// 注销DLL
	// 功能:	从目标进程卸载之前注入的DLL
	// 返回:	成功等于true, 失败调用false
	// 参数:	无
	bool EjectLib ();

	// 远程调用
	// 功能:	调用注入DLL的导出远程函数
	// 返回:	成功等于true, 失败调用false
	// 参数:	pvContext	调用过程所需要的参数, 可以为NULL
	//		dwSize		pvContext参数的字节数, 如果pvContext等于NULL, 该参数被忽略
	//		strFunction	远程函数的导出符号
	bool RemoteCall (PVOID pvContext, DWORD dwSize, const std::string& strFunction = "GameCommand");

	// 写内存
	// 功能:	写数据到远程进程
	// 返回:	成功等于true, 失败调用false
	// 参数:	pvData		预写入的本地数据
	//		dwSize		pvData的字节数
	//		pvOffset	预写入的远程偏移
	bool WriteMem (PVOID pvData, DWORD dwSize, PVOID pvOffset);
	
	// 读内存
	// 功能:	写数据到远程进程
	// 返回:	成功等于true, 失败调用false
	// 参数:	pvData		预读取的本地缓冲区
	//		dwSize		pvData的字节数
	//		dwOffset	预读取的远程偏移
	bool ReadMem (PVOID pvData, DWORD dwSize, DWORD dwOffset = 0);

	// 更新偏移
	// 功能:	迭代到列表指定地址
	// 返回:	成功等于true, 失败调用false
	// 参数:	initOffset	偏移列表
	bool UpdateOffset (const std::initializer_list<DWORD>& initOffset);

	// 以下函数获悉内部状态
	HWND GetHWnd () const;
	HANDLE GetHandle () const;
	DWORD GetProcessId () const;
	DWORD GetThreadId () const;
};

// 定义的异常处理
class CException
{
private:
	std::string m_function;
	ULONG m_line;
	DWORD m_error;
	bool m_silent;

public:
	CException (const std::string& function, ULONG line, DWORD e, bool silent = true)
		:m_function (function), m_line (line), m_error(e), m_silent(silent) {}
	
	void Trace () const {
		TCHAR msg[128] = {};
		StringCchPrintf (msg, 128, TEXT ("函数名:%S - 错误在%d行 - 错误代码:%d\n"), m_function.c_str (), m_line, m_error);
		if (m_silent)
			OutputDebugString (msg);
		else
			MessageBox (nullptr, msg, nullptr, 0);
	}

};
