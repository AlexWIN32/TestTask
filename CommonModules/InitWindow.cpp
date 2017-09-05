/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <InitWindow.h>
#include <Utils/AutoCOM.h>

HWND InitWindow(HINSTANCE Instance, LONG ClientWidth, LONG ClientHeight, const std::wstring &Caption, MainWndProc WndProc, DWORD Style) throw (Exception)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Instance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
		throw Exception("RegisterClass Failed");

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, Style, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	HWND mhMainWnd = CreateWindow(L"D3DWndClassName", Caption.c_str(),
		Style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, Instance, 0);
	if (!mhMainWnd)
		throw Exception("CreateWindow Failed");

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return mhMainWnd;
}
