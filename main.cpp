#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Console_Operator.hpp"

class Tetris_Block
{
private:
	Console::TextColor enBlockColor;
	bool bBlock[4][4][4];//4*4*4的三维数组，分别是4*4方块旋转的4个形态
	char cBlock[3];//用于输出块的数组
	char cBlank[3];//用于输出空位的数组
	unsigned long ulRotate : 2;//只使用2bit，实现旋转环绕
public:
	Tetris_Block(
		bool _bBlock[4][4],
		Console::TextColor _enBlockColor = Console::TextColor::white,
		const char *_cBlock = "■",
		const char *_cNull = "□")
		: Tetris_Block((bool *)_bBlock, _enBlockColor, _cBlock, _cNull)//转发调用
	{}

	Tetris_Block(
		bool *_bBlock,
		Console::TextColor _enBlockColor = Console::TextColor::white,
		const char *_cBlock = "■",
		const char *_cNull = "□")
		: enBlockColor(_enBlockColor), cBlock{0}, cBlank{0}, ulRotate(0)
	{
		if (_cBlock != NULL)
		{
			memcpy(cBlock, _cBlock, sizeof(cBlock) - 1);//不拷贝最后一个，保留末尾0字符
		}
		if (_cNull != NULL)
		{
			memcpy(cBlank, _cNull, sizeof(cBlank) - 1);//不拷贝最后一个，保留末尾0字符
		}
		
		memcpy(&bBlock[0], _bBlock, sizeof(bBlock[0]));

		for (long z = 1; z < 4; ++z)
		{
			for (long y = 0; y < 4; ++y)
			{
				for (long x = 0; x < 4; ++x)
				{
					bBlock[z][y][x] = bBlock[z - 1][(4 - 1) - x][y];//二维数组右旋转
				}
			}
		}
		
		return;
	}

	~Tetris_Block(void) = default;


	void SetBlockColor(Console::TextColor _enBlockColor)
	{
		enBlockColor = _enBlockColor;
	}

	Console::TextColor GetBlockColor(void)
	{
		return enBlockColor;
	}

	void PrintBlockAndBlank(Console csConsole = GetStdHandle(STD_OUTPUT_HANDLE), long lStartX = 0, long lStartY = 0)
	{
		Console::TextColor enOldColor = csConsole.GetTextColor();//设置颜色之前保存原来的颜色
		csConsole.SetTextColor(enBlockColor);

		for (long y = 0; y < 4; ++y)
		{
			csConsole.SetCursorPos({lStartX, lStartY + y});
			for (long x = 0; x < 4; ++x)
			{
				if (bBlock[ulRotate][y][x])
				{
					csConsole.WriteBuffer(cBlock, sizeof(cBlock) - 1);
				}
				else
				{
					csConsole.WriteBuffer(cBlank, sizeof(cBlank) - 1);
				}
				
			}
		}

		csConsole.SetTextColor(enOldColor);
	}

	void PrintBlockSkipBlank(Console csConsole = GetStdHandle(STD_OUTPUT_HANDLE), long lStartX = 0, long lStartY = 0)
	{
		Console::TextColor enOldColor = csConsole.GetTextColor();
		csConsole.SetTextColor(enBlockColor);

		for (long y = 0; y < 4; ++y)
		{
			csConsole.SetCursorPos({lStartX, lStartY + y});
			for (long x = 0; x < 4; ++x)
			{
				if (bBlock[ulRotate][y][x])
				{
					csConsole.WriteBuffer(cBlock, sizeof(cBlock) - 1);
				}
				else//跳过这个空白
				{
					csConsole.MoveCursorPos({2, 0});
				}
			}
		}

		csConsole.SetTextColor(enOldColor);
	}

	void Rotation(long lRotate)//正右反左
	{
		ulRotate += lRotate;
	}

};

int main(void)
{
	bool test[4][4] =
	{
		0,0,0,0,
		0,0,1,0,
		0,1,1,1,
		0,0,0,0,
	};
	
	//俄罗斯方块对象
	Tetris_Block tb(test, Console::TextColor::bright_blue, "■", "  ");
	//多缓冲绘制
	DoubleBufferDraw dbd;


	while(true)
	{
		dbd.BegPrint();
	
		dbd.GetConsole().SetCursorShow(false);
		tb.PrintBlockSkipBlank(dbd.GetConsole(), 0, 0);
	
		dbd.EndPrint();
	
		//旋转
		tb.Rotation(-1);
		//随机方块颜色
		//tb.SetBlockColor((Console::TextColor)(rand() % ((int)Console::TextColor::bright_white + 1)));
		Sleep(80);
	}



	return 0;
}


