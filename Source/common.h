#pragma once

// 仅支持XP以上系统运行
#define _WIN32_WINNT _WIN32_WINNT_WINXP 
#define WINVER       _WIN32_WINNT_WINXP 

// 总以 UNICODE 版本函数编译
#ifndef UNICODE
#define UNICODE
#endif

// 定义 UNICODE 同时定义 _UNICODE
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

///////////////////////// 包含 Windows 定义 /////////////////////////

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#pragma warning(push, 3)
#include <Windows.h>
#pragma warning(pop) 
#pragma warning(push, 4)
#include <CommCtrl.h>
#include <process.h>

// 禁止未引用参数警告
#pragma warning(disable:4100)

///////////////////////// 公用函数及宏定义 /////////////////////////

// 处理对话框消息
#define HANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

// 设置程序图标
inline void SetDlgIcons (HWND hWnd, int idi) {
	HICON hIcon = idi == 0 ? LoadIcon (NULL, IDI_APPLICATION)
		: LoadIcon ((HINSTANCE)GetWindowLongPtr (hWnd, GWLP_HINSTANCE),
			MAKEINTRESOURCE (idi));

	SendMessage (hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage (hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

// C++ 创建线程
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
   pvParam, dwCreateFlags, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                          \
         (void *)        (psa),                         \
         (unsigned)      (cbStackSize),                 \
         (PTHREAD_START) (pfnStartAddr),                \
         (void *)        (pvParam),                     \
         (unsigned)      (dwCreateFlags),               \
         (unsigned *)    (pdwThreadId)))

/////////////////////////// 链接样式清单 ////////////////////////////


#pragma comment(linker, "/nodefaultlib:oldnames.lib")

// Needed for supporting XP/Vista styles.
#if defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


///////////////////////////////////////////////////////////////////////////////
/*
BOOL Dlg_OnInitDialog (HWND hWnd, HWND hwndFocus, LPARAM lParam) {

	SetDlgIcons (hWnd, 0);

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////

void Dlg_OnCommand (HWND hWnd, int id, HWND hwndCtl, UINT codeNotify) {

	switch (id) {

		case IDCANCEL:
			// 关闭对话框
			EndDialog (hWnd, id);
			break;

		case IDOK:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////

INT_PTR WINAPI Dlg_Proc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		// 初始化对话框
		HANDLE_DLGMSG (hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		// 处理命令消息
		HANDLE_DLGMSG (hwnd, WM_COMMAND, Dlg_OnCommand);
	}

	return(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain (HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{

	// 创建对话框
	DialogBoxParam (hinstExe, MAKEINTRESOURCE (IDD_DLG),
		NULL, Dlg_Proc, NULL);

	return(0);
}
*/
///////////////////////////////////////////////////////////////////////////////
