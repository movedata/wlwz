#pragma once

BOOL Dlg_OnInitDialog (HWND hWnd, HWND hwndFocus, LPARAM lParam);
void Cls_OnTimer (HWND hWnd, UINT id);
void Dlg_OnCommand (HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
INT_PTR WINAPI Dlg_Proc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

