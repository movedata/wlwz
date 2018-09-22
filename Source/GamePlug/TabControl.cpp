#include "CommandInfo.h"

const DWORD g_dwPageMax = 6;
HWND g_hPages[g_dwPageMax];

INT_PTR WINAPI Dlg_Child (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	bool bRet = false;

	switch (uMsg) {
		case WM_COMMAND: {
			switch (LOWORD (wParam))
			{
				/////////////////////////////////////////////////////////
				// 快捷键处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACCELERATOR_F1: case IDC_ACCELERATOR_F2: 
				case IDC_ACCELERATOR_F3: case IDC_ACCELERATOR_F4:
				case IDC_ACCELERATOR_F5: case IDC_ACCELERATOR_F6: 
				case IDC_ACCELERATOR_F7: case IDC_ACCELERATOR_F8:
				case IDC_ACCELERATOR_1: case IDC_ACCELERATOR_2: 
				case IDC_ACCELERATOR_3: case IDC_ACCELERATOR_4: 
				case IDC_ACCELERATOR_5:	case IDC_ACCELERATOR_6: 
				case IDC_ACCELERATOR_7: case IDC_ACCELERATOR_8: 
				case IDC_ACCELERATOR_9: case IDC_ACCELERATOR_0:
				{
					BOOL bFn = LOWORD (wParam) <= IDC_ACCELERATOR_F8;
					DWORD dwIndex = bFn ? (LOWORD (wParam) - IDC_ACCELERATOR_F1)
						: (LOWORD (wParam) - IDC_ACCELERATOR_1);
					DWORD dwCol = bFn ? (SendDlgItemMessage (g_hPages[1], IDC_ACCELERATOR_FN, CB_GETCURSEL, 0, 0) + 1)
						: (SendDlgItemMessage (g_hPages[1], IDC_ACCELERATOR_N, CB_GETCURSEL, 0, 0) + 1);

					if (!(bRet = Accelerator (dwCol, dwIndex, bFn)))
						MsgBox (TEXT ("快捷键没有执行"));

					break;
				} // IDC_F1 ~ IDC_F8
				/////////////////////////////////////////////////////////
				// 选怪处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACCELERATOR_SELECT_BEAST: {
					if (!(bRet = SelectBeast ()))
						MsgBox (TEXT ("选怪没有执行"));

					break;
				} // case IDC_SELECT_BEAST
				/////////////////////////////////////////////////////////
				// 死亡回城处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACCELERATOR_DEATH_RETURN: {
					if (!(bRet = DeathReturn ()))
						MsgBox (TEXT ("死亡回城没有执行"));

					break;
				} // case IDC_ACCELERATOR_DEATH_RETURN
				/////////////////////////////////////////////////////////
				// 自动补药处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACCELERATOR_SUPPLEMENT_RED:
				case IDC_ACCELERATOR_SUPPLEMENT_BLUE: {
					MsgBox (TEXT ("此功能为自动搜索包裹\r\n需包裹内容易格子放置[金疮药]和[提神霜"));
					if (!(bRet = Supplement ((LOWORD (wParam) == IDC_ACCELERATOR_SUPPLEMENT_RED))))
						MsgBox (TEXT ("没有药物"));

					break;
				} // case IDC_ACCELERATOR_SUPPLEMENT_RED
				/////////////////////////////////////////////////////////
				// 技能处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACCELERATOR_SKILL_QFP:
				case IDC_ACCELERATOR_SKILL_ZLJ:
				case IDC_ACCELERATOR_SKILL_BACKCITY: {
					DWORD dwSkillIds[] = {
						2,3,78
					};
					if (!(bRet = Skill (dwSkillIds[LOWORD(wParam)-
						IDC_ACCELERATOR_SKILL_QFP])))
						MsgBox (TEXT ("技能没有执行"));
					break;
				} // IDC_ACCELERATOR_SKILL_XX

				/////////////////////////////////////////////////////////
				// 聊天处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_CHATSEND: {
					const DWORD dwTextSize = 80;
					TCHAR szChatText[dwTextSize]{};
					DWORD dwChatLevel = SendDlgItemMessage (g_hPages[2], IDC_CHATLEVEL, CB_GETCURSEL, 0, 0);
					if (0 != dwChatLevel) 
						StringCchCopy (szChatText, dwTextSize, TEXT ("!!"));

					if (0 != SendDlgItemMessage (g_hPages[2],
						IDC_CHATCONTENT, WM_GETTEXT, dwTextSize - _tcslen (szChatText),
						(LPARAM)(szChatText + _tcslen (szChatText)))) {
						if (!(bRet = SendChat (szChatText, dwChatLevel)))
							MsgBox (TEXT ("聊天没有执行"));
					}


					break;
				} // case IDC_CHATSEND
				case IDC_AUTO_CHAT: {
					UINT uCheckStatus = 0;
					uCheckStatus = Button_GetCheck (GetDlgItem (hwnd, IDC_AUTO_CHAT));
					
					if (BST_CHECKED == uCheckStatus) {
						const DWORD dwTextSize = 80;
						TCHAR szChatText[dwTextSize]{};
						if (0 == SendDlgItemMessage (hwnd, IDC_CHATCONTENT, WM_GETTEXT, dwTextSize, (LPARAM)szChatText) ||
							0 == SendDlgItemMessage (hwnd, IDC_INTERVAL_TIME, WM_GETTEXT, dwTextSize, (LPARAM)szChatText)) {
							MsgBox (TEXT ("需要填写聊天内容和间隔时间!"));
							CheckDlgButton (hwnd, IDC_AUTO_CHAT, BST_UNCHECKED);
							break;
						}

						EnableWindow (GetDlgItem (hwnd, IDC_CHATCONTENT), false);
						EnableWindow (GetDlgItem (hwnd, IDC_INTERVAL_TIME), false);
						SetEvent (g_hAutoChatEvent);
					}
					else {
						EnableWindow (GetDlgItem (hwnd, IDC_CHATCONTENT), true);
						EnableWindow (GetDlgItem (hwnd, IDC_INTERVAL_TIME), true);
						ResetEvent (g_hAutoChatEvent);
					}

					break;
				} // case IDC_AUTO_CHAT

				/////////////////////////////////////////////////////////
				// 动作处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_ACTION_ATTACK: {		//普攻
					if (!(bRet = Attack ())) {
						MsgBox (TEXT ("普攻没有执行"));
					}
					break;
				} //case IDC_ACTION_ATTACK
				case IDC_ACTION_RUN: {			//走跑
					if (!(bRet = Run ())) {
						MsgBox (TEXT ("走跑没有执行"));
					}
					break;
				} //case IDC_ACTION_RUN
				case IDC_ACTION_REST: {			//打坐
					if (!(bRet = Restore ())) {
						MsgBox (TEXT ("打坐没有执行"));
					}
					break;
				} //case IDC_ACTION_REST
				case IDC_ACTION_ASSISTS: 		//助攻
				case IDC_ACTION_TEAM: 			//组队
				case IDC_ACTION_LEAVE:			//离队
				case IDC_ACTION_DEAL: 			//交易
				case IDC_ACTION_STALL:			//摆摊
				case IDC_ACTION_PICK: {			//拾取
					if (!(bRet = Actions (LOWORD (wParam) - IDC_ACTION_ASSISTS))) {
						MsgBox (TEXT ("动作没有执行"));
					}
					break;
				} //case IDC_ACTION_ASSISTS

				case IDC_ACTION_FACE_00:
				case IDC_ACTION_FACE_01:
				case IDC_ACTION_FACE_02:
				case IDC_ACTION_FACE_03:
				case IDC_ACTION_FACE_04:
				case IDC_ACTION_FACE_05:
				case IDC_ACTION_FACE_06:
				case IDC_ACTION_FACE_07:
				case IDC_ACTION_FACE_08:
				case IDC_ACTION_FACE_09:
				case IDC_ACTION_FACE_10: {		//表情	
					if (!(bRet = Face (LOWORD (wParam) - IDC_ACTION_FACE_00))) {
						MsgBox (TEXT ("表情没有执行"));
					}
					break;
				} //case IDC_ACTION_RUN

				case IDC_MOVEAUTOGOTO:
				case IDC_MOVEGOTO: {			//移动
					const DWORD dwTextSize = 10;
					TCHAR szBuffer[dwTextSize]{};
					float x, y;
					bool bOK = false;

					if (0 != SendDlgItemMessage (g_hPages[4],
						IDC_MOVEX, WM_GETTEXT, dwTextSize, (LPARAM)szBuffer)) {
						x = static_cast<float>(_ttof (szBuffer));
						bOK = true;
					}
					else {
						MsgBox (TEXT ("没有填写X坐标"));
						
					}
					
					if (bOK && 0 != SendDlgItemMessage (g_hPages[4],
						IDC_MOVEY, WM_GETTEXT, dwTextSize, (LPARAM)szBuffer)) {
						y = static_cast<float>(_ttof (szBuffer));
					}
					else {
						MsgBox (TEXT ("没有填写Y坐标"));
						bOK = false;
					}

					if (bOK) {
						if (!(bRet = GoToXY (x, y, (LOWORD (wParam) == IDC_MOVEAUTOGOTO)))) {
							MsgBox (TEXT ("移动没有执行"));
						}
					}

					break;
				} // case IDC_MOVEGOTO
				
				/////////////////////////////////////////////////////////
				// 自动挂机处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_HOOKSTART: {
					const DWORD dwTextSize = 4;
					TCHAR szMinHp[dwTextSize] = {};
					if (0 == SendDlgItemMessage (hwnd, IDC_HP_MIN, WM_GETTEXT, dwTextSize, (LPARAM)szMinHp)) {
						MsgBox (TEXT ("最小血量必须填写且小于等于3个字符"));
						break;
					}

					std::vector<GAME_DATA_EX> vect;
					if (!GetPlayerInfo (vect)) {
						MsgBox (TEXT ("玩家信息获取失败!"));
						break;
					}

					DWORD dwMinHp = 0;
					dwMinHp = _tcstol (szMinHp, nullptr, 10);
					if (dwMinHp < 20 || dwMinHp > vect.back ().MaxHP) {
						MsgBox (TEXT ("最小血量必须大于20且小于等于最大血量"));
						break;
					}

					SetEvent (g_hAutoOnHookEvent);
					EnableWindow (GetDlgItem (hwnd, IDC_HOOKSTOP), true);
					EnableWindow (GetDlgItem (hwnd, IDC_HOOKSTART), false);

					break;
				} // case IDC_HOOKSTART

				case IDC_HOOKSTOP: {
					ResetEvent (g_hAutoOnHookEvent);
					EnableWindow (GetDlgItem (hwnd, IDC_HOOKSTOP), false);
					EnableWindow (GetDlgItem (hwnd, IDC_HOOKSTART), true);
					break;
				} // IDC_HOOKSTOP

				/////////////////////////////////////////////////////////
				// 查询信息处理代码开始
				/////////////////////////////////////////////////////////
				case IDC_NPCINFO: {	// Npc信息
					std::vector<GAME_DATA_EX> vect;
					if (!(bRet = GetNpcInfo (vect, true))) {
						MsgBox (TEXT ("显示Npc无或没有执行"));
					}

					break;
				} // case IDC_NPCINFO:

				case IDC_FLOORINFO: {	// 地面信息
					std::vector<GAME_DATA_EX> vect;
					if (!(bRet = GetFloorInfo (vect, true))) {
						MsgBox (TEXT ("显示地面物品无或没有执行"));
					}

					break;
				} // case IDC_FLOORINFO

				case IDC_OTHERPLAYER: {	// 其他玩家
					std::vector<GAME_DATA_EX> vect;
					if (!(bRet = GetOtherPlayers (vect, true))) {
						MsgBox (TEXT ("显示其他玩家无或没有执行"));
					}

					break;
				} // case IDC_OTHERPLAYER

			} // switch (LOWORD (wParam))
		} // case WM_COMMAND
	} // switch (uMsg)

	return(bRet);
}

BOOL Dlg_OnNotify (HWND hwnd, INT id, LPNMHDR pnm)
{
	int        i;

	BOOL bRet = FALSE;

	if (pnm->code == TCN_SELCHANGE)
	{
		i = SendDlgItemMessage (hwnd, IDC_GAME_INFO, TCM_GETCURSEL, 0, 0);

		for (int ct = 0; ct < g_dwPageMax; ct++)
		{
			ShowWindow (g_hPages[ct], FALSE);	//假定不显示
			if (ct == i) {
				ShowWindow (g_hPages[i], TRUE); //显示Page i页
				bRet = TRUE;
			}
		}

		if (2 == i) {
			SendDlgItemMessage (g_hPages[i],
				IDC_CHATLEVEL, CB_RESETCONTENT, 0, 0); 
			SendDlgItemMessage (g_hPages[i],
				IDC_CHATLEVEL, CB_ADDSTRING, 0, (LPARAM)TEXT ("普聊"));
			SendDlgItemMessage (g_hPages[i],
				IDC_CHATLEVEL, CB_ADDSTRING, 0, (LPARAM)TEXT ("队聊"));
			SendDlgItemMessage (g_hPages[i],
				IDC_CHATLEVEL, CB_SETCURSEL, 0, 0);
		}
		
		if (1 == i) {
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_FN, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_FN, CB_ADDSTRING, 0, (LPARAM)TEXT ("功能键一栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_FN, CB_ADDSTRING, 0, (LPARAM)TEXT ("功能键二栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_FN, CB_ADDSTRING, 0, (LPARAM)TEXT ("功能键三栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_FN, CB_SETCURSEL, 0, 0);
			
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_N, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_N, CB_ADDSTRING, 0, (LPARAM)TEXT ("数字键一栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_N, CB_ADDSTRING, 0, (LPARAM)TEXT ("数字键二栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_N, CB_ADDSTRING, 0, (LPARAM)TEXT ("数字键三栏"));
			SendDlgItemMessage (g_hPages[i],
				IDC_ACCELERATOR_N, CB_SETCURSEL, 0, 0);
		}

		if (0 == i) SetEvent (g_hAutoInfoEvent);
		else ResetEvent (g_hAutoInfoEvent);
	}

	return bRet;
}