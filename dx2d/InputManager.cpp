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
			curState[i] = (GetAsyncKeyState(i) & 0x8000) && true;

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

	char CInputManager::GetChar()
	{
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