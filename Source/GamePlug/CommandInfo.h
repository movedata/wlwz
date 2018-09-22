#pragma once

#include "../common.h"

#include <malloc.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <windowsx.h>
#include <strsafe.h>
#include <stdlib.h>

#include <initializer_list>
#include <vector>
#include <string>

#include "resource.h"
#include "GamePlug.h"
#include "GameLib.h"
#include "TabControl.h"
#include "CommandInfo.h"
#include "GameClass.h"

// 命令定义
#define GAME_CMD_ACTION_GENERAL				0
#define GAME_CMD_ACTION_REST				1
#define GAME_CMD_ACTION_RUN					2
#define GAME_CMD_ACTION_ATTACK				3
#define GAME_CMD_ACTION_FACE				4
#define GAME_CMD_ACCELERATOR_KEY			5
#define GAME_CMD_ACCELERATOR_SELECT			6
#define GAME_CMD_ACCELERATOR_RETURN			7
#define GAME_CMD_ACCELERATOR_SUPPLEMENT		8
#define GAME_CMD_ACCELERATOR_SKILL			9
#define GAME_CMD_CHAT						10
#define GAME_CMD_GOTOXY						11

#define GAME_BASE					0x00DE4660		//基址

const DWORD GAME_CALL_REST = 0x0046D090;			//打坐
const DWORD GAME_CALL_RUN = 0x0046D0C0;				//走跑
const DWORD GAME_CALL_ATTACK = 0x0046D260;			//普攻
const DWORD GAME_CALL_ACTIONS[] = {					//无参数动作调用集合
	0x0046D490,	//助攻
	0x0046DEA0,	//组队
	0x0046E450,	//离队
	0x0046E570,	//交易
	0x0046E5D0,	//摆摊
	0x0046D4D0	//拾取
};
const DWORD GAME_CALL_FACE = 0x0046E600;					//表情

const DWORD GAME_CALL_SELECT = 0x0047D760;					//tab选怪
const DWORD GAME_CALL_RETURN = 0x0074CDD0;					//死亡回城
const DWORD GAME_CALL_SUPPLEMENT = 0x0074CAC0;				//补药
const DWORD GAME_CALL_SKILL = 0x00488C80;					//技能
const DWORD GAME_CALL_CHAT = 0x007544F0;					//聊天

const DWORD GAME_CALL_GOTO[] = {						//移动相关调用集合
	0x00498380,		//分配参数
	0x004988B0, 	//移动
	0x00433C50		//自动寻路移动
};


inline void __stdcall MsgBox (PCTSTR pszMsg) {
	MessageBox (nullptr, pszMsg, TEXT ("提示"), 0);
}

inline void __stdcall NumMsgBox (DWORD dwMsg) {
	TCHAR szInfo[128]{};
	int ccb = sizeof (szInfo) / sizeof (TCHAR);
	StringCchPrintf (szInfo, ccb, TEXT ("%d( %X )"), dwMsg,dwMsg);
	MessageBox (nullptr, szInfo, TEXT ("提示"), 0);
}

inline void __stdcall DbgTrace (PCTSTR sz) {
	OutputDebugString (sz);
}

inline void __stdcall DbgTrace (DWORD dw) {
	TCHAR sz[128] = {};
	StringCchPrintf (sz, 128, TEXT ("%d(%#X)\n"), dw, dw);
	DbgTrace (sz);
}

// 全局变量

extern HWND g_hPages[];					// TabControl.cpp
extern const DWORD g_dwPageMax;			// TabControl.cpp

extern HINSTANCE g_hInstExe;			// GamePlug.cpp
extern UINT_PTR	g_uiTimer;				// GamePlug.cpp
extern bool g_bAutoTestExit;			// GamePlug.cpp

extern TCHAR g_szGameTitle[];			// GameLib.cpp
extern TCHAR g_szGameClass[];			// GameLib.cpp

extern const DWORD g_dwThreadMax;		// GameLib.cpp
extern HANDLE g_hThreads[];				// GameLib.cpp
extern HANDLE g_hOnlineEvent;			// GameLib.cpp
extern HANDLE g_hExitEvent;				// GameLib.cpp
extern HANDLE g_hAutoChatEvent;			// GameLib.cpp
extern HANDLE g_hAutoOnHookEvent;		// GameLib.cpp
extern HANDLE g_hAutoInfoEvent;			// GameLib.cpp