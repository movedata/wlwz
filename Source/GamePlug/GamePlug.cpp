#include "CommandInfo.h"

HINSTANCE g_hInstExe;
UINT_PTR g_uiTimer = 0;
bool g_bAutoTestExit = false;

BOOL Dlg_OnInitDialog (HWND hWnd, HWND hwndFocus, LPARAM lParam) {

	SetDlgIcons (hWnd, 0);
	
	// 初始化TAB控件

	static PTSTR pPageTitles[] = {
		TEXT ("信息"),
		TEXT ("技能"),
		TEXT ("聊天"),
		TEXT ("动作"),
		TEXT ("移动"),
		TEXT ("挂机")
	};

	TCITEM  tie;
	tie.mask = TCIF_TEXT;

	RECT rect;
	for (size_t i = 0; i < g_dwPageMax; i++)
	{
		g_hPages[i] =
			CreateDialog (g_hInstExe, MAKEINTRESOURCE (IDD_DLG_PLAYER_PAGE + i),
				GetDlgItem (hWnd, IDC_GAME_INFO), Dlg_Child);

		tie.pszText = pPageTitles[i];
		SendDlgItemMessage (hWnd,
			IDC_GAME_INFO, TCM_INSERTITEM, i, (LPARAM)&tie);

		if (0 == i) {
			// 仅需获得第一个客户端窗口尺寸进行调整
			GetClientRect (g_hPages[i], &rect);
			rect.top += 20;
			rect.left += 0;
			rect.right -= 1;
			rect.bottom -= 2;
		}

		MoveWindow (g_hPages[i], rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, true);

	}

	ShowWindow (g_hPages[0], SW_SHOW);
	
	//设置聊天文本框的最大字符数
	SendDlgItemMessage (g_hPages[2],
		IDC_CHATCONTENT, EM_SETLIMITTEXT, 77, 0);

	//设置聊天间隔时间文本框的最大字符数
	SendDlgItemMessage (g_hPages[2],
		IDC_INTERVAL_TIME, EM_SETLIMITTEXT, 4, 0);

	//设置挂机最小血量
	SendDlgItemMessage (g_hPages[5],
		IDC_HP_MIN, EM_SETLIMITTEXT, 4, 0);

	//设置挂机恢复默认
	CheckDlgButton (g_hPages[5], IDC_HP_RESTORE, BST_CHECKED);


	//进程控制信息
	ProcessControl (hWnd);

	g_hThreads[0] = chBEGINTHREADEX (nullptr, 0, AutoTestThread, hWnd, CREATE_SUSPENDED, 0);
	g_hThreads[1] = chBEGINTHREADEX (nullptr, 0, AutoChatThread, g_hPages[2], CREATE_SUSPENDED, 0);
	g_hThreads[2] = chBEGINTHREADEX (nullptr, 0, AutoInfoThread, g_hPages[0], CREATE_SUSPENDED, 0);
	g_hThreads[3] = chBEGINTHREADEX (nullptr, 0, AutoOnHookThread, g_hPages[5], CREATE_SUSPENDED, 0);
	for (size_t i = 0; i < g_dwThreadMax; i++)
	{
		if (nullptr == g_hThreads[i]) {
			MsgBox (TEXT ("检测线程创建失败,需要重新启动本程序!"));
		}
		else {
			SetPriorityClass (g_hThreads[i], BELOW_NORMAL_PRIORITY_CLASS);
			ResumeThread (g_hThreads[i]);
		}
	}

	// 初始化通知事件
	g_hOnlineEvent = CreateEvent (nullptr, true, false, nullptr);
	if (nullptr == g_hOnlineEvent)
		MsgBox (TEXT ("在线通知事件创建失败,需要重新启动本程序!"));

	g_hExitEvent = CreateEvent (nullptr, true, false, nullptr);
	if (nullptr == g_hExitEvent)
		MsgBox (TEXT ("退出通知事件创建失败,需要重新启动本程序!"));

	g_hAutoChatEvent = CreateEvent (nullptr, true, false, nullptr);
	if (nullptr == g_hAutoChatEvent)
		MsgBox (TEXT ("自动聊天通知事件创建失败,需要重新启动本程序!"));
	
	g_hAutoOnHookEvent = CreateEvent (nullptr, true, false, nullptr);
	if (nullptr == g_hAutoOnHookEvent)
		MsgBox (TEXT ("自动挂机通知事件创建失败,需要重新启动本程序!"));
	
	g_hAutoInfoEvent = CreateEvent (nullptr, true, true, nullptr);
	if (nullptr == g_hAutoOnHookEvent)
		MsgBox (TEXT ("自动信息通知事件创建失败,需要重新启动本程序!"));


	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////

void Cls_OnTimer (HWND hWnd, UINT id) {
}

///////////////////////////////////////////////////////////////////////////////

void Dlg_OnCommand (HWND hWnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id) {
		case IDCANCEL:
			// 退出事件有信号
			SetEvent (g_hExitEvent);
			WaitForThreadQuit (g_hThreads, g_dwThreadMax);

			// 关闭所有线程句柄
			for (size_t i = 0; i < g_dwThreadMax; i++) 
				if (nullptr != g_hThreads[i])
					CloseHandle (g_hThreads[i]);

			// 关闭所有事件句柄
			CloseHandle (g_hExitEvent);
			CloseHandle (g_hOnlineEvent);
			CloseHandle (g_hAutoChatEvent);
			CloseHandle (g_hAutoOnHookEvent);

			// 关闭对话框
			EndDialog (hWnd, id);

			break;

		case IDOK:
			break;
	}
}

INT_PTR WINAPI Dlg_Proc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		// 初始化对话框
		HANDLE_DLGMSG (hWnd, WM_INITDIALOG, Dlg_OnInitDialog);
		// 处理命令消息
		HANDLE_DLGMSG (hWnd, WM_COMMAND, Dlg_OnCommand);
		// 实时更新
		HANDLE_DLGMSG (hWnd, WM_TIMER, Cls_OnTimer);
		// 处理通知消息
		HANDLE_DLGMSG (hWnd, WM_NOTIFY, Dlg_OnNotify);
	}

	return(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain (HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	g_hInstExe = hinstExe;

	// 创建对话框
	DialogBoxParam (hinstExe, MAKEINTRESOURCE (IDD_DLG),
		NULL, Dlg_Proc, NULL);

	return(0);
}
