#include "CommandInfo.h"

// 私有辅助函数

void CProcessData::Init ()
{
	m_hWnd = nullptr;
	m_hProcess = nullptr;
	m_dwProcessId = 0;
	m_dwThreadId = 0;
	m_dwOffset = 0;
	m_dwModule = 0;
	m_pfnThreadRtn = nullptr;
	m_strLibFile.clear ();
	m_strFunction.clear ();
	m_bInject = false;
}

// 公有功能函数

bool CProcessData::AttachProcess (const _tstring & strTitle, const _tstring & strClass)
{
	bool bOK = false;
	
	try
	{
		if (nullptr == m_hWnd && nullptr == m_hProcess) {
			if (strTitle.empty () || strClass.empty ())
				THROW ();

			// 查找窗口以获得窗口句柄
			m_hWnd = FindWindow (strClass.c_str (), strTitle.c_str ());
			if (nullptr == m_hWnd) THROW ();

			// 获取进程ID
			m_dwThreadId = GetWindowThreadProcessId (m_hWnd, &m_dwProcessId);
			if (0 == m_dwProcessId) THROW ();

			// 打开进程并获取进程句柄
			m_hProcess = ::OpenProcess (
				PROCESS_QUERY_INFORMATION |		// 申请查询信息权限
				PROCESS_CREATE_THREAD |			// 申请调用 CreateRemoteThread 权限
				PROCESS_VM_OPERATION |			// 申请调用 VirtualAllocEx/VirtualFreeEx 权限
				PROCESS_VM_WRITE |				// 申请调用 WriteProcessMemory 权限
				PROCESS_VM_READ,				// 申请调用 ReadProcessMemory 权限
				FALSE, m_dwProcessId);
			if (nullptr == m_hProcess) THROW ();
		}

		bOK = true;
	}
	catch (const CException& e)
	{
		e.Trace ();
		Init ();
	}
	
	return bOK;
}

void CProcessData::DetachProcess ()
{
	if (m_bInject) EjectLib ();
	if (nullptr != m_hProcess) CloseHandle (m_hProcess);

	Init ();
}

bool CProcessData::EnableDebugPrivilege (bool bEnable)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	// 以调整权限打开当前进程令牌
	if (!OpenProcessToken (::GetCurrentProcess (), 
		TOKEN_ADJUST_PRIVILEGES, &hToken))
		return false;

	LUID luid;

	LookupPrivilegeValue (NULL, SE_DEBUG_NAME, &luid);
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnable)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;
	
	if (!AdjustTokenPrivileges (
		hToken,
		false,
		&tp,
		sizeof (TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
		return false;

	if (GetLastError () == ERROR_NOT_ALL_ASSIGNED)
		return false;

	CloseHandle (hToken);

	return true;
}

bool CProcessData::IsVistaOrLater ()
{
	OSVERSIONINFO osvi;
	ZeroMemory (&osvi, sizeof (OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&osvi);
	if (osvi.dwMajorVersion >= 6)
		return true;
	return false;
}

HANDLE CProcessData::NtCreateRemoteThread (PTHREAD_START_ROUTINE pThreadProc, PVOID pContext)
{
	HANDLE hThread = nullptr;
	PFNTCREATETHREADEX pNtCreateThreadEx = nullptr;

	try
	{
		if (IsVistaOrLater ()) {		// Vista, 7, Server2008  
			pNtCreateThreadEx = (PFNTCREATETHREADEX)
				GetProcAddress (GetModuleHandle (TEXT("ntdll.dll")), "NtCreateThreadEx");
			if (nullptr == pNtCreateThreadEx) THROW ();

			(pNtCreateThreadEx)(&hThread, 0x1FFFFF,	NULL, m_hProcess, 
				pThreadProc, pContext, false, 0, 0, 0, nullptr);
			if (nullptr == hThread) THROW ();
		}
		else {							// 2000, XP, Server2003  
			hThread = CreateRemoteThread (m_hProcess, nullptr, 0,
				pThreadProc, pContext, 0, nullptr);
			if (nullptr == hThread) THROW ();
		}
	}
	catch (const CException& e)
	{
		e.Trace ();
	}
	catch (...) {
		OutputDebugString (TEXT ("CProcessData::NtCreateRemoteThread 未知错误\n"));
	}

	return hThread;
}

bool CProcessData::InjectLib (const _tstring& strLibFile)
{
	bool bOk = false; // 假设调用失败
	HANDLE hThread = nullptr;
	PTSTR pszLibFileRemote = nullptr;

	if (m_bInject) return true;

	try {
		if (strLibFile.empty ())  THROW ();

		TCHAR szPath[MAX_PATH] = {};
		GetCurrentDirectory (sizeof (szPath), szPath);
		m_strLibFile = szPath + strLibFile;
		if (nullptr == m_hProcess) THROW ();

		// 计算 DLL 文件名字符串的字节长度
		if (m_strLibFile.empty ()) THROW ();
		DWORD cb = m_strLibFile.size () * sizeof (TCHAR) + sizeof (TCHAR);

		// 远程进程中分配 DLL 文件名字符串字节长度的空间
		pszLibFileRemote = (PTSTR)
			VirtualAllocEx (m_hProcess, nullptr, cb, MEM_COMMIT, PAGE_READWRITE);
		if (nullptr == pszLibFileRemote) THROW ();

		// 复制 DLL 文件名字符串到远程进程
		if (!WriteMem ((PVOID)m_strLibFile.c_str (), cb, pszLibFileRemote)) THROW ();

		// 获取 Kernel32.dll 下 LoadLibrary 的固定函数地址
#ifdef UNICODE
		char szLoadLibrary[] = { "LoadLibraryW" };
#else
		char szLoadLibrary[] = { "LoadLibraryA" };
#endif // UNICODE

		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress (GetModuleHandle (TEXT ("Kernel32")), szLoadLibrary);
		if (nullptr == pfnThreadRtn) THROW ();

		hThread = NtCreateRemoteThread (pfnThreadRtn, pszLibFileRemote);
		if (nullptr == hThread) THROW ();

		// 等待远程线程结束
		WaitForSingleObject (hThread, INFINITE);

		// 获取 LoadLibraryW 的返回值，即注入 DLL 的基址
		GetExitCodeThread (hThread, &m_dwModule);
		if (0 == m_dwModule) THROW ();

		m_bInject = true;

		bOk = true; // 成功
	}
	catch(CException& e) { 
		e.Trace ();
	}
	catch (...) {
		OutputDebugString (TEXT ("CProcessData::InjectLib 未知错误\n"));
	}

	// 最后的清理
	if (nullptr != pszLibFileRemote) {
		VirtualFreeEx (m_hProcess, pszLibFileRemote, 0, MEM_RELEASE);
	}

	if (nullptr != hThread)
		CloseHandle (hThread);

	return(bOk);
}

bool CProcessData::EjectLib ()
{

	bool bOk = false; // 假设调用失败
	HANDLE hthSnapshot = NULL;
	HANDLE hThread = NULL;

	if (!m_bInject) return false;
	
	try {
		// 抓取目标进程快照
		hthSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPMODULE, m_dwProcessId);
		if (INVALID_HANDLE_VALUE == hthSnapshot) THROW ();

		// 获取所需的模块
		MODULEENTRY32 me = { sizeof (me) };
		bool bFound = false;
		bool bMoreMods = Module32First (hthSnapshot, &me);
		for (; bMoreMods; bMoreMods = Module32Next (hthSnapshot, &me)) {
			bFound = (0 == _tcsicmp (me.szModule, m_strLibFile.c_str ())) ||
				(0 == _tcsicmp (me.szExePath, m_strLibFile.c_str ()));
			if (bFound) break;
		}
		if (!bFound) THROW ();  // 没有找到指定模块则结束执行

		if (nullptr == m_hProcess) THROW ();

		// 获取 Kernel32.dll 下 FreeLibrary 的固定函数地址
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress (GetModuleHandle (TEXT ("Kernel32")), "FreeLibrary");
		if (nullptr == pfnThreadRtn) THROW ();

		// 创建远程线程并调用 FreeLibrary(注入 DLL 的句柄)
		hThread = NtCreateRemoteThread (pfnThreadRtn, me.modBaseAddr);
		if (nullptr == hThread) THROW ();

		// 等待远程线程结束
		WaitForSingleObject (hThread, INFINITE);

		m_bInject = false;

		bOk = true; // 成功
	}
	catch(const CException& e) { 
		e.Trace ();
	}
	catch (...) {
		OutputDebugString (TEXT ("CProcessData::EjectLib 未知错误\n"));
	}

	// 最后的清理
	if (nullptr != hthSnapshot)
		CloseHandle (hthSnapshot);

	if (nullptr != hThread)
		CloseHandle (hThread);

	return(bOk);
}

bool CProcessData::RemoteCall (PVOID pvContext, DWORD dwSize, const std::string& strFunction)
{
	bool bOk = false;
	HANDLE hThread = nullptr;
	HMODULE hModule = nullptr;
	PVOID pvBuffer = nullptr;

	try
	{
		
		if (nullptr == m_hProcess || 0 == m_dwModule)
			THROW();

		if (nullptr == m_pfnThreadRtn || strFunction != m_strFunction) {
			hModule = LoadLibrary (m_strLibFile.c_str ());
			if (nullptr == hModule) THROW ();

			PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
				GetProcAddress (hModule, strFunction.c_str ());
			if (nullptr == pfnThreadRtn) THROW ();
			
			// 计算当前载入模块与目标载入模块相对远程函数的偏移
			m_pfnThreadRtn = (PTHREAD_START_ROUTINE)(((DWORD)pfnThreadRtn) - ((DWORD)hModule) + m_dwModule);

			// 保存符号名
			m_strFunction = strFunction;
		}

		if (nullptr != pvContext) {
			pvBuffer = VirtualAllocEx (m_hProcess, NULL, dwSize,
				MEM_COMMIT, PAGE_READWRITE);
			if (nullptr == pvBuffer) THROW();

			if (!WriteMem (pvContext, dwSize, pvBuffer)) THROW();
		}

		hThread = NtCreateRemoteThread (m_pfnThreadRtn, pvBuffer);
		if (nullptr == hThread) THROW();

		// 等待远程线程结束
		WaitForSingleObject (hThread, INFINITE);

		bOk = true;

	}
	catch (const CException& e)
	{
		e.Trace ();
	}
	catch (...) {
		OutputDebugString (TEXT ("CProcessData::RemoteCall 未知错误\n"));
	}

	if (nullptr != hModule)
		FreeLibrary (hModule);

	if (nullptr != pvBuffer)
		VirtualFreeEx (m_hProcess, pvBuffer, 0, MEM_RELEASE);

	if (nullptr != hThread)
		CloseHandle (hThread);


	return bOk;
}

bool CProcessData::WriteMem (PVOID pvData, DWORD dwSize, PVOID pvOffset)
{
	return WriteProcessMemory (m_hProcess, pvOffset, pvData, dwSize, nullptr);
}

bool CProcessData::ReadMem (PVOID pvData, DWORD dwSize, DWORD dwOffset)
{
	return ReadProcessMemory (m_hProcess, (PVOID)(m_dwOffset + dwOffset), pvData, dwSize, nullptr);
}

bool CProcessData::UpdateOffset (const std::initializer_list<DWORD>& initOffset)
{
	bool bOK = false;
	DWORD dwBuffer = 0;
	m_dwOffset = 0;

	try
	{
		for (DWORD offset : initOffset)
			if (bOK = ReadMem (&dwBuffer, sizeof (dwBuffer), offset))
				m_dwOffset = dwBuffer;
	}
	catch (...) {
		bOK = false; 
		OutputDebugString (TEXT ("CProcessData::UpdateOffset 未知错误\n"));
	}

	return bOK;
}

HWND CProcessData::GetHWnd () const
{
	return m_hWnd;
}

HANDLE CProcessData::GetHandle () const
{
	return m_hProcess;
}

DWORD CProcessData::GetProcessId () const
{
	return m_dwProcessId;
}

DWORD CProcessData::GetThreadId () const
{
	return m_dwThreadId;
}