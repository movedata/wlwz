#pragma once


extern "C" {

	DWORD 	__stdcall GameCommand (PVOID pv);
	//bool	__stdcall Trace (DWORD dwOut = 0, PCTSTR pText = TEXT ("д╛хопео╒"), bool bHex = true);
}

//extern HWND g_hWnd;

inline void __stdcall Helper (DWORD dwOut) {
	TCHAR sz[128] = {};
	StringCchPrintf (sz, 128, TEXT ("%X\n"), dwOut);
	OutputDebugString (sz);
}