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
#include <string>
#include <wingdi.h>
#include <WinUser.h>
#include <windowsx.h>
#include <atlimage.h>

int WindowCount=0;

class VBWindow {

	HWND window;
	HINSTANCE winInstance;
	char* winCaption;
	HDC MyHDC=0;
	float mousex, mousey;
	bool mousedown=false;
	//used to set a callback function to call in the eventloop;
	int(*MSGCallback)(VBWindow*, UINT, WPARAM, LPARAM)=NULL;
	
public:
	//destructor
	~VBWindow() { DeleteDC(MyHDC); };

	//constructor
	VBWindow(HINSTANCE instance, char* classname)
	{
		OpenWindow(instance, classname);
	};

	//constructor overload
	VBWindow(HINSTANCE instance, char* classname, DWORD style )
	{
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
	void SetCaption(char* caption) {
		winCaption = caption;
		SetWindowText(window, winCaption);
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
		return mousex;
	}
	float GetMouseY() {
		return mousey;
	}
	bool MouseDown() {
		return mousedown;
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

	void SetBits(int* bits, int w, int h) {
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
			break;
		case WM_MOUSEMOVE:
			mousex = GET_X_LPARAM(lparam);
			mousey = GET_Y_LPARAM(lparam);
			break;
		case WM_LBUTTONDOWN:
			mousedown = true;
			break;
		case WM_LBUTTONUP:
			mousedown = false;
			break;
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


	void OpenWindow(HINSTANCE instance, char* ClassName, DWORD style=WS_OVERLAPPEDWINDOW) {
		winInstance = instance;
		WindowCount+=1;
		WNDCLASSA window_class = { 0 };
		window_class.lpfnWndProc = WindowProc;
		window_class.lpszClassName = ClassName;
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClassA(&window_class);

		window = CreateWindowA( ClassName,  winCaption, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, (void *)this );
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



