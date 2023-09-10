#pragma once
#include <Windows.h>

class DoubleBufferDraw;

class Console
{
private:
	HANDLE hConsole;
	CONSOLE_SCREEN_BUFFER_INFO stConsoleInfo;
	friend class DoubleBufferDraw;
public:
	enum class TextColor
	{
		//字体色
		black = 0,
		blue = FOREGROUND_BLUE,
		green = FOREGROUND_GREEN,
		light_green = FOREGROUND_GREEN | FOREGROUND_BLUE,
		red = FOREGROUND_RED,
		purple = FOREGROUND_BLUE | FOREGROUND_RED,
		yellow = FOREGROUND_GREEN | FOREGROUND_RED,
		white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
		gray = FOREGROUND_INTENSITY,
		bright_blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		bright_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		bright_light_green = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		bright_red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_purple = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		//背景色(使用时与字体色按位或:|)
		bg_blue = BACKGROUND_BLUE,
		bg_green = BACKGROUND_GREEN,
		bg_light_green = BACKGROUND_GREEN | BACKGROUND_BLUE,
		bg_red = BACKGROUND_RED,
		bg_purple = BACKGROUND_BLUE | BACKGROUND_RED,
		bg_yellow = BACKGROUND_GREEN | BACKGROUND_RED,
		bg_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED,
		bg_gray = BACKGROUND_INTENSITY,
		bg_bright_blue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		bg_bright_green = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		bg_bright_light_green = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		bg_bright_red = BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_purple = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_yellow = BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
	};

	struct CursorPos
	{
		SHORT x, y;
	};

	Console(HANDLE _hConsole = GetStdHandle(STD_OUTPUT_HANDLE)) :hConsole(_hConsole)
	{
		GetConsoleScreenBufferInfo(hConsole, &stConsoleInfo);
	}
	~Console(void) = default;

	HANDLE GetConsole(void)
	{
		return hConsole;
	}

	void SetConsole(HANDLE _hConsole)
	{
		hConsole = _hConsole;
	}

	DWORD WriteBuffer(const char *cpBuffer, DWORD dwWriteLen)
	{
		DWORD dwWrittenLen;
		if (!WriteConsoleA(hConsole, cpBuffer, dwWriteLen, &dwWrittenLen, NULL))
		{
			return 0;
		}
		return dwWrittenLen;
	}

	DWORD ClearBuffer(void)
	{
		DWORD dwWrittenLen;
		if (!FillConsoleOutputAttribute(hConsole, 0, stConsoleInfo.dwSize.X * stConsoleInfo.dwSize.Y, COORD{0, 0}, &dwWrittenLen))
		{
			return 0;
		}
		return dwWrittenLen;
	}

	//设置光标坐标
	bool SetCursorPos(SHORT x, SHORT y)
	{
		return SetConsoleCursorPosition(hConsole, COORD{x, y});//同步到控制台(Set设置)（Console控制台）（Cursor光标） （Position位置）
	}

	//获取光标坐标
	CursorPos GetCursorPos(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return {screen_buffer_info.dwCursorPosition.X,screen_buffer_info.dwCursorPosition.Y};
	}

	//从当前位置移动光标
	bool MoveCursorPos(SHORT x, SHORT y)
	{
		CursorPos stCurrentPos = GetCursorPos();
		return SetCursorPos(stCurrentPos.x + x, stCurrentPos.y + y);
	}

	//设置文本颜色
	bool SetTextColor(TextColor enColor)
	{
		return SetConsoleTextAttribute(hConsole, (WORD)enColor);
	}

	//获取文本颜色
	TextColor GetTextColor(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return (TextColor)screen_buffer_info.wAttributes;
	}

	//得到当前光标设置
	bool GetCursorShow(void)//return true代表光标可见
	{
		CONSOLE_CURSOR_INFO cursor_info;
		GetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
		return cursor_info.bVisible;
	}

	//光标隐藏||显示
	bool SetCursorShow(bool bShow)//Show=false代表光标不可见
	{
		CONSOLE_CURSOR_INFO cursor_info = {1, bShow};//1代表由光标填充的字符单元格的百分比。 此值介于1到100之间。 光标外观会变化，范围从完全填充单元格到单元底部的水平线条。
		return SetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
	}
};

class DoubleBufferDraw
{
private:
	Console hOldBuf;//保存对象，以便之后恢复
	Console hBuffer[2];
	unsigned long ulCurrent;
	DWORD dwError;
	bool bIsError;
public:
	DoubleBufferDraw(void) :
		hOldBuf(GetStdHandle(STD_OUTPUT_HANDLE)), 
		hBuffer
	{
		CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL),
		CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL),
	},
	ulCurrent(0),
	dwError(NO_ERROR),
	bIsError(false)
	{
		if (hBuffer[0].GetConsole() == INVALID_HANDLE_VALUE || hBuffer[1].GetConsole() == INVALID_HANDLE_VALUE)
		{
			dwError = GetLastError();
			bIsError = true;

			CloseHandle(hBuffer[0].GetConsole());
			CloseHandle(hBuffer[1].GetConsole());
			return;
		}
	}

	~DoubleBufferDraw(void)
	{
		//恢复成原来的
		SetConsoleActiveScreenBuffer(hOldBuf.GetConsole());
		//销毁句柄
		CloseHandle(hBuffer[0].GetConsole());
		CloseHandle(hBuffer[1].GetConsole());
	}

	void BegPrint(void)
	{
		hBuffer[ulCurrent].ClearBuffer();//清理缓冲区
	}

	void EndPrint(void)
	{
		SetConsoleActiveScreenBuffer(hBuffer[ulCurrent].GetConsole());//绘制结束输出
		ulCurrent = (ulCurrent + 1) & 1;//切换到下一个缓冲区（截断到第一位因为数组只有0和1）
	}

	DWORD WriteBuffer(const char *cpBuffer, DWORD dwWriteLen)
	{
		DWORD dwWrittenLen = hBuffer[ulCurrent].WriteBuffer(cpBuffer, dwWriteLen);
		if (dwWrittenLen == 0)
		{
			dwError = GetLastError();
			bIsError = true;
		}

		return dwWrittenLen;
	}

	DWORD GetLastError(void)
	{
		return dwError;
	}

	bool IsError(void)
	{
		return bIsError;
	}

	Console &GetConsole(void)
	{
		return hBuffer[ulCurrent];
	}

	Console &GetCurrentBuffer(void)
	{
		return hBuffer[ulCurrent];
	}

	Console *GetBufferArr(void)
	{
		return hBuffer;
	}
};