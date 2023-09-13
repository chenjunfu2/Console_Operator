#pragma once
#include <Windows.h>
#include <utility>

class Console
{
private:
	HANDLE hConsole;
public:
	enum class TextColor :unsigned long
	{
		//����ɫ
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
		//����ɫ(ʹ��ʱ������ɫ��λ��:|)
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
		long x, y;

		CursorPos(long _x, long _y) :x(_x), y(_y)
		{}

		CursorPos(const COORD &_crd) :x(_crd.X), y(_crd.Y)
		{}

		operator COORD(void) const
		{
			return COORD{(SHORT)x,(SHORT)y};
		}

		CursorPos &operator+=(const CursorPos &_r)
		{
			x += _r.x;
			y += _r.y;
			return *this;
		}

		CursorPos &operator-=(const CursorPos &_r)
		{
			x -= _r.x;
			y -= _r.y;
			return *this;
		}

		CursorPos operator+(const CursorPos &_r) const
		{
			return CursorPos(x + _r.x, y + _r.y);
		}

		CursorPos operator-(const CursorPos &_r) const
		{
			return CursorPos(x - _r.x, y - _r.y);
		}
	};

public:
	Console(HANDLE _hConsole = INVALID_HANDLE_VALUE) :hConsole(_hConsole)
	{}

	Console(const Console &) = default;

	Console(Console &&_Move) noexcept :hConsole(_Move.hConsole)
	{
		_Move.hConsole = INVALID_HANDLE_VALUE;
	}

	~Console(void) = default;

	Console &operator=(Console &&_Move) noexcept
	{
		hConsole = _Move.hConsole;
		_Move.hConsole = INVALID_HANDLE_VALUE;
		return *this;
	}

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
		CONSOLE_SCREEN_BUFFER_INFO stConsoleInfo;
		
		if (!GetConsoleScreenBufferInfo(hConsole, &stConsoleInfo) ||
			!FillConsoleOutputAttribute(hConsole, 0, stConsoleInfo.dwSize.X * stConsoleInfo.dwSize.Y, COORD{0, 0}, &dwWrittenLen))
		{
			return 0;
		}

		return dwWrittenLen;
	}

	//���ù������
	bool SetCursorPos(const CursorPos &stCursorPos)
	{
		return SetConsoleCursorPosition(hConsole, (COORD)stCursorPos);
	}

	//��ȡ�������
	CursorPos GetCursorPos(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return CursorPos(screen_buffer_info.dwCursorPosition);
	}

	//�ӵ�ǰλ���ƶ����
	bool MoveCursorPos(const CursorPos &stCursorPos)
	{
		return SetCursorPos(GetCursorPos() + stCursorPos);
	}

	//�����ı���ɫ
	bool SetTextColor(TextColor enColor)
	{
		return SetConsoleTextAttribute(hConsole, (WORD)enColor);
	}

	//��ȡ�ı���ɫ
	TextColor GetTextColor(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return (TextColor)screen_buffer_info.wAttributes;
	}

	//�õ���ǰ�������
	bool GetCursorShow(void)//return true������ɼ�
	{
		CONSOLE_CURSOR_INFO cursor_info;
		GetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
		return cursor_info.bVisible;
	}

	//�������||��ʾ
	bool SetCursorShow(bool bShow)//Show=false�����겻�ɼ�
	{
		CONSOLE_CURSOR_INFO cursor_info = {1, bShow};//1�����ɹ�������ַ���Ԫ��İٷֱȡ� ��ֵ����1��100֮�䡣 �����ۻ�仯����Χ����ȫ��䵥Ԫ�񵽵�Ԫ�ײ���ˮƽ������
		return SetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
	}
};

Console::TextColor operator|(const Console::TextColor &_l, const Console::TextColor &_r)//���ػ��������������TextColor
{
	return (Console::TextColor)((unsigned long)_l | (unsigned long)_r);
}

class DoubleBufferDraw
{
private:
	static constexpr long lBufferNum = 2;
	unsigned long ulCurrent;
	Console csBuffer[lBufferNum];
	Console csOldBuf;//��������Ա�֮��ָ�
public:
	DoubleBufferDraw(void)
	{
		ulCurrent = 0;
		csOldBuf = GetStdHandle(STD_OUTPUT_HANDLE);//���浱ǰ����̨��ʼ���
		for (long i = 0; i < lBufferNum; ++i)
		{
			HANDLE hNewConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			if (hNewConsole == INVALID_HANDLE_VALUE)
			{
				hNewConsole = csOldBuf.GetConsole();//����ʧ��ʹ�ó�ʼ���
			}
			csBuffer[i].SetConsole(hNewConsole);
		}
	}

	DoubleBufferDraw(DoubleBufferDraw &&_Move) noexcept :
		ulCurrent(_Move.ulCurrent),
		csOldBuf(std::move(_Move.csOldBuf))
	{
		for (long i = 0; i < lBufferNum; ++i)
		{
			csBuffer[i] = std::move(_Move.csBuffer[i]);
		}
	}

	DoubleBufferDraw(const DoubleBufferDraw &) = delete;

	~DoubleBufferDraw(void)
	{
		//�ָ���ԭ����
		SetConsoleActiveScreenBuffer(csOldBuf.GetConsole());
		//���پ��
		for (long i = 0; i < lBufferNum; ++i)
		{
			HANDLE hDelConsole = csBuffer[i].GetConsole();
			if (hDelConsole != csOldBuf.GetConsole())//�ر����зǳ�ʼ���
			{
				CloseHandle(hDelConsole);
			}
		}
	}

	DoubleBufferDraw &operator=(const DoubleBufferDraw &) = delete;
	DoubleBufferDraw &operator=(DoubleBufferDraw &&) = delete;

	void BegPrint(void)
	{
		csBuffer[ulCurrent].ClearBuffer();//��������
	}

	void EndPrint(void)
	{
		SetConsoleActiveScreenBuffer(csBuffer[ulCurrent].GetConsole());//���ƽ������
		ulCurrent = (ulCurrent + 1) % lBufferNum;//�л�����һ��������
	}

	Console &GetConsole(void)
	{
		return csBuffer[ulCurrent];
	}
};