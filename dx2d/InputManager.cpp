#include "Private.h"

namespace dx2d
{
	CInput::CInput()
	{
		zMouseWheel = 0;
		zKeyCount = 256;
		zCurState = new bool[zKeyCount];
		zPrevState = new bool[zKeyCount];
		zCurMouse = new POINT;
		zPrevMouse = new POINT;
		ZeroMemory(zCurState, sizeof(bool)*zKeyCount);
		ZeroMemory(zPrevState, sizeof(bool)*zKeyCount);
		GetCursorPos(zCurMouse);
		GetCursorPos(zPrevMouse);
	}

	void CInput::zActivity()
	{
		//swap current and previous states
		zMouseWheel = 0;
		bool* temp = zPrevState;
		zPrevState = zCurState;
		zCurState = temp;
		POINT* temp1 = zPrevMouse;
		zPrevMouse = zCurMouse;
		zCurMouse = temp1;

		//get current state
		for (int i = 0; i < zKeyCount; i++)
		{
			zCurState[i] = (GetAsyncKeyState(i) & 0x8000) && true;
		}

		GetCursorPos(zCurMouse);
	}

	bool CInput::IsKeyDown(int vKey)
	{
		return zCurState[vKey];
	}

	bool CInput::IsKeyPressed(int vKey)
	{
		return zCurState[vKey] && !zPrevState[vKey];
	}

	bool CInput::IsKeyReleased(int vKey)
	{
		return !zCurState[vKey] && zPrevState[vKey];
	}

	bool CInput::IsAnyKeyDown()
	{
		for (int i = 0; i < zKeyCount; i++)
			if (zCurState[i])
				return true;
		return false;
	}

	POINT CInput::GetCursorDelta()
	{
		POINT point;
		point.x = zCurMouse->x - zPrevMouse->x;
		point.y = zCurMouse->y - zPrevMouse->y;
		return point;
	}

	bool CInput::IsCapslockActive()
	{
		return GetKeyState(Key.CapsLock) & 1;
	}

	char CInput::GetChar(bool enableShift, bool enableCapslock)
	{
		BYTE input[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'Q', 'W', 'E', 'R', 'T',
			'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
			'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0xc0, 0xbd, 0xbb, 0xdc, 0xdb, 
			0xdd, 0xba, 0xde, 0xbc, 0xbe, 0xbf,
			' ', 0x0d, '\t', '\b' };
		BYTE output[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'q', 'w', 'e', 'r', 't',
			'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
			'z', 'x', 'c', 'v', 'b', 'n', 'm', '`', '-', '=', '\\', '[', ']', ';', '\'', ',', '.', '/',
			' ', '\n', '\t', '\b' };
		BYTE output2[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(', 'Q', 'W', 'E', 'R', 'T',
			'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
			'Z', 'X', 'C', 'V', 'B', 'N', 'M', '~', '_', '+', '|', '{', '}', ':', '\"', '<', '>', '?',
			' ', '\n', '\t', '\b' };
		char mod = (enableShift && IsKeyDown(Key.Shift)) + (enableCapslock && IsCapslockActive());
		for (int i = 0; i < sizeof(input); i++)
		{
			if (IsKeyPressed(input[i]))
			{
				if (mod == 1)
					return output2[i];
				else
					return output[i];
			}
		}
		return 0;
	}

	char CInput::GetKey(int offset)
	{
		for (int i = offset; i < zKeyCount; i++)
			if (zCurState[i])
				return (char)i;
		return 0;
	}

	void CInput::ResetKey(int vKey)
	{
		if (vKey < 0 || vKey > zKeyCount)
			return;
		zCurState[vKey] = false;
		zPrevState[vKey] = false;
	}

	int CInput::GetMouseWheel()
	{
		return zMouseWheel;
	}

	void CInput::Destroy()
	{
		delete[] zCurState;
		delete[] zPrevState;
		delete zCurMouse;
		delete zPrevMouse;
		delete this;
	}
}