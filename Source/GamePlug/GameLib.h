#pragma once

// 游戏扩展数据
struct GAME_DATA_EX
{
	DWORD ID;
	DWORD RandomID;
	TCHAR Name[64];
	DWORD HP;
	DWORD MaxHP;
	DWORD MP;
	DWORD MaxMP;
	DWORD Attack;
	DWORD MaxAttack;
	DWORD Hit;
	DWORD Avoid;
	DWORD Armor;
	float X;
	float H;
	float Y;
	DWORD Level;
	DWORD Type;
};

// Npc信息子类型
enum NpcType
{
	NpcMonster = 6,
	NpcPerson = 7
};

/*
功能：获取游戏人物某项信息
参数：
	GameData	返回信息, 由调用方提供空间
返回：成功执行为true ，否则为false
*/
bool GetPlayerInfo (std::vector<GAME_DATA_EX>& vectGameData);

/*
功能：计时器显示玩家信息
参数：无
返回：无
*/
void TimerDisplayPlayerInfo ();

/*
功能：获取Npc信息
参数：
	vectGameData	返回信息的数组, 由调用方提供空间
	bShow			true = 直接显示信息 ; false = 不显示信息
	bSel			true = 选择的Npc信息 ; false = 非选择Npc信息
返回：成功执行为true ，否则为false
*/
bool GetNpcInfo (std::vector<GAME_DATA_EX>& vectGameData, bool bShow, bool bSel = false);

/*
功能：获取地面物品信息
参数：
	vectGameData	返回信息的数组, 由调用方提供空间
	bShow			true = 直接显示信息 ; false = 不显示信息
返回：成功执行为true ，否则为false
*/
bool GetFloorInfo (std::vector<GAME_DATA_EX>& vectGameData, bool bShow);

/*
功能：获取其他玩家信息
参数：
	vectGameData	返回信息的数组, 由调用方提供空间
	bShow			true = 直接显示信息 ; false = 不显示信息
返回：成功执行为true ，否则为false
*/
bool GetOtherPlayers (std::vector<GAME_DATA_EX>& vectGameData, bool bShow);

/*
功能：调用远程过程
参数：
	pvContext	过程所需参数数组指针
	dwSize		参数数组的字节数
返回：成功执行为true ，否则为false
*/
bool RemoteCall (PVOID pvContext, DWORD dwSize);

/*
功能：调用按键过程
参数：
	dwCol		数字键和功能键的按键列
	dwIndex		数字键和功能键的编号
	bFn			指定 true = 功能键 ; false = 数字键
返回：成功执行为true ，否则为false
*/
bool Accelerator (DWORD dwCol, DWORD dwIndex, BOOL bFn);

/*
功能：调用打坐过程
参数：无
返回：成功执行为true ，否则为false
*/
bool Restore ();

/*
功能：调用走跑过程
参数：无
返回：成功执行为true ，否则为false
*/
bool Run ();

/*
功能：调用普攻过程
参数：无
返回：成功执行为true ，否则为false
*/
bool Attack ();

/*
功能：调用组队 ; 离队 ; 摆摊等过程
参数：
	dwCallIndex		过程数组索引
返回：成功执行为true ，否则为false
*/
bool Actions (DWORD dwCallIndex);

/*
功能：调用表情动作过程
参数：
	dwFaceIndex	表情数组索引
返回：成功执行为true ，否则为false
*/
bool Face (DWORD dwFaceIndex);

/*
功能：调用调用tab选择怪物npc过程
参数：无
返回：成功执行为true ，否则为false
*/
bool SelectBeast ();

/*
功能：调用死亡回城过程
参数：无
返回：成功执行为true ，否则为false
*/
bool DeathReturn ();

/*
功能：调用自动补药过程
参数：
	bRed		true = 红药 ; false = 蓝药
返回：成功执行为true ，否则为false
*/
bool Supplement (bool bRed);

/*
功能：调用技能过程
参数：
	dwSkillID	技能编号
返回：成功执行为true ，否则为false
*/
bool Skill (DWORD dwSkillID);

/*
功能：调用技能过程
参数：
	pChatText	聊天的文字内容远程指针
	dwChatLevel	聊天频道
返回：成功执行为true ，否则为false
*/
bool SendChat (PCTSTR pChatText, DWORD dwChatLevel);

/*
功能：调用行走过程
参数：
	x			目标坐标x
	y			目标坐标y
	bAuto		true = 自动寻路 ; false = 普通移动
返回：成功执行为true ，否则为false
*/
bool GoToXY (FLOAT x, FLOAT y, BOOL bAuto);

/*
功能：游戏进程是否存在
参数：
	szProcessName	游戏进程名
返回：进程存在为true , 进程不存在为false
*/
bool IsExistProcess (PCTSTR szProcessName);

/*
功能：游戏进程是否被控制并显示控制信息
参数：
	hWnd	传入显示信息窗口的HWND
返回：无
*/
void ProcessControl (HWND hWnd);

/*
功能：游戏人物是否在线
参数：无
返回：在线为true , 离线为false
*/
bool IsOnline ();


/*
功能：线程安全退出
参数：
	hTheads		需要关闭的线程句柄指针
	threadCount 句柄指针的个数
返回：无
*/void WaitForThreadQuit (HANDLE hTheads[], int threadCount);

/*
功能：检查游戏进程及游戏人物是否登录的线程函数
参数：
	hWnd	传入显示信息窗口的HWND
返回：成功执行为true ，其他未定义
*/
DWORD __stdcall AutoTestThread (HWND hWnd);

/*
功能：自动显示玩家状态信息的线程函数
参数：
	hWnd	传入显示信息窗口的HWND
返回：成功执行为true ，其他未定义
*/
DWORD __stdcall AutoInfoThread (HWND hWnd);

/*
功能：自动聊天的线程函数
参数：
	hWnd	传入获取信息窗口的HWND
返回：成功执行为true ，其他未定义
*/
DWORD __stdcall AutoChatThread (HWND hWnd);

/*
功能：自动挂机的线程函数
参数：
	hWnd	传入显示信息窗口的HWND
返回：成功执行为true ，其他未定义
*/
DWORD __stdcall AutoOnHookThread (HWND hWnd);