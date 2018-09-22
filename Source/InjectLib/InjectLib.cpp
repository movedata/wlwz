// InjectLib.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InjectLib.h"

//#pragma data_seg("Shared")
//HWND g_hWnd = 0;
//#pragma data_seg()
//#pragma comment (linker, "/SECTION:Shared,RWS")

DWORD __stdcall GameCommand (PVOID pv)
{
	PDWORD pContext = reinterpret_cast<PDWORD>(pv);
	DWORD dwCmd = pContext[0];

	DWORD dwBaseL1L2 = 0;
	__asm {
		mov	esi, GAME_BASE
		mov esi, [esi]
		mov	esi, [esi + 0x1c]
		mov esi, [esi + 0x2c]
		mov [dwBaseL1L2], esi
	}

	switch (dwCmd)
	{
		case GAME_CMD_ACTION_REST: {		//打坐
			__asm {
				mov esi, [dwBaseL1L2]
				mov eax, [esi + 0x858]
				mov ecx, esi
				shr eax, 5
				not al
				and al, 0x1
				push eax
				call [GAME_CALL_REST]
			}
			break;
		} // case GAME_CMD_ACTION_REST
		case GAME_CMD_ACTION_RUN: {			//走跑
			__asm {
				mov esi, [dwBaseL1L2]
				mov cl, [esi + 0x6e8]
				test cl, cl
				sete cl
				push ecx
				mov ecx, esi
				call [GAME_CALL_RUN]
			}
			break;
		} // case GAME_CMD_ACTION_RUN
		case GAME_CMD_ACTION_ATTACK: {			//普攻
			__asm {
				push -1
				push 0
				push 0
				push 0
				mov ecx, [dwBaseL1L2]
				call[GAME_CALL_ATTACK]
			}
			break;
		} // case GAME_CMD_ACTION_ATTACK
		case GAME_CMD_ACTION_GENERAL: {			//动作一般性调用
			DWORD dwCallIndex = pContext[1];

			__asm {
				mov eax, [dwCallIndex]
				mov ecx, [dwBaseL1L2]
				call [GAME_CALL_ACTIONS + eax * 4]
			}
			break;
		} // case GAME_CMD_ACTION_GENERAL
		case GAME_CMD_ACTION_FACE: {			//表情
			DWORD dwFaceIndex = pContext[1];

			__asm {
				mov ecx, [dwBaseL1L2]
				push [dwFaceIndex]
				call[GAME_CALL_FACE]
			}
			break;
		} // case GAME_CMD_ACTION_FACE
		case GAME_CMD_ACCELERATOR_KEY: {	//快捷键
			DWORD dwCol=pContext[1];
			DWORD dwIndex = 4 * pContext[2];
			BOOL bFn = pContext[3];
			DWORD dwKeyOffset = bFn ? (dwCol * 4 + 0x15C8) : (dwCol * 4 + 0x15BC);
			TCHAR* szMsg = TEXT ("该快捷键位无任何技能");
			__asm {
				mov esi, [dwBaseL1L2]
				mov edi, [dwKeyOffset]
				mov esi, [esi + edi]
				mov esi, [esi + 0xc]
				mov edi, [dwIndex]
				mov esi, [esi + edi]
				test esi, esi
				je Failed

				mov edx, [esi]
				mov ecx, esi
				call [edx + 0x8]
				jmp Return

				Failed:
					push szMsg
					call MsgBox
				Return:
			}

			break;
		} // GAME_COM_ACCELERATOR_KEY
		case GAME_CMD_ACCELERATOR_SELECT: {		//选怪
			__asm {
				mov ecx, [dwBaseL1L2]
				push 0
				call [GAME_CALL_SELECT]
			}
			break;
		} // case GAME_CMD_ACCELERATOR_SELECT
		case GAME_CMD_ACCELERATOR_RETURN: {		//死亡回城
			__asm {
				mov eax, GAME_BASE
				mov eax, [eax]
				mov eax, [eax + 0x20]
				lea ecx, [eax + 0x270]
				call [GAME_CALL_RETURN]
			}
			break;
		} // case GAME_CMD_ACCELERATOR_RETURN
		case GAME_CMD_ACCELERATOR_SUPPLEMENT: {		//使用背包物品
			DWORD dwID = pContext[1];
			__asm {
				mov ebx, [dwBaseL1L2]
				mov ebx, [ebx + 0x1554]
				mov ebx, [ebx + 0xc]
				mov ecx, -1
				ForBegin:
					inc ecx
					cmp ecx, 0x17
					jg ForEnd
					mov edi, [ebx + ecx * 4]
					test edi, edi
					je ForBegin
					mov edx, [edi + 0xc]
					cmp edx, [dwID]
					jne ForBegin
					cmp [edi + 0x20], 0
					je ForEnd
						mov esi, GAME_BASE
						mov esi, [esi]
						mov esi, [esi + 0x20]
						push 1
						push [dwID]
						push ecx
						push 0
						lea ecx, [esi + 0x270]
						call [GAME_CALL_SUPPLEMENT]
				ForEnd:
			}
			break;
		}
		case GAME_CMD_ACCELERATOR_SKILL: {		//使用技能
			DWORD dwID = pContext[1];
			__asm {
				mov ecx, [dwBaseL1L2]
				push 1
				push 0
				push - 1
				push 0
				push 0
				push [dwID]
				call [GAME_CALL_SKILL]
			}
			break;
		}

		case GAME_CMD_CHAT: {		//聊天
			DWORD dwText = pContext[1];
			DWORD dwChatLevel;
			if (0 == pContext[2])
				dwChatLevel = 0x0012E500;	// 可能需要更新
			else
				dwChatLevel = 0x0012E502;	// 可能需要更新
			
			__asm {
				push 0xffffffff
				push 0xffffffff
				push [dwText]
				push [dwChatLevel]
				mov ecx, GAME_BASE
				mov ecx, [ecx]
				mov ecx, [ecx + 0x20]
				call [GAME_CALL_CHAT]
			}
			break;
		} // case GAME_CMD_CHAT:

		case GAME_CMD_GOTOXY: {		//去某处
			DWORD x = pContext[1];
			DWORD y = pContext[2];
			DWORD bAuto = pContext[3];

			if (!bAuto) {
				__asm {
					mov	edi, [dwBaseL1L2]

					push 1
					mov ecx, [edi + 0x15E4]		//Update
					call[GAME_CALL_GOTO]
					mov esi, eax
					; ...
					mov ecx, [edi + 0x15E4]		//Update
					push 0
					push 1
					push esi
					push 1
					call[GAME_CALL_GOTO + 4]

					mov edi, [edi + 0x15E4]		//Update
					mov edi, [edi + 0x30]
					mov edi, [edi + 0x4]
					mov eax, [x]
					mov edx, [y]
					mov[edi + 0x1C], eax
					mov[edi + 0x24], edx
				}
			}
			else {
				__asm {
					mov esi, GAME_BASE
					mov esi, [esi]
					mov esi, [esi + 0x1C]
					mov esi, [esi + 0x10]
					mov esi, [esi + 0x90]

					mov ebx, 0xDEB848			//Update
					push[x]
					pop dword ptr [ebx + 0]
					push[y]
					pop dword ptr[ebx + 8]

					mov edx, [dwBaseL1L2]
					lea edx, [edx + 0x828]

					push esi
					push ebx
					push edx
					mov ecx, 0xDE4610			//Update
					call[GAME_CALL_GOTO + 8]

				}
			}
			break;
		} // case GAME_CMD_GOTOXY:

	} // switch (dwCmd)

	return 0;
}
//
//bool __stdcall Trace (DWORD dwOut, PCTSTR pText, bool bHex)
//{
//	DWORD dwSize = _tcslen (pText) + 20;
//	PTSTR pBuffer = new TCHAR[dwSize]{};
//	if (nullptr == pText)	
//		return false;
//	
//	StringCchPrintf (pBuffer, dwSize, 
//		(bHex ? TEXT ("  %s：%08x") :TEXT ("  %s：%d")), pText, dwOut);
//
//	if (g_hWnd != 0) 
//	{
//		::SendMessage (g_hWnd, WM_SETTEXT, 0, (LPARAM)pBuffer);
//	}
//
//	delete[]pBuffer;
//
//	return true;
//}
