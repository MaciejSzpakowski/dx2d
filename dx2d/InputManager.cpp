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
		zGamepadStatePrev = new XINPUT_STATE[XUSER_MAX_COUNT];
		zGamepadStateCur = new XINPUT_STATE[XUSER_MAX_COUNT];
		ZeroMemory(zCurState, sizeof(bool)*zKeyCount);
		ZeroMemory(zPrevState, sizeof(bool)*zKeyCount);
		ZeroMemory(zGamepadStatePrev, sizeof(XINPUT_STATE)*XUSER_MAX_COUNT);
		ZeroMemory(zGamepadStateCur, sizeof(XINPUT_STATE)*XUSER_MAX_COUNT);
		AcceptGamepads = 0;
		zActiveGamepads[0] = false;
		zActiveGamepads[1] = false;
		zActiveGamepads[2] = false;
		zActiveGamepads[3] = false;
		zGamepadConnected = false;
		zGamepadDisconnected = false;
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

		zCheckGamepads();

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

	void CInput::zCheckGamepads()
	{
		zGamepadConnected = false;
		zGamepadDisconnected = false;
		if (AcceptGamepads > XUSER_MAX_COUNT)
			AcceptGamepads = XUSER_MAX_COUNT;
		for (UINT i = 0; i < AcceptGamepads; i++)
		{
			auto temp = zGamepadStatePrev;
			zGamepadStatePrev = zGamepadStateCur;
			zGamepadStateCur = temp;
			ZeroMemory(zGamepadStateCur + i, sizeof(XINPUT_STATE));
			DWORD dwResult = XInputGetState(i, zGamepadStateCur + i);

			if (dwResult == ERROR_SUCCESS)
			{
				if (zActiveGamepads[i] == false)
					zGamepadConnected = true;
				zActiveGamepads[i] = true;
			}
			else
			{
				if (zActiveGamepads[i] == true)
					zGamepadDisconnected = true;
				zActiveGamepads[i] = false;
			}
		}
	}

	bool CInput::IsButtonDown(UINT gamepad, int button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return (zGamepadStateCur[gamepad].Gamepad.wButtons & button) && true;
	}

	bool CInput::IsButtonPressed(UINT gamepad, int button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return (zGamepadStateCur[gamepad].Gamepad.wButtons & button) && 
			!(zGamepadStatePrev[gamepad].Gamepad.wButtons & button);
	}

	bool CInput::IsButtonReleased(UINT gamepad, int button)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return !(zGamepadStateCur[gamepad].Gamepad.wButtons & button) &&
			(zGamepadStatePrev[gamepad].Gamepad.wButtons & button);
	}

	bool CInput::IsGamepadActive(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return false;
		return zActiveGamepads[gamepad];
	}

	bool CInput::GamepadConnected()
	{
		return zGamepadConnected;
	}

	bool CInput::GamepadDisconnected()
	{
		return zGamepadDisconnected;
	}

	BYTE CInput::GetLeftTrigger(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.bLeftTrigger;
	}

	BYTE CInput::GetRightTrigger(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.bRightTrigger;
	}

	SHORT CInput::GetLeftStickX(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbLX;
	}

	SHORT CInput::GetLeftStickY(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbLY;
	}

	SHORT CInput::GetRightStickX(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbRX;
	}

	SHORT CInput::GetRightStickY(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbRY;
	}

	BYTE CInput::GetLeftTriggerDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.bLeftTrigger - 
			zGamepadStatePrev[gamepad].Gamepad.bLeftTrigger;
	}

	BYTE CInput::GetRightTriggerDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.bRightTrigger -
			zGamepadStatePrev[gamepad].Gamepad.bRightTrigger;
	}

	SHORT CInput::GetLeftStickXDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbLX -
			zGamepadStatePrev[gamepad].Gamepad.sThumbLX;
	}

	SHORT CInput::GetLeftStickYDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbLY -
			zGamepadStatePrev[gamepad].Gamepad.sThumbLY;
	}

	SHORT CInput::GetRightStickXDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbRX -
			zGamepadStatePrev[gamepad].Gamepad.sThumbRX;
	}

	SHORT CInput::GetRightStickYDelta(UINT gamepad)
	{
		if (gamepad > XUSER_MAX_COUNT - 1)
			return 0;
		return zGamepadStateCur[gamepad].Gamepad.sThumbRY -
			zGamepadStatePrev[gamepad].Gamepad.sThumbRY;
	}

	void CInput::SetMotorSpeed(UINT gamepad, USHORT speedLeft, USHORT speedRight)
	{
		XINPUT_VIBRATION vib;
		vib.wLeftMotorSpeed = speedLeft;
		vib.wRightMotorSpeed = speedRight;
		XInputSetState(gamepad, &vib);
	}

	void CInput::zTest()
	{
		short stickLeft = Input->GetLeftStickX(0);
		USHORT left = 0;		
		USHORT right = 0;
		if (stickLeft < 0)
			left = -stickLeft * 2;
		else
			right = stickLeft * 2;
		Input->SetMotorSpeed(0, left, right);

		if (Input->IsButtonDown(0, Button.A))
			DebugManager->Debug(L"A", L"A");
		if (Input->IsButtonDown(0, Button.B))
			DebugManager->Debug(L"B", L"B");
		if (Input->IsButtonDown(0, Button.Back))
			DebugManager->Debug(L"Back", L"Back");
		if (Input->IsButtonDown(0, Button.Down))
			DebugManager->Debug(L"Down", L"Down");
		if (Input->IsButtonDown(0, Button.LB))
			DebugManager->Debug(L"LB", L"LB");
		if (Input->IsButtonDown(0, Button.Left))
			DebugManager->Debug(L"Left", L"Left");
		if (Input->IsButtonDown(0, Button.LeftStick))
			DebugManager->Debug(L"LeftStick", L"LeftStick");
		if (Input->IsButtonDown(0, Button.RB))
			DebugManager->Debug(L"RB", L"RB");
		if (Input->IsButtonDown(0, Button.Right))
			DebugManager->Debug(L"Right", L"Right");
		if (Input->IsButtonDown(0, Button.RightStick))
			DebugManager->Debug(L"RightStick", L"RightStick");
		if (Input->IsButtonDown(0, Button.Start))
			DebugManager->Debug(L"Start", L"Start");
		if (Input->IsButtonDown(0, Button.Up))
			DebugManager->Debug(L"Up", L"Up");
		if (Input->IsButtonDown(0, Button.X))
			DebugManager->Debug(L"X", L"X");
		if (Input->IsButtonDown(0, Button.Y))
			DebugManager->Debug(L"Y", L"Y");

		DebugManager->Debug(Input->GetLeftTrigger(0), L"left trigger");
		DebugManager->Debug(Input->GetRightTrigger(0), L"right trigger");
		DebugManager->Debug(Input->GetLeftStickX(0), L"left stick x");
		DebugManager->Debug(Input->GetLeftStickY(0), L"left stick y");
		DebugManager->Debug(Input->GetRightStickX(0), L"right stick x");
		DebugManager->Debug(Input->GetRightStickY(0), L"right stick y");
		DebugManager->Debug(Input->GetLeftTriggerDelta(0), L"left trigger Delta");
		DebugManager->Debug(Input->GetRightTriggerDelta(0), L"right trigger Delta");
		DebugManager->Debug(Input->GetLeftStickXDelta(0), L"left stick x Delta");
		DebugManager->Debug(Input->GetLeftStickYDelta(0), L"left stick y Delta");
		DebugManager->Debug(Input->GetRightStickXDelta(0), L"right stick x Delta");
		DebugManager->Debug(Input->GetRightStickYDelta(0), L"right stick y Delta");

		if (Input->IsButtonPressed(0, Button.A))
			printf("A pressed\n");
		if (Input->IsButtonPressed(0, Button.B))
			printf("B pressed\n");
		if (Input->IsButtonPressed(0, Button.Back))
			printf("Back pressed\n");
		if (Input->IsButtonPressed(0, Button.Down))
			printf("Down pressed\n");
		if (Input->IsButtonPressed(0, Button.LB))
			printf("LB pressed\n");
		if (Input->IsButtonPressed(0, Button.Left))
			printf("Left pressed\n");
		if (Input->IsButtonPressed(0, Button.LeftStick))
			printf("LeftStick pressed\n");
		if (Input->IsButtonPressed(0, Button.RB))
			printf("RB pressed\n");
		if (Input->IsButtonPressed(0, Button.Right))
			printf("Right pressed\n");
		if (Input->IsButtonPressed(0, Button.RightStick))
			printf("RightStick pressed\n");
		if (Input->IsButtonPressed(0, Button.Start))
			printf("Start pressed\n");
		if (Input->IsButtonPressed(0, Button.Up))
			printf("Up pressed\n");
		if (Input->IsButtonPressed(0, Button.X))
			printf("X pressed\n");
		if (Input->IsButtonPressed(0, Button.Y))
			printf("Y pressed\n");

		if (Input->IsButtonReleased(0, Button.A))
			printf("A released\n");
		if (Input->IsButtonReleased(0, Button.B))
			printf("B released\n");
		if (Input->IsButtonReleased(0, Button.Back))
			printf("Back released\n");
		if (Input->IsButtonReleased(0, Button.Down))
			printf("Down released\n");
		if (Input->IsButtonReleased(0, Button.LB))
			printf("LB released\n");
		if (Input->IsButtonReleased(0, Button.Left))
			printf("Left released\n");
		if (Input->IsButtonReleased(0, Button.LeftStick))
			printf("LeftStick released\n");
		if (Input->IsButtonReleased(0, Button.RB))
			printf("RB released\n");
		if (Input->IsButtonReleased(0, Button.Right))
			printf("Right released\n");
		if (Input->IsButtonReleased(0, Button.RightStick))
			printf("RightStick released\n");
		if (Input->IsButtonReleased(0, Button.Start))
			printf("Start released\n");
		if (Input->IsButtonReleased(0, Button.Up))
			printf("Up released\n");
		if (Input->IsButtonReleased(0, Button.X))
			printf("X released\n");
		if (Input->IsButtonReleased(0, Button.Y))
			printf("Y released\n");

		if (Input->GamepadConnected())
			printf("gamepad connected\n");
		if (Input->GamepadDisconnected())
			printf("gamepad disconnected\n");

		UINT connectedGamepads = 0;
		for (UINT i = 0; i < XUSER_MAX_COUNT; i++)
			if (Input->IsGamepadActive(i))
				connectedGamepads++;
		DebugManager->Debug((int)connectedGamepads, L"Connected gamepads");
	}

	void CInput::Destroy()
	{
		delete[] zGamepadStatePrev;
		delete[] zGamepadStateCur;
		delete[] zCurState;
		delete[] zPrevState;
		delete zCurMouse;
		delete zPrevMouse;
		delete this;
	}
}