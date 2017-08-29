/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <DirectInput.h>
#include <COMHelpers.h>

DirectInput *DirectInput::instance = NULL;

DirectInput::DirectInput()
{
    memset(kbBtnsDownState, 0, sizeof(kbBtnsDownState));
    memset(kbBtnsPressState, 0, sizeof(kbBtnsPressState));

    memset(msBtnsDownState, 0, sizeof(msBtnsDownState));
    memset(msBtnsPressState, 0, sizeof(msBtnsPressState));
}

DirectInput::~DirectInput()
{
    ReleaseCOM(Keyboard);
    ReleaseCOM(Mouse);
}

void DirectInput::Init(HINSTANCE AppInstance, HWND Hwnd) throw (Exception)
{
	hwnd = Hwnd;

	IDirectInput8* DInput;
	if (FAILED(DirectInput8Create(AppInstance, 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		reinterpret_cast<void**>(&DInput), 0)))
		throw DirectInputException("cant create direct input");
		

	if (FAILED(DInput->CreateDevice(GUID_SysKeyboard, &Keyboard, 0)))
		throw DirectInputException("cant create keyboard device");

	Keyboard->SetDataFormat(&c_dfDIKeyboard);
	Keyboard->SetCooperativeLevel(Hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	Keyboard->Acquire();

	if (FAILED(DInput->CreateDevice(GUID_SysMouse, &Mouse, 0)))
		throw DirectInputException("cant create mouse device");

	Mouse->SetDataFormat(&c_dfDIMouse2);
	Mouse->SetCooperativeLevel(Hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	Mouse->Acquire();

    DIPROPDWORD dipDw = {};
    dipDw.diph.dwSize = sizeof(DIPROPDWORD);
    dipDw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipDw.diph.dwHow = DIPH_BYOFFSET;
 
    dipDw.diph.dwObj = DIMOFS_X;
    if(FAILED(Mouse->GetProperty(DIPROP_GRANULARITY, &dipDw.diph)))
        throw DirectInputException("cant get mouse property");
    mouseOffset.x = dipDw.dwData;

    dipDw.diph.dwObj = DIMOFS_Y;
    if(FAILED(Mouse->GetProperty(DIPROP_GRANULARITY, &dipDw.diph)))
        throw DirectInputException("cant get mouse property");
    mouseOffset.y = dipDw.dwData;

    dipDw.diph.dwObj = DIMOFS_Z;
    if(FAILED(Mouse->GetProperty(DIPROP_GRANULARITY, &dipDw.diph)))
        throw DirectInputException("cant get mouse property");
    mouseOffset.z = dipDw.dwData;

	DInput->Release();
}

void DirectInput::Poll()
{	
	::GetCursorPos(&mouseCursorPos);
	ScreenToClient(hwnd, &mouseCursorPos);

	char keyboardState[256] = { 0 };
	if (FAILED(Keyboard->GetDeviceState(sizeof(keyboardState), reinterpret_cast<void**>(&keyboardState))))
		Keyboard->Acquire();

	DIMOUSESTATE2 mouseState = { 0 };
	if (FAILED(Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), reinterpret_cast<void**>(&mouseState))))
		Mouse->Acquire();

    typedText = L"";

    BYTE kbVirtCodes[256] = {0};
    GetKeyboardState(kbVirtCodes);

	for (int i = 0; i < 256; i++)
		if (keyboardState[i] & 0x80)
			kbBtnsDownState[i] = true;					
		else if (kbBtnsDownState[i]){
			kbBtnsDownState[i] = false;
			kbBtnsPressState[i] = true;

            UINT vk = MapVirtualKey(i, MAPVK_VSC_TO_VK);

            WCHAR res[5] = {};

            if(ToUnicode(vk, i, kbVirtCodes, res, 4, 0) == 1){
                if(res[0] != '\b' && res[0] != '\r' && res[0] != '\n' && res[0] != '\t' && res[0] != '\x1b')
                    typedText += res;
            }
		}
		else
			kbBtnsPressState[i] = false;

    mouseDelta.x = mouseState.lX / mouseOffset.x;
    mouseDelta.y = mouseState.lY / mouseOffset.y;
    mouseDelta.z = mouseState.lZ / mouseOffset.z;

	for (int i = 0; i < 8; i++)
		if (mouseState.rgbButtons[i] & 0x80)
			msBtnsDownState[i] = true;
		else if (msBtnsDownState[i]){
			msBtnsDownState[i] = false;
			msBtnsPressState[i] = true;
		}
		else
			msBtnsPressState[i] = false;
}

void DirectInput::ProcessKeyboardPress(const ButtonHandlersStorage &Handlers)
{
    for(const ButtonHandler &hdr : Handlers)
        if(kbBtnsPressState[hdr.code])
            hdr.handler();
}

void DirectInput::ProcessKeyboardDown(const ButtonHandlersStorage &Handlers)
{
    for(const ButtonHandler &hdr : Handlers)
        if(kbBtnsDownState[hdr.code])
            hdr.handler();
}