// A simple window wrapper class loosely resembling Forms of classic Visual Basic
// use .DoEvents to process the message que.  A result of false means all windows have been closed
// If you have multiple widows, you only have to doevents on the first window
// because it reads all messages for all windows on the current thread.

////It's also possible, (but not required) to add callbacks into each window to process messages if we want to go the traditional event callback rouut.
////This is some boiler plate code
//int onMsg(VBWindow* VBwin, HWND wnd, UINT message, WPARAM wparam, LPARAM lparam) {

//int onMsg(VBWindow* vbw, UINT message, WPARAM wparam, LPARAM lparam) {
//	//this gives us a pointer to our VBWindow class that initiated the message.
//	//if we want the HWND, we can get it like this vbw->GetHWND();
//	switch (message) {
//	case WM_PAINT:
//		vbw->Cls();
//		for (int n = 0; n < 100; n++) { vbw->Circle(random(vbw->GetWidth()), random(vbw->GetHeight()), random(50)); }
//		break;
//	}
//	return 0;
//}

//Then add the callback to the desired window like this.  Each window can have it's own callback
//MyWindow.SetMsgCallback(onMsg);


#pragma once
#include <windows.h>
#include <commctrl.h>

#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
                                              name                  = 'Microsoft.Windows.Common-Controls' \
                                              version               = '6.0.0.0' \
                                              processorArchitecture = '*' \
                                              publicKeyToken        = '6595b64144ccf1df' \
                                              language              = '*'\"")

#include <string>
#include <wingdi.h>
#include <WinUser.h>
#include <windowsx.h>
#include <atlimage.h>
#include <CommCtrl.h>
#include <vector>
#pragma comment(lib,"Comctl32.lib")
 
int WindowCount=0;

class VBWindow {

	HWND window;
	HINSTANCE winInstance;
	HDC MyHDC=0;
	std::string winCaption;
	int childWindowIdentifierCounter=0;

	//used to set a callback function to call in the eventloop;
	int(*MSGCallback)(VBWindow*, UINT, WPARAM, LPARAM)=NULL;
	
public:
	//destructor
	~VBWindow() { DeleteDC(MyHDC); };

	//constructor
	VBWindow(HINSTANCE instance, std::string classname="DefaultWinClass")
	{
		InitControls();
		OpenWindow(instance, classname);
	};

	//constructor overload
	VBWindow(HINSTANCE instance, std::string classname, DWORD style )
	{
		InitControls();
		OpenWindow(instance, classname, style);
	};

	HWND GetHWND() {
		return window;
	}

	void Hide() {
		ShowWindow(window, SW_HIDE);
	};
	void Show() {
		ShowWindow(window, SW_SHOW);
	};
	void Close() {
		//I believe this posts a wm_close message to the window.
		DestroyWindow(window);
	};
	void SetCaption(std::string caption) {
		winCaption = caption.c_str();
		SetWindowText(window, caption.c_str());
		//need to repaint window?
	}
	void Move(int l, int t, int w, int h) {
		MoveWindow(window, l, t, w, h, true);
	}

	//left,top,width,height, etc.
	int GetWidth() {
		RECT rect;
		GetWindowRect(window, &rect);
		return rect.right-rect.left;
	}
	int GetHeight() {
		RECT rect;
		GetWindowRect(window, &rect);
		return rect.bottom - rect.top;
	}
	int GetLeft() {
		RECT rect;
	    GetWindowRect(window, &rect);
		return rect.left;
	}
	int GetTop() {
		RECT rect;
		GetWindowRect(window, &rect);
		return rect.top;
	}

	RECT GetRect() {
		RECT rect;
		GetWindowRect(window, &rect);
		return rect;
	}
	RECT GetWindowClientRect() {
		RECT rect;
		GetClientRect(window, &rect);
		return rect;
	}

	int GetClientWidth() {
		RECT rect;
		GetClientRect(window, &rect);
		return rect.right;
	}
	int GetClientHeight() {
		RECT rect;
		GetClientRect(window, &rect);
		return rect.bottom;
	}
	HDC GetHDC() {
		return MyHDC;
	}

	float GetMouseX() {
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(window,&p);
		return p.x;
	}
	float GetMouseY() {
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(window, &p);
		return p.y;
	}

	float GetScreenMouseX() {
		POINT p;
		GetCursorPos(&p);
		return p.x;
	}
	float GetScreenMouseY() {
		POINT p;
		GetCursorPos(&p);
		return p.y;
	}

	bool GetButtons(int button) {
		if (GetForegroundWindow() != window) return false;
		switch (button) {
		case 1:
			return GetAsyncKeyState(VK_LBUTTON);
		case 2:
			return  GetAsyncKeyState(VK_RBUTTON);
		case 4:
			return  GetAsyncKeyState(VK_MBUTTON);
		case 5:
			return  GetAsyncKeyState(VK_XBUTTON1);
		case 6:
			return  GetAsyncKeyState(VK_XBUTTON2);
		}
	}
	HCURSOR  SetMouseCursor(LPCSTR name) {
		HCURSOR loncursor=LoadCursor( 0 ,  name);
		HCURSOR old=SetCursor(loncursor);
		DestroyCursor(loncursor);
		return  old;
	}

	void Cls() {
		RECT rect ;
		GetClientRect(window, &rect);
		FillRect(GetHDC(), &rect, (HBRUSH)(COLOR_WINDOW + 1));
	}
	int GetCurrentX() {
		POINT p;
		GetCurrentPositionEx(GetHDC(), &p);
		return p.x;
	}
	int GetCurrentY() {
		POINT p;
		GetCurrentPositionEx(GetHDC(), &p);
		return p.y;
	}

	void SetCurrentX(int x) {
		MoveToEx(GetHDC(), x, GetCurrentY(), NULL);
	}
	void SetCurrentY(int y) {
		MoveToEx(GetHDC(), GetCurrentX(),y, NULL);
	}

	void Print(std::string text) {
		int x, y;
		x = GetCurrentX();
		y = GetCurrentY();
		TextOut(GetHDC(), x, y, text.c_str(), text.size() );
		TEXTMETRIC TM;
		GetTextMetrics(GetHDC(), &TM);
		y += TM.tmHeight;
		SetCurrentX(x);
		SetCurrentY(y);
		}

	void PrintWrap(std::string text) {
		RECT rect;
		GetClientRect(window, &rect);
		DrawText(GetHDC(), text.c_str(), text.length(), &rect, DT_LEFT);
	}
	
	void Refresh() {
		RedrawWindow(GetHWND(), NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
		//UpdateWindow(GetHWND());
	}
	void SetForeColor(int color) {
		SelectObject(GetHDC(), GetStockObject(DC_PEN));
		SetDCPenColor(GetHDC(), color);
	}
	void SetBackColor(int color) {
		SetBkColor(GetHDC(), color);
	}
	void SetTxtColor(int color) {
		SetTextColor(GetHDC(), color);
	}
	void SetBackMode(int mode) {
		//TRANSPARENT=1
		//OPAQUE=2	    
		SetBkMode(GetHDC(), mode);
	}
	void SetFillColor(int color) {
		SelectObject(GetHDC(), GetStockObject(DC_BRUSH));
		SetDCBrushColor(GetHDC(), color);
	}

	void Line(int x1, int y1, int x2, int y2) {
		MoveToEx(GetHDC(), x1, y1, NULL);
		LineTo(GetHDC(), x2, y2);
	}
	void Line_To(int x, int y) {
		LineTo(GetHDC(), x, y);
	}
	void Pset(int x, int y, int color) {
		SetPixelV(GetHDC(), x, y, color);
	}

	int Point(int x, int y) {
		return GetPixel(GetHDC(), x, y);
	}

	void Box(int x1, int y1, int x2, int y2) {
		Rectangle(GetHDC(),x1,y1,x2,y2);
	}

	void Circle(int x, int y, int radius) {
		Ellipse(GetHDC(),x-radius,y-radius,x+radius,y+radius);
	}

	void SetBits(const unsigned int* bits, int w, int h) {
		//if ((w == GetClientWidth()) && (h == GetClientHeight())) {
			CImage c;
			c.Create(w, -h, 32, 0);
			//writing pixels via the cimage dib section
			//be sure the cImage is created with negative height
			unsigned int* pointer = (unsigned int*)c.GetBits();
			for (int y = 0; y < h; y++) {
				int offset = y*w;
				for (int x = 0; x < w; x++) {
					pointer[offset] = bits[offset++];
				};
			}
			c.BitBlt(GetHDC(), 0, 0,w,h, 0, 0, SRCCOPY);
			c.Destroy();
		//}
	}
	//actually don't think I'll support getting bits from the image, because it's not in a persistant bitmap anyway.
	//void GetBits(std::vector<int>* bits) {
	//	int size = GetClientWidth() * GetClientHeight());
	//	bits->resize(size);
	//}

	bool IsActive() {
		return (bool(GetActiveWindow() == window));
	}
	void Focus() {
		SetFocus(window);
	}

	int MakeButton(std::string Caption, int l, int t, int w, int h) {
		HWND result = CreateWindow(TEXT("BUTTON"), Caption.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeRadioButton(std::string Caption, int l, int t, int w, int h, bool checked) {
		HWND result = CreateWindow(TEXT("BUTTON"), Caption.c_str(), WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_CHILD,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		CheckDlgButton(window, childWindowIdentifierCounter, BST_CHECKED & checked);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeCheckbox(std::string Caption, int l, int t, int w, int h, bool checked) {
		HWND result = CreateWindow(TEXT("BUTTON"), Caption.c_str(), WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		CheckDlgButton(window, childWindowIdentifierCounter, BST_CHECKED & checked);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeLabel(std::string Caption, int l, int t, int w, int h) {
		HWND result = CreateWindow(TEXT("STATIC"), Caption.c_str(), WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		//WS_BORDER
		ShowWindow(result, SW_SHOW);
		//needs to be called message callback
		//HDC hdc = GetWindowDC(result);
		//SetBkMode(hdc, TRANSPARENT);
		//DeleteDC(hdc);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeTrackbar(std::string Caption, int l, int t, int w, int h, int low, int high) {

		HWND result = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
			WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS ,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, NULL, NULL);

		SendMessageW(result, TBM_SETRANGE, TRUE, MAKELONG(low, high));
		SendMessageW(result, TBM_SETPAGESIZE, 0, 10);
		SendMessageW(result, TBM_SETTICFREQ, 10, 0);
		SendMessageW(result, TBM_SETPOS, FALSE, 0);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeDropdown(int l, int t, int w, int h, std::vector<std::string> list) {
		HWND result = CreateWindow(TEXT("COMBOBOX"),TEXT(""), WS_TABSTOP | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_VISIBLE,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);

		for (int k = 0; k < (int)list.size(); k ++)
		{
			// Add string to combobox.
			SendMessage(result, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)list[k].c_str());
		}
		// Send the CB_SETCURSEL message to display an initial item 
//  in the selection field  
		SendMessage(result, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		return childWindowIdentifierCounter++;
	}


	int MakeSplitButton(std::string Caption,int l, int t, int w, int h) {
		HWND result = CreateWindow(TEXT("Button"), TEXT(Caption.c_str()), WS_TABSTOP | WS_CHILD | BS_SPLITBUTTON | CBS_HASSTRINGS | WS_OVERLAPPED | WS_VISIBLE,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}


	int MakeCommandLink(std::string Caption, int l, int t, int w, int h) {
		HWND result = CreateWindow(TEXT("Button"), TEXT(Caption.c_str()), WS_TABSTOP | WS_CHILD | BS_COMMANDLINK | CBS_HASSTRINGS | WS_OVERLAPPED | WS_VISIBLE,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}

	int MakeHyperLink(std::string Caption, int l, int t, int w, int h) {
		HWND result = CreateWindowEx(0,TEXT("SYSLINK"),TEXT(Caption.c_str()), WS_VISIBLE | WS_CHILD | WS_TABSTOP ,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		return childWindowIdentifierCounter++;
	}
 
	int MakeTextInput(std::string Text, int l, int t, int w, int h, bool Multiline=false) {
		int ml = 0;
		if (Multiline = true) ml = (ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_LEFT);
		HWND result = CreateWindowEx(0, TEXT("EDIT"), TEXT(Text.c_str()), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ml ,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		// Add text to the window. 
		//SendMessage(result, WM_SETTEXT, 0, (LPARAM)Text.c_str());
		return childWindowIdentifierCounter++;
	}

	int MakeNumericInput(std::string value, int l, int t, int w, int h) {
		HWND result = CreateWindowEx(0, TEXT("EDIT"),  TEXT(value.c_str()), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_LEFT | ES_NUMBER,
			l, t, w, h, window, (HMENU)childWindowIdentifierCounter, winInstance, NULL);
		ShowWindow(result, SW_SHOW);
		UpdateWindow(result);
		// Add text to the window. 
		//SendMessage(result, WM_SETTEXT, 0, (LPARAM)value.c_str());
		return childWindowIdentifierCounter++;
	}

	void MakePopup(std::vector<std::string> list,int x, int y) {
		// Get screen coordinates of the button.
		POINT pt;
		pt.x = x;
		pt.y = y;
		ClientToScreen(window, &pt);

		// Create a menu and add items.
		HMENU hMenu = CreatePopupMenu();
		for (int n = 0; n < list.size(); n++) {
			AppendMenu(hMenu, MF_BYPOSITION | MF_STRING, n, list[n].c_str());
		}
		TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, window, NULL);
	}
	
	//void TDialog() {
	//	int* pnbutts;
	//	pnbutts = 0;
	//	TaskDialog(window, winInstance, (PCWSTR)"Title", (PCWSTR)"This is the ain instruction", (PCWSTR)"This is the content",
	//		TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, TD_INFORMATION_ICON, pnbutts);
	//}


	//void MakeMenu() {
	//	HMENU hMenubar = CreateMenu();
	//	HMENU hMenu = CreateMenu();
	//	UINT_PTR IDM_FILE_NEW;
	//	UINT_PTR IDM_FILE_OPEN;
	//	UINT_PTR IDM_FILE_QUIT;
	//	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	//	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
	//	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	//	AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");
	//	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
	//}

	
	void SetMsgCallback( int(*func)(VBWindow*, UINT, WPARAM, LPARAM)) {
		
		MSGCallback = func;
		
		};



	//returns true if quit is pressed.
	bool DoEvents(bool wait=false) {
		if (WindowCount==0) { return true; };
		//WaitMessage();
		MSG message;
		int result;
		if (wait == true) result=GetMessage(&message, NULL, 0, 0);
		if (wait == false) result = PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
		if (result){
			//if (message.message=WM_QUIT)
			TranslateMessage(&message);
			DispatchMessage(&message);
		};
		return false;
	}

private:
	////the fake window message loop
	static LRESULT CALLBACK WindowProc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam) {

		LRESULT result = 0;
	
			VBWindow* pWnd;

			if (message == WM_NCCREATE)
			{
				// get the pointer to the window from
				// lpCreateParams which was set in CreateWindow
				SetWindowLong(wnd, GWL_USERDATA,
					(long)((LPCREATESTRUCT(lparam))->lpCreateParams));
			}

			// get the pointer to the window
			pWnd = GetObjectFromWindow(wnd);

			// if we have the pointer, go to the message handler of the window
			// else, use DefWindowProc
			if (pWnd)
				return pWnd->WinMsgHandler(wnd, message, wparam, lparam);
			else
				return DefWindowProc(wnd, message, wparam, lparam);

		return(result);

	}
protected:

	// the real message loop
	virtual LRESULT CALLBACK WinMsgHandler(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam) {
		LRESULT result = 0;

	
		switch (message) {
		case WM_DESTROY:
			WindowCount -= 1;
			break;
		case WM_PAINT:
			DrawSomething();//needed to get windows to stop sending paint events over and over and over...
			//Refresh();
			break;
		//case WM_MOUSEMOVE:
		//	//mousex = GET_X_LPARAM(lparam);
		//	//mousey = GET_Y_LPARAM(lparam);
		//	mousex = LOWORD(lparam);
		//	mousey = HIWORD(lparam);
		//	break;
		//case WM_LBUTTONDOWN:
		//	MousedownL = true;
		//	break;
		//case WM_LBUTTONUP:
		//	MousedownL = false;
		//	break;
		//case WM_RBUTTONDOWN:
		//	MousedownR = true;
		//	break;
		//case WM_RBUTTONUP:
		//	MousedownR = false;
		//	break;
		//case WM_MBUTTONDOWN:
		//	MousedownM = true;
		//	break;
		//case WM_MBUTTONUP:
		//	MousedownM = false;
		//	break;
		//case WM_COMMAND:
		//	//do callback of button handling code with LOWORD(wparam)){
		//	break;
		//case EM_SCROLL:
		//	//do callback of trackbar code
		//	break;
		default:
			result = DefWindowProcA(wnd, message, wparam, lparam);
			break;
		}
		//calls an optional callback function, if asigned.
		//the callback does not ned to invoke DefWindowProcA
		//a callback needs to be assigned to each window where it's desired.
		if (MSGCallback != NULL) MSGCallback(this, message, wparam, lparam);

		return result;
	};

	// returns a pointer the window (stored as the WindowLong)
	inline static VBWindow* GetObjectFromWindow(HWND hWnd)
	{
		return (VBWindow*)GetWindowLong(hWnd, GWL_USERDATA);
	}


private:

	void InitControls() {
		INITCOMMONCONTROLSEX iccx;
		iccx.dwSize = sizeof iccx;
		iccx.dwICC = ICC_STANDARD_CLASSES;
		InitCommonControlsEx(&iccx);
		//calls to InitCommonControlsEx are cumulative
		iccx.dwICC = ICC_LINK_CLASS;
		InitCommonControlsEx(&iccx);
		//
		iccx.dwICC = ICC_PROGRESS_CLASS;
		InitCommonControlsEx(&iccx);
		//
		iccx.dwICC = ICC_USEREX_CLASSES;
		InitCommonControlsEx(&iccx);
	}

	void OpenWindow(HINSTANCE instance, std::string ClassName, DWORD style=WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN ) {
		winInstance = instance;
		WindowCount+=1;
		WNDCLASSA window_class = { 0 };
		window_class.lpfnWndProc = WindowProc;
		window_class.lpszClassName = ClassName.c_str();
		window_class.hbrBackground= GetSysColorBrush(COLOR_3DFACE);
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		window_class.hCursor = LoadCursor(0,IDC_ARROW);
		RegisterClassA(&window_class);

		window = CreateWindowA( ClassName.c_str(),  winCaption.c_str(), style,  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, (void *)this );
		MyHDC = GetDC(window);  //GetWindowDC gets the entire window dc including non client areas.
	}

	void DrawSomething() {
		//I think we need to do a minimum of this to get WINDOWS to stop sending paint events.
		PAINTSTRUCT ps;
		RECT MyRect;
		HDC hdc = BeginPaint(window, &ps);
		GetClientRect(window, &MyRect);
		ValidateRect(window, &MyRect);
		DeleteDC(hdc);
		EndPaint(window, &ps);
	}

};



