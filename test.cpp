
#include "VBWindow.h"
#include <Vector>

float random(float max) {
	return (float)(rand() / (float)RAND_MAX)*max;
}

////these are callbacks that we can hook into each window to process messages, if we want to go the traditional event callback rouut.
//	int onMsg(VBWindow* vbw, UINT message, WPARAM wparam, LPARAM lparam) {
//	//this gives us a pointer to our VBWindow class that initiated the message.
//	//if we want the HWND, we can get it like this vbw->GetHWND();
//	switch (message) {
//		case WM_PAINT:
//			vbw->Cls();
//			for (int n = 0; n < 100; n++) { vbw->Circle(random(vbw->GetWidth()), random(vbw->GetHeight()), random(50));}
//		break;
//		}
//	return 0;
//}



int __stdcall WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_command) {

	VBWindow MyWindow(instance, "MyClass1");
	MyWindow.Show();
	MyWindow.SetCaption ("My Window");
	MyWindow.Move(20, 40, 500, 400);
	
	

	VBWindow MyWindow2(instance, "MyClass2");
	MyWindow2.Show();
	MyWindow2.SetCaption("My Wndow 2");
	MyWindow2.SetMouseCursor(IDC_HAND);
	
	//it's possible to add a callback function for event processing.
	//MyWindow.SetMsgCallback(onMsg);
	

	MyWindow2.SetBackColor(0xFFFFFF);
	MyWindow2.SetTxtColor(0xFF0000);
	MyWindow2.SetBackMode(2);//1 or 2 or  TRANSPARENT or OPAQUE

	//while ((MyWindow.DoEvents()==false) || (MyWindow2.DoEvents()==false)) {};
	//so, interestingly, we only test one window, and all windows work?
	while (MyWindow.DoEvents() == false) {
		Sleep(6);

		//test getting HWND from VBWindow object
		//InvalidateRect(MyWindow.GetHWND(), &MyWindow.GetWindowClientRect(), 0);

		
		//MyWindow.Cls();
		MyWindow.Print("Hi mom");
		//MyWindow2.Cls();

		MyWindow2.SetCurrentX(100);
		MyWindow2.SetCurrentY(100);
		MyWindow2.Print("Next thing goes here");
		MyWindow2.Print("And Here it is");
		MyWindow.SetForeColor(RGB(random(255),random(255),random(255)));
		MyWindow.SetFillColor(RGB(random(255), random(255), random(255)));
		MyWindow2.SetForeColor(RGB(random(255), random(255), random(255)));
		MyWindow2.SetFillColor(RGB(random(255), random(255), random(255)));
		MyWindow2.Box(random(MyWindow2.GetWidth()), random(MyWindow2.GetHeight()), random(MyWindow2.GetWidth()) , random(MyWindow2.GetHeight()) );
		MyWindow.Circle(random(MyWindow2.GetWidth()), random(MyWindow2.GetHeight()), random(400));

		////test .SetBits
		//std::vector<int> v;
		//int w = MyWindow.GetClientWidth();
		//int h = MyWindow.GetClientHeight();
		//int size = w * h;
		//v.resize(size);
		//for (int n = 0; n < size; n++) { v[n] = RGB(random(255), random(255), random(255)); }

		//MyWindow.SetBits(&v[0], w, h);
	};
	
}


