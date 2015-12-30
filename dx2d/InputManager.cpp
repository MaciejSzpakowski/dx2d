#include "Viva.h"

namespace Viva
{
	CInputManager::CInputManager()
	{
		mouseWheel = 0;
		keyCount = 256;
		curState = new bool[keyCount];
		prevState = new bool[keyCount];
		curMouse = new POINT;
		prevMouse = new POINT;
		gamepadStatePrev = new XINPUT_STATE[XUSER_MAX_COUNT];
		gamepadStateCur = new XINPUT_STATE[XUSER_MAX_COUNT];
		ZeroMemory(curState, sizeof(bool)*keyCount);
		ZeroMemory(prevState, sizeof(bool)*keyCount);
		ZeroMemory(gamepadStatePrev, sizeof(XINPUT_STATE)*XUSER_MAX_COUNT);
		ZeroMemory(gamepadStateCur, sizeof(XINPUT_STATE)*XUSER_MAX_COUNT);
		acceptGamepads = 0;
		activeGamepads[0] = false;
		activeGamepads[1] = false;
		activeGamepads[2] = false;
		activeGamepads[3] = false;
		gamepadConnected = false;
		gamepadConnected = false;
		GetCursorPos(curMouse);
		GetCursorPos(prevMouse);
	}

	void CInputManager::_Activity()
	{
		//swap current and previous states
		mouseWheel = 0;
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

		_CheckGamepads();

		GetCursorPos(curMouse);
	}

	POINT CInputManager::GetCursorClientPos()
	{
		POINT p;
		::GetCursorPos(&p);
		ScreenToClient(Core->zWindow->zHandle, &p);
		return p;
	}

	bool CInputManager::IsKeyDown(Keys vKey)
	{
		return curState[(int)vKey];
	}

	bool CInputManager::IsKeyPressed(Keys vKey)
	{
		return curState[(int)vKey] && !prevState[(int)vKey];
	}

	bool CInputManager::IsKeyReleased(Keys vKey)
	{
		return !curState[(int)vKey] && prevState[(int)vKey];
	}

	bool CInputManager::IsAnyKeyDown()
	{
		for (int i = 0; i < keyCount; i++)
			if (curState[i])
				return true;
		return false;
	}

	POINT CInputManager::GetCursorDelta()
	{
		POINT point;
		point.x = curMouse->x - prevMouse->x;
		point.y = curMouse->y - prevMouse->y;
		return point;
	}

	bool CInputManager::IsCapslockActive()
	{
		return GetKeyState((int)Keys::CapsLock) & 1;
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
		// from combination of capslock and shit, figure out what is the case
		char mod = (enableShift && IsKeyDown(Keys::Shift)) + (enableCapslock && IsCapslockActive());
		for (int i = 0; i < sizeof(input); i++)
		{
			if (IsKeyPressed((Keys)input[i]))
			{
				if (mod == 1)
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

	int CInputManager::GetMouseWheel()
	{
		return mouseWheel;
	}

	void CInputManager::_CheckGamepads()
	{
		gamepadConnected = false;
		gamepadConnected = false;
		if (acceptGamepads > XUSER_MAX_COUNT)
			acceptGamepads = XUSER_MAX_COUNT;
		for (UINT i = 0; i < acceptGamepads; i++)
		{
			auto temp = gamepadStatePrev;
			gamepadStatePrev = gamepadStateCur;
			gamepadStateCur = temp;
			ZeroMemory(gamepadStateCur + i, sizeof(XINPUT_STATE));
			DWORD dwResult = XInputGetState(i, gamepadStateCur + i);

			if (dwResult == ERROR_SUCCESS)
			{
				if (activeGamepads[i] == false)
					gamepadConnected = true;
				activeGamepads[i] = true;
			}
			else
			{
				if (activeGamepads[i] == true)
					gamepadConnected = true;
				activeGamepads[i] = false;
			}
		}
	}

	bool CInputManager::IsAnyButtonDown(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return (gamepadStateCur[gamepad].Gamepad.wButtons) && true;
	}

	bool CInputManager::IsButtonDown(UINT gamepad, Buttons button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return (gamepadStateCur[gamepad].Gamepad.wButtons & (int)button) && true;
	}

	bool CInputManager::IsButtonPressed(UINT gamepad, Buttons button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return (gamepadStateCur[gamepad].Gamepad.wButtons & (int)button) &&
			!(gamepadStatePrev[gamepad].Gamepad.wButtons & (int)button);
	}

	bool CInputManager::IsButtonReleased(UINT gamepad, Buttons button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return !(gamepadStateCur[gamepad].Gamepad.wButtons & (int)button) &&
			(gamepadStatePrev[gamepad].Gamepad.wButtons & (int)button);
	}

	bool CInputManager::IsGamepadActive(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return activeGamepads[gamepad];
	}

	bool CInputManager::GamepadConnected()
	{
		return gamepadConnected;
	}

	bool CInputManager::GamepadDisconnected()
	{
		return gamepadConnected;
	}

	BYTE CInputManager::GetLeftTrigger(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.bLeftTrigger;
	}

	BYTE CInputManager::GetRightTrigger(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.bRightTrigger;
	}

	SHORT CInputManager::GetLeftStickX(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbLX;
	}

	SHORT CInputManager::GetLeftStickY(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbLY;
	}

	SHORT CInputManager::GetRightStickX(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbRX;
	}

	SHORT CInputManager::GetRightStickY(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbRY;
	}

	BYTE CInputManager::GetLeftTriggerDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.bLeftTrigger -
			gamepadStatePrev[gamepad].Gamepad.bLeftTrigger;
	}

	BYTE CInputManager::GetRightTriggerDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.bRightTrigger -
			gamepadStatePrev[gamepad].Gamepad.bRightTrigger;
	}

	SHORT CInputManager::GetLeftStickXDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbLX -
			gamepadStatePrev[gamepad].Gamepad.sThumbLX;
	}

	SHORT CInputManager::GetLeftStickYDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbLY -
			gamepadStatePrev[gamepad].Gamepad.sThumbLY;
	}

	SHORT CInputManager::GetRightStickXDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbRX -
			gamepadStatePrev[gamepad].Gamepad.sThumbRX;
	}

	SHORT CInputManager::GetRightStickYDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return gamepadStateCur[gamepad].Gamepad.sThumbRY -
			gamepadStatePrev[gamepad].Gamepad.sThumbRY;
	}

	void CInputManager::SetMotorSpeed(UINT gamepad, USHORT speedLeft, USHORT speedRight)
	{
		XINPUT_VIBRATION vib;
		vib.wLeftMotorSpeed = speedLeft;
		vib.wRightMotorSpeed = speedRight;
		XInputSetState(gamepad, &vib);
	}

	void CInputManager::_Test()
	{
		short stickLeft = GetLeftStickX(0);
		USHORT left = 0;
		USHORT right = 0;
		if (stickLeft < 0)
			left = -stickLeft * 2;
		else
			right = stickLeft * 2;
		SetMotorSpeed(0, left, right);

		if (IsButtonDown(0, Buttons::A))
			DebugManager->Debug(L"A", L"A");
		if (IsButtonDown(0, Buttons::B))
			DebugManager->Debug(L"B", L"B");
		if (IsButtonDown(0, Buttons::Back))
			DebugManager->Debug(L"Back", L"Back");
		if (IsButtonDown(0, Buttons::Down))
			DebugManager->Debug(L"Down", L"Down");
		if (IsButtonDown(0, Buttons::LB))
			DebugManager->Debug(L"LB", L"LB");
		if (IsButtonDown(0, Buttons::Left))
			DebugManager->Debug(L"Left", L"Left");
		if (IsButtonDown(0, Buttons::LeftStick))
			DebugManager->Debug(L"LeftStick", L"LeftStick");
		if (IsButtonDown(0, Buttons::RB))
			DebugManager->Debug(L"RB", L"RB");
		if (IsButtonDown(0, Buttons::Right))
			DebugManager->Debug(L"Right", L"Right");
		if (IsButtonDown(0, Buttons::RightStick))
			DebugManager->Debug(L"RightStick", L"RightStick");
		if (IsButtonDown(0, Buttons::Start))
			DebugManager->Debug(L"Start", L"Start");
		if (IsButtonDown(0, Buttons::Up))
			DebugManager->Debug(L"Up", L"Up");
		if (IsButtonDown(0, Buttons::X))
			DebugManager->Debug(L"X", L"X");
		if (IsButtonDown(0, Buttons::Y))
			DebugManager->Debug(L"Y", L"Y");

		DebugManager->Debug(GetLeftTrigger(0), L"left trigger");
		DebugManager->Debug(GetRightTrigger(0), L"right trigger");
		DebugManager->Debug(GetLeftStickX(0), L"left stick x");
		DebugManager->Debug(GetLeftStickY(0), L"left stick y");
		DebugManager->Debug(GetRightStickX(0), L"right stick x");
		DebugManager->Debug(GetRightStickY(0), L"right stick y");
		DebugManager->Debug(GetLeftTriggerDelta(0), L"left trigger Delta");
		DebugManager->Debug(GetRightTriggerDelta(0), L"right trigger Delta");
		DebugManager->Debug(GetLeftStickXDelta(0), L"left stick x Delta");
		DebugManager->Debug(GetLeftStickYDelta(0), L"left stick y Delta");
		DebugManager->Debug(GetRightStickXDelta(0), L"right stick x Delta");
		DebugManager->Debug(GetRightStickYDelta(0), L"right stick y Delta");

		if (IsButtonPressed(0, Buttons::A))
			printf("A pressed\n");
		if (IsButtonPressed(0, Buttons::B))
			printf("B pressed\n");
		if (IsButtonPressed(0, Buttons::Back))
			printf("Back pressed\n");
		if (IsButtonPressed(0, Buttons::Down))
			printf("Down pressed\n");
		if (IsButtonPressed(0, Buttons::LB))
			printf("LB pressed\n");
		if (IsButtonPressed(0, Buttons::Left))
			printf("Left pressed\n");
		if (IsButtonPressed(0, Buttons::LeftStick))
			printf("LeftStick pressed\n");
		if (IsButtonPressed(0, Buttons::RB))
			printf("RB pressed\n");
		if (IsButtonPressed(0, Buttons::Right))
			printf("Right pressed\n");
		if (IsButtonPressed(0, Buttons::RightStick))
			printf("RightStick pressed\n");
		if (IsButtonPressed(0, Buttons::Start))
			printf("Start pressed\n");
		if (IsButtonPressed(0, Buttons::Up))
			printf("Up pressed\n");
		if (IsButtonPressed(0, Buttons::X))
			printf("X pressed\n");
		if (IsButtonPressed(0, Buttons::Y))
			printf("Y pressed\n");

		if (IsButtonReleased(0, Buttons::A))
			printf("A released\n");
		if (IsButtonReleased(0, Buttons::B))
			printf("B released\n");
		if (IsButtonReleased(0, Buttons::Back))
			printf("Back released\n");
		if (IsButtonReleased(0, Buttons::Down))
			printf("Down released\n");
		if (IsButtonReleased(0, Buttons::LB))
			printf("LB released\n");
		if (IsButtonReleased(0, Buttons::Left))
			printf("Left released\n");
		if (IsButtonReleased(0, Buttons::LeftStick))
			printf("LeftStick released\n");
		if (IsButtonReleased(0, Buttons::RB))
			printf("RB released\n");
		if (IsButtonReleased(0, Buttons::Right))
			printf("Right released\n");
		if (IsButtonReleased(0, Buttons::RightStick))
			printf("RightStick released\n");
		if (IsButtonReleased(0, Buttons::Start))
			printf("Start released\n");
		if (IsButtonReleased(0, Buttons::Up))
			printf("Up released\n");
		if (IsButtonReleased(0, Buttons::X))
			printf("X released\n");
		if (IsButtonReleased(0, Buttons::Y))
			printf("Y released\n");

		if (GamepadConnected())
			printf("gamepad connected\n");
		if (GamepadDisconnected())
			printf("gamepad disconnected\n");

		UINT connectedGamepads = 0;
		for (UINT i = 0; i < XUSER_MAX_COUNT; i++)
			if (IsGamepadActive(i))
				connectedGamepads++;
		DebugManager->Debug((int)connectedGamepads, L"Connected gamepads");
	}

	void CInputManager::Destroy()
	{
		delete[] gamepadStatePrev;
		delete[] gamepadStateCur;
		delete[] curState;
		delete[] prevState;
		delete curMouse;
		delete prevMouse;
		delete this;
	}
}