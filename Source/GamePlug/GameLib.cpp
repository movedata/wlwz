#include "CommandInfo.h"

TCHAR g_szGameTitle[] = TEXT ("Element Client");
TCHAR g_szGameClass[] = TEXT ("QElementClient Window");

const DWORD g_dwThreadMax = 4;
HANDLE g_hThreads[g_dwThreadMax] = {};
HANDLE g_hOnlineEvent = nullptr;
HANDLE g_hExitEvent = nullptr;
HANDLE g_hAutoChatEvent = nullptr;
HANDLE g_hAutoOnHookEvent = nullptr;
HANDLE g_hAutoInfoEvent = nullptr;

bool GetPlayerInfo (std::vector<GAME_DATA_EX>& vectGameData)
{
	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return false;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return false;

	GAME_DATA_EX GameData = {};

	if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x2C }) ||
		!Game.ReadMem (&GameData.HP, sizeof (GameData.HP), 0x280) ||
		!Game.ReadMem (&GameData.MaxHP, sizeof (GameData.MaxHP), 0x2A0) ||
		!Game.ReadMem (&GameData.MP, sizeof (GameData.MaxMP), 0x284) ||
		!Game.ReadMem (&GameData.MaxMP, sizeof (GameData.HP), 0x2A4) ||
		!Game.ReadMem (&GameData.Attack, sizeof (GameData.Attack), 0x2D8) ||
		!Game.ReadMem (&GameData.MaxAttack, sizeof (GameData.MaxAttack), 0x2DC) ||
		!Game.ReadMem (&GameData.Hit, sizeof (GameData.Hit), 0x2E0) ||
		!Game.ReadMem (&GameData.Avoid, sizeof (GameData.Avoid), 0x2E8) ||
		!Game.ReadMem (&GameData.Armor, sizeof (GameData.Armor), 0x2E4) ||
		!Game.ReadMem (&GameData.X, sizeof (GameData.X), 0x3C) ||
		!Game.ReadMem (&GameData.H, sizeof (GameData.H), 0x40) ||
		!Game.ReadMem (&GameData.Y, sizeof (GameData.Y), 0x44) ||
		!Game.ReadMem (&GameData.Level, sizeof (GameData.Level), 0x270))
		return false;

	if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x2C, 0x7E4 }) ||
		!Game.ReadMem (GameData.Name, sizeof (GameData.Name))) 
		return false;
	
	vectGameData.push_back (GameData);

	return true;
}

void TimerDisplayPlayerInfo () {
	
	//GAME_DATA_EX GameData = {};
	//if (!GetPlayerInfo (GameData))
	//	memset (&GameData, 0, sizeof (GAME_DATA_EX));

	//TCHAR szInfo[64] = {};

	//// 名称
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_NAME, WM_SETTEXT, 0, (LPARAM)GameData.Name);

	//// 等级
	//StringCchPrintf (szInfo, 64, TEXT ("%d"), GameData.Level);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_LEVEL, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 血
	//StringCchPrintf (szInfo, 64, TEXT ("%d/%d"), GameData.MaxHP, GameData.HP);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_HP, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 蓝
	//StringCchPrintf (szInfo, 64, TEXT ("%d/%d"), GameData.MaxMP, GameData.MaxMP);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_MP, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 攻击
	//StringCchPrintf (szInfo, 64, TEXT ("%d-%d"), GameData.Attack, GameData.MaxAttack);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_ATTACK, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 命中
	//StringCchPrintf (szInfo, 64, TEXT ("%d"), GameData.Hit);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_HIT, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 防御
	//StringCchPrintf (szInfo, 64, TEXT ("%d"), GameData.Armor);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_ARMOR, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 躲闪
	//StringCchPrintf (szInfo, 64, TEXT ("%d"), GameData.Avoid);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_AVOID, WM_SETTEXT, 0, (LPARAM)szInfo);

	//// 坐标 x y h
	//StringCchPrintf (szInfo, 64, TEXT ("X:%.f"), GameData.X);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_X, WM_SETTEXT, 0, (LPARAM)szInfo);

	//StringCchPrintf (szInfo, 64, TEXT ("Y:%.f"), GameData.Y);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_Y, WM_SETTEXT, 0, (LPARAM)szInfo);

	//StringCchPrintf (szInfo, 64, TEXT ("H:%.f"), GameData.H);
	//SendDlgItemMessage (g_hPages[0],
	//	IDC_STATIC_H, WM_SETTEXT, 0, (LPARAM)szInfo);
}

bool GetNpcInfo (std::vector<GAME_DATA_EX>& vectGameData, bool bShow, bool bSel)
{
	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return false;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return false;
	
	DWORD dwCount = 0;
	DWORD dwOffset = 0;
	if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x10, 0x24 }) ||
			!Game.ReadMem (&dwCount, sizeof (dwCount), 0x5C) ||
				0 == dwCount ||
			!Game.ReadMem (&dwOffset, sizeof (dwOffset), 0x58) ||
				0 == dwOffset)
			return false;

	DWORD dwSelId = 0;
	if (bSel)
		if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x2C }) ||
				!Game.ReadMem (&dwSelId, sizeof (dwSelId), 0x142C) ||
					0 == dwSelId)
				return false;

	for (size_t i = 0; i < dwCount; i++)
	{
		GAME_DATA_EX GameData = {};

		if (!Game.UpdateOffset ({ dwOffset + i * 4 }) ||
			!Game.ReadMem (&GameData.ID, sizeof (GameData.ID), 0x114) ||
			!Game.ReadMem (&GameData.X, sizeof (GameData.X), 0x3C) ||
			!Game.ReadMem (&GameData.Y, sizeof (GameData.Y), 0x44) ||
			!Game.ReadMem (&GameData.Type, sizeof (GameData.Type), 0xB4) ||
			!Game.ReadMem (&GameData.Level, sizeof (GameData.Level), 0x120) ||
			!Game.ReadMem (&GameData.HP, sizeof (GameData.HP), 0x130)) {
			break;
		}

		if (!Game.UpdateOffset ({ dwOffset + i * 4, 0x2C8 }) ||
			!Game.ReadMem (GameData.Name, sizeof (GameData.Name))) {
			break;
		}

		if (0 == GameData.Name[0])
			StringCchCopy (GameData.Name, sizeof (GameData.Name) / sizeof (TCHAR), TEXT ("未知"));

		if (bSel && GameData.ID == dwSelId) {
			vectGameData.push_back (GameData);
			break;
		}
		else 
			if (bSel && GameData.ID != dwSelId)
				continue;

		vectGameData.push_back (GameData);
	}

	if (!vectGameData.empty () && bShow) {
		TCHAR szInfo[256] = {};
		
		if(bSel)
		{
			GAME_DATA_EX e = vectGameData.back ();
			StringCchPrintf (szInfo, 256, TEXT ("选中的Npc\nID:%X  坐标:%0.f,%0.f  HP:%d 等级:%d  名称:%s\n"),
				e.ID, e.X, e.Y, e.HP, e.Level, e.Name);
			
			MsgBox (szInfo);
		}
		else
		{
			_tstring strMonster = TEXT ("");
			_tstring strPerson = TEXT ("");
			DWORD dwMonsterCount = 0;
			DWORD dwPersonCount = 0;

			for (const GAME_DATA_EX e : vectGameData) {
				memset (szInfo, 0, sizeof (szInfo));
				StringCchPrintf (szInfo, 256, TEXT ("ID:%X  坐标:%0.f,%0.f  HP:%d 等级:%d  名称:%s\n"),
					e.ID, e.X, e.Y, e.HP, e.Level, e.Name);
				if (NpcMonster == e.Type) {
					strMonster += szInfo;
					dwMonsterCount++;
				}
				else {
					strPerson += szInfo;
					dwPersonCount++;
				}
			}
			StringCchPrintf (szInfo, 256, TEXT ("附近怪物Npc数量：%d\n"), dwMonsterCount);
			strMonster = szInfo + strMonster;
			MsgBox (strMonster.c_str ());

			StringCchPrintf (szInfo, 256, TEXT ("附近人物Npc数量：%d\n"), dwPersonCount);
			strPerson = szInfo + strPerson;
			MsgBox (strPerson.c_str ());
		}
	}

	return (!vectGameData.empty());
}


bool GetFloorInfo (std::vector<GAME_DATA_EX>& vectGameData, bool bShow)
{
	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return false;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return false;
	
	DWORD dwCount = 0;
	DWORD dwOffset = 0;
	if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x10, 0x28 }) ||
		!Game.ReadMem (&dwCount, sizeof (dwCount), 0x18) ||
			0 == dwCount ||
		!Game.ReadMem (&dwOffset, sizeof (dwOffset), 0x1C) ||
			0 == dwOffset)
		return false;

	for (size_t i = 0; i < dwCount; i++)
	{
		if (!Game.UpdateOffset ({ dwOffset + i * 4, 4 }))
			continue;

		GAME_DATA_EX GameData = {};

		if (!Game.ReadMem (&GameData.X, sizeof (GameData.X), 0x3C) ||
			!Game.ReadMem (&GameData.Y, sizeof (GameData.Y), 0x44) ||
			!Game.ReadMem (&GameData.RandomID, sizeof (GameData.RandomID), 0x104) ||
			!Game.ReadMem (&GameData.ID, sizeof (GameData.ID), 0x108))
			break;

		if (!Game.UpdateOffset ({ dwOffset + i * 4, 4, 0x1B0 }) ||
			!Game.ReadMem (GameData.Name, sizeof (GameData.Name))) 
			break;

		vectGameData.push_back (GameData);
	}

	if (!vectGameData.empty () && bShow) {
		TCHAR szInfo[256] = {};
		StringCchPrintf (szInfo, 256, TEXT ("附近物品数量：%d\n"), vectGameData.size ());
		_tstring str (szInfo);

		for (const GAME_DATA_EX e : vectGameData) {
			memset (szInfo, 0, sizeof (szInfo));
			StringCchPrintf (szInfo, 256, TEXT ("物品坐标：%.0f,%.0f 物品名称：%s \t物品ID:%#X\n随机ID：%#X\n"),
				e.X, e.Y, e.Name, e.ID, e.RandomID);
			str += szInfo;
		}

		MsgBox (str.c_str ());
	}

	return (!vectGameData.empty ());
}

bool GetOtherPlayers (std::vector<GAME_DATA_EX>& vectGameData, bool bShow)
{
	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return false;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return false;
	
	DWORD dwCount = 0;
	DWORD dwOffset = 0;
	if (!Game.UpdateOffset ({ GAME_BASE, 0x1C, 0x10, 0x20 }) ||
		!Game.ReadMem (&dwCount, sizeof (dwCount), 0x14) ||
			0 == dwCount ||
		!Game.ReadMem (&dwOffset, sizeof (dwOffset), 0xAC) ||
			0 == dwOffset)
		return false;

	for (size_t i = 0; i < dwCount; i++)
	{
		GAME_DATA_EX GameData = {};

		if (!Game.UpdateOffset ({ dwOffset + i * 4 }) ||
			!Game.ReadMem (&GameData.X, sizeof (GameData.X), 0x3C) ||
			!Game.ReadMem (&GameData.Y, sizeof (GameData.Y), 0x44) ||
			!Game.ReadMem (&GameData.Level, sizeof (GameData.Level), 0x270)) 
			break;

		if (!Game.UpdateOffset ({ dwOffset + i * 4, 0x7E4 }) ||
			!Game.ReadMem (&GameData.Name, sizeof (GameData.Name))) 
			break;

		vectGameData.push_back (GameData);
	}

	if (!vectGameData.empty () && bShow) {
		TCHAR szInfo[256] = {};
		StringCchPrintf (szInfo, 256, TEXT ("附近玩家数量：%d \n"), dwCount);
		_tstring str (szInfo);
		
		for (const GAME_DATA_EX e : vectGameData) {
			memset (szInfo, 0, sizeof (szInfo));
			StringCchPrintf (szInfo, 256, TEXT ("玩家等级：%d 玩家坐标：%.0f,%.0f 玩家名称：%s\n"),
				e.Level, e.X, e.Y, e.Name);
			str += szInfo;
		}

		MsgBox (str.c_str ());
	}

	return (!vectGameData.empty());
}

bool RemoteCall (PVOID pvContext, DWORD dwSize) {
	bool bOK = false;

	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return bOK;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return bOK;

	if (Game.InjectLib ()) {
		bOK = Game.RemoteCall (pvContext, dwSize);
	}

	return bOK;
}

bool Accelerator (DWORD dwCol, DWORD dwIndex, BOOL bFn)
{
	DWORD array[] = {
		GAME_CMD_ACCELERATOR_KEY,
		dwCol,
		dwIndex,
		(DWORD)bFn
	};

	return RemoteCall (array, sizeof (array));
}

bool Restore ()
{
	const int limit = 1;
	DWORD array[limit] = {
		GAME_CMD_ACTION_REST
	};

	return RemoteCall (array, sizeof (array));
}

bool Run ()
{
	DWORD array[] = {
		GAME_CMD_ACTION_RUN
	};

	return RemoteCall (array, sizeof (array));
}

bool Attack ()
{
	DWORD array[] = {
		GAME_CMD_ACTION_ATTACK
	};

	return RemoteCall (array, sizeof (array));
}

bool Actions (DWORD dwCallIndex)
{
	DWORD array[] = {
		GAME_CMD_ACTION_GENERAL,
		dwCallIndex
	};

	return RemoteCall (array, sizeof (array));
}

bool Face (DWORD dwFaceIndex)
{
	DWORD array[] = {
		GAME_CMD_ACTION_FACE,
		dwFaceIndex
	};

	return RemoteCall (array, sizeof (array));
}

bool SelectBeast ()
{
	DWORD array[] = {
		GAME_CMD_ACCELERATOR_SELECT,
		0x0,
	};

	return RemoteCall (array, sizeof (array));
}

bool DeathReturn ()
{
	DWORD array[] = {
		GAME_CMD_ACCELERATOR_RETURN,
	};

	return RemoteCall (array, sizeof (array));
}

bool Supplement (bool bRed)
{
	DWORD array[] = {
		GAME_CMD_ACCELERATOR_SUPPLEMENT,
		bRed ? static_cast<DWORD>(0x6bd) : static_cast<DWORD>(0x6c8)
	};

	return RemoteCall (array, sizeof (array));
}

bool Skill (DWORD dwSkillID)
{
	DWORD array[] = {
		GAME_CMD_ACCELERATOR_SKILL,
		dwSkillID
	};

	return RemoteCall (array, sizeof (array));
}

bool SendChat (PCTSTR pChatText, DWORD dwChatLevel)
{
	if (WAIT_OBJECT_0 != WaitForSingleObject (g_hOnlineEvent, 0))
		//if (!g_bGameEnable)
		return false;

	CProcessData Game;
	if (!Game.AttachProcess (g_szGameTitle, g_szGameClass))
		return false;
	
	PTSTR pBuffer = nullptr;

	bool bRet = false;
	
	try
	{
		DWORD cb = _tcslen (pChatText) * sizeof (TCHAR);

		pBuffer = (PTSTR)
			VirtualAllocEx (Game.GetHandle(), NULL, cb,
				MEM_COMMIT, PAGE_READWRITE);
		if (pBuffer == NULL) THROW ();

		if (!Game.WriteMem ((PVOID) pChatText, cb, pBuffer))
			THROW ();

		DWORD array[] = {
			GAME_CMD_CHAT,
			(DWORD)pBuffer,
			dwChatLevel,
		};

		bRet = RemoteCall (array, sizeof (array));

	}
	catch (const CException& e) 
	{
		e.Trace ();
	}
	catch (...)
	{

	}

	if (pBuffer != NULL)
		VirtualFreeEx (Game.GetHandle(), pBuffer, 0, MEM_RELEASE);

	return bRet;
}

bool GoToXY (FLOAT x, FLOAT y, BOOL bAuto)
{
	DWORD array[] = {
		GAME_CMD_GOTOXY,
		*((PDWORD)&x),
		*((PDWORD)&y),
		(DWORD)bAuto
	};

	return RemoteCall (array, sizeof (array));
}

bool IsExistProcess (PCTSTR szProcessName)
{
	PROCESSENTRY32 processEntry32;
	HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != toolHelp32Snapshot)
	{
		processEntry32.dwSize = sizeof (processEntry32);
		if (Process32First (toolHelp32Snapshot, &processEntry32))
		{
			do
			{
				if (_tcscmp (szProcessName, processEntry32.szExeFile) == 0)
					return true;
			} while (Process32Next (toolHelp32Snapshot, &processEntry32));
		}
		CloseHandle (toolHelp32Snapshot);
	}

	return false;
}

void ProcessControl (HWND hWnd)
{

	CProcessData Game;

	TCHAR szText[32] = {};

	if (Game.AttachProcess (g_szGameTitle, g_szGameClass)) 
		SendDlgItemMessage (hWnd, IDC_STATIC_TEXT, WM_SETTEXT, 0,
			(LPARAM)TEXT ("获取游戏控制权限成功!"));
	else
		SendDlgItemMessage (hWnd, IDC_STATIC_TEXT, WM_SETTEXT, 0,
			(LPARAM)TEXT ("获取游戏控制权限失败!"));

	// 显示窗口句柄
	StringCchPrintf (szText, sizeof (szText) / sizeof (TCHAR),
		TEXT ("(%#08x) ~ %d"), (DWORD)Game.GetHWnd (), (DWORD)Game.GetHWnd ());
	SendDlgItemMessage (hWnd, IDC_STATIC_HWND, WM_SETTEXT, 0, (LPARAM)szText);

	// 显示进程ID
	StringCchPrintf (szText, sizeof (szText) / sizeof (TCHAR),
		TEXT ("(%#08x) ~ %d"), Game.GetProcessId (), Game.GetProcessId ());
	SendDlgItemMessage (hWnd, IDC_STATIC_PROCESS_ID, WM_SETTEXT, 0, (LPARAM)szText);

	// 显示线程ID
	StringCchPrintf (szText, sizeof (szText) / sizeof (TCHAR),
		TEXT ("(%#08x) ~ %d"), Game.GetThreadId (), Game.GetThreadId ());
	SendDlgItemMessage (hWnd, IDC_STATIC_THREAD_ID, WM_SETTEXT, 0, (LPARAM)szText);

}

bool IsOnline ()
{
	CProcessData Online;
	DWORD ret = 0;
	if (Online.AttachProcess (g_szGameTitle, g_szGameClass) &&
			Online.UpdateOffset ({ GAME_BASE, 0x1C, 0x2C}) &&
				Online.ReadMem (&ret, sizeof (ret)))
					return (bool)(!!ret);

	return false;
}

void WaitForThreadQuit (HANDLE hTheads[], int threadCount)
{
	MSG msg;

	while (threadCount > 0)
	{
		DWORD dRet = MsgWaitForMultipleObjects (threadCount, hTheads, FALSE, INFINITE, QS_ALLINPUT);

		if (dRet == (WAIT_OBJECT_0 + threadCount))
		{
			while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}
		else if ((dRet >= WAIT_OBJECT_0) && (dRet<WAIT_OBJECT_0 + threadCount))
		{

			int quitIndex = dRet - WAIT_OBJECT_0;

			HANDLE hThread = hTheads[quitIndex];
			hTheads[quitIndex] = hTheads[--threadCount];
			hTheads[threadCount] = hThread;
		}
	}
}

DWORD __stdcall AutoTestThread (HWND hWnd)
{
	bool bControl = false;
	bool bEvent = false;
	DWORD interval = 1000;

	while (WAIT_OBJECT_0 != WaitForSingleObject (g_hExitEvent, interval)) {
		if (IsExistProcess (TEXT ("elementclient.exe"))) {
			// 游戏进程已存在
			if (IsOnline ()) {
				// 游戏人物在线
				if (!bEvent) {
					bEvent = true;
					SetEvent (g_hOnlineEvent);
				}
				interval = 5000;
			}
			else {
				// 游戏人物离线
				if (bEvent) {
					bEvent = false;
					ResetEvent (g_hOnlineEvent);
				}
				interval = 1000;
			}

			if (!bControl) {
				Sleep (1000);
				bControl = true;
				ProcessControl (hWnd);
			}
		}
		else {
			// 游戏进程不存在
			if (bControl) {
				bControl = false;
				ProcessControl (hWnd);
			}

			if (bEvent) {
				bEvent = false;
				ResetEvent (g_hOnlineEvent);
			}
			interval = 1000;
		}
	}

	// 检测线程函数退出前所有事件必须有信号
	SetEvent (g_hOnlineEvent);
	SetEvent (g_hAutoChatEvent);
	SetEvent (g_hAutoOnHookEvent);
	SetEvent (g_hAutoInfoEvent);

	OutputDebugString (L"线程1退出\n");
	return 0;
}

DWORD __stdcall AutoInfoThread (HWND hWnd)
{
	while (true)
	{
		// 等待在线信号
		WaitForSingleObject (g_hOnlineEvent, INFINITE);

		// 等待自动信息信号
		WaitForSingleObject (g_hAutoInfoEvent, INFINITE);

		// 判断退出信号
		if (WAIT_OBJECT_0 == WaitForSingleObject (g_hExitEvent, 1000))
			break;

		std::vector<GAME_DATA_EX> vect;
		if (!GetPlayerInfo (vect))
			vect.push_back (GAME_DATA_EX ());

		TCHAR szInfo[64] = {};

		// 名称
		SendDlgItemMessage (hWnd,
			IDC_STATIC_NAME, WM_SETTEXT, 0, (LPARAM)vect.back().Name);

		// 等级
		StringCchPrintf (szInfo, 64, TEXT ("%d"), vect.back().Level);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_LEVEL, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 血
		StringCchPrintf (szInfo, 64, TEXT ("%d/%d"), vect.back().MaxHP, vect.back().HP);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_HP, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 蓝
		StringCchPrintf (szInfo, 64, TEXT ("%d/%d"), vect.back().MaxMP, vect.back().MaxMP);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_MP, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 攻击
		StringCchPrintf (szInfo, 64, TEXT ("%d-%d"), vect.back().Attack, vect.back().MaxAttack);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_ATTACK, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 命中
		StringCchPrintf (szInfo, 64, TEXT ("%d"), vect.back().Hit);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_HIT, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 防御
		StringCchPrintf (szInfo, 64, TEXT ("%d"), vect.back().Armor);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_ARMOR, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 躲闪
		StringCchPrintf (szInfo, 64, TEXT ("%d"), vect.back().Avoid);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_AVOID, WM_SETTEXT, 0, (LPARAM)szInfo);

		// 坐标 x y h
		StringCchPrintf (szInfo, 64, TEXT ("X:%.f"), vect.back().X);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_X, WM_SETTEXT, 0, (LPARAM)szInfo);

		StringCchPrintf (szInfo, 64, TEXT ("Y:%.f"), vect.back().Y);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_Y, WM_SETTEXT, 0, (LPARAM)szInfo);

		StringCchPrintf (szInfo, 64, TEXT ("H:%.f"), vect.back().H);
		SendDlgItemMessage (hWnd,
			IDC_STATIC_H, WM_SETTEXT, 0, (LPARAM)szInfo);
	}

	OutputDebugString (L"线程2退出\n");

	return 0;
}

DWORD __stdcall AutoChatThread (HWND hWnd)
{
	while (true)
	{
		// 等待在线信号
		WaitForSingleObject (g_hOnlineEvent, INFINITE);
		
		// 等待自动聊天信号
		WaitForSingleObject (g_hAutoChatEvent, INFINITE);
		
		// 判断退出信号
		if (WAIT_OBJECT_0 == WaitForSingleObject (g_hExitEvent, 0))
			break;
		
		// 处理自动聊天
		TCHAR szChatText[80]{};
		if (0 != SendDlgItemMessage (hWnd, 
			IDC_INTERVAL_TIME, WM_GETTEXT, 80, (LPARAM)szChatText)) {
				long interval = 0;
				interval = _tcstol (szChatText, nullptr, 10) * 1000;
				if (0 != SendDlgItemMessage (hWnd, 
					IDC_CHATCONTENT, WM_GETTEXT, 80, (LPARAM)szChatText))
						SendChat (szChatText, 0);
			
				Sleep (interval);
		}
	}

	OutputDebugString (L"线程3退出\n");

	return 0;
}

DWORD __stdcall AutoOnHookThread (HWND hWnd)
{
	std::vector<GAME_DATA_EX> GameDatas;
	
	while (true)
	{
		// 等待在线信号
		WaitForSingleObject (g_hOnlineEvent, INFINITE);

		// 等待自动挂机信号
		WaitForSingleObject (g_hAutoOnHookEvent, INFINITE);

		// 判断退出信号
		if (WAIT_OBJECT_0 == WaitForSingleObject (g_hExitEvent, 0))
			break;

		// 连续选怪和攻击
		GameDatas.clear ();
		if (GetNpcInfo (GameDatas, false, true)) {
			if (GameDatas.back ().HP > 0)
				Attack ();		// 普通攻击
			Sleep (100);
		}
		else {
			Actions (5);		// 拾取
			Sleep (100);
			
			// 处理恢复HP
			GameDatas.clear ();
			if (GetPlayerInfo (GameDatas)) {
				TCHAR szNum[5] = {};
				if (0 != SendDlgItemMessage (hWnd, IDC_HP_MIN, WM_GETTEXT, 5, (LPARAM)szNum)) {
					DWORD minHp = 0;
					minHp = _tcstol (szNum, nullptr, 10);
					if (0 != minHp && GameDatas.back ().HP < minHp) {
						UINT uCheckStatus = 0;
						uCheckStatus = Button_GetCheck (GetDlgItem (hWnd, IDC_HP_RESTORE));
						if (BST_CHECKED == uCheckStatus) {
							Restore ();
							while (GameDatas.back ().HP < GameDatas.back ().MaxHP)
							{
								Sleep (1000);
								GameDatas.clear ();
								if (!GetPlayerInfo (GameDatas))
									break;
							}
						}
						else
							Supplement (true);
					}	
				}	
			}

			// 选怪
			SelectBeast ();
			Sleep (100);
		}

		// 死亡回城
		GameDatas.clear ();
		if (GetPlayerInfo (GameDatas) && 0 == GameDatas.back ().HP)
			DeathReturn ();

	}
	
	OutputDebugString (L"线程4退出\n");
	
	return 0;
}

