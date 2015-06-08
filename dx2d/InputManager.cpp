#include "Private.h"

namespace dx2d
{
	CInputManager::CInputManager()
	{
		keyCount = 256;
		curState = new bool[keyCount];
		prevState = new bool[keyCount];
		curMouse = new POINT;
		prevMouse = new POINT;
		ZeroMemory(curState, sizeof(bool)*keyCount);
		ZeroMemory(prevState, sizeof(bool)*keyCount);
		GetCursorPos(curMouse);
		GetCursorPos(prevMouse);
	}

	void CInputManager::Activity()
	{
		//swap current and previous states
		bool* temp = prevState;
		prevState = curState;
		curState = temp;
		POINT* temp1 = prevMouse;
		prevMouse = curMouse;
		curMouse = temp1;

		//get current state
		for (int i = 0; i < keyCount; i++)
		{
			curState[i] = (GetAsyncKeyState(i) & 0x8000) && true;
		}

		GetCursorPos(curMouse);
	}

	bool CInputManager::IsKeyDown(int vKey)
	{
		return curState[vKey];
	}

	bool CInputManager::IsKeyPressed(int vKey)
	{
		return curState[vKey] && !prevState[vKey];
	}

	bool CInputManager::IsKeyReleased(int vKey)
	{
		return !curState[vKey] && prevState[vKey];
	}

	bool CInputManager::IsAnyKeyDown()
	{
		for (int i = 0; i < keyCount; i++)
			if (curState[i])
				return true;
		return false;
	}

	void CInputManager::GetCursorDelta(POINT* point)
	{
		point->x = curMouse->x - prevMouse->x;
		point->y = curMouse->y - prevMouse->y;
	}

	bool CInputManager::IsCapslockActive()
	{
		return false;
	}

	char CInputManager::GetChar(bool enableShift, bool enableCapslock)
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
				if (mod != 1)
					return output2[i];
				else
					return output[i];
			}
		}
		return 0;
	}

	char CInputManager::GetKey(int offset)
	{
		for (int i = offset; i < keyCount; i++)
			if (curState[i])
				return (char)i;
		return 0;
	}

	void CInputManager::ResetKey(int vKey)
	{
		if (vKey < 0 || vKey > keyCount)
			return;
		curState[vKey] = false;
		prevState[vKey] = false;
	}

	void CInputManager::Destroy()
	{
		delete[] curState;
		delete[] prevState;
		delete curMouse;
		delete prevMouse;
		delete this;
	}
}