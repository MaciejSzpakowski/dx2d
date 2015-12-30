#pragma once

namespace Viva
{
	class CInputManager
	{
	private:
		int keyCount;
		bool* curState;
		bool* prevState;
		POINT* curMouse;
		POINT* prevMouse;
		int mouseWheel;
		bool activeGamepads[XUSER_MAX_COUNT];
		XINPUT_STATE* gamepadStatePrev;
		XINPUT_STATE* gamepadStateCur;
		bool gamepadConnected;
		bool gamepadDisconnected;
		UINT acceptGamepads;

		void _CheckGamepads();

	public:
		CInputManager();
		
		void _Activity();
		void _Test();
		void _SetMouseWheel(int _mouseWheel) { mouseWheel = _mouseWheel; }


		// MOUSE
		int GetMouseWheel();
		POINT GetCursorDelta();
		POINT GetCursorClientPos();

		// KEYS (aslo mouse buttons) and KEYBOARD
		bool IsKeyDown(Keys vkey);
		bool IsKeyPressed(Keys vkey);
		bool IsKeyReleased(Keys vkey);
		bool IsAnyKeyDown();
		char GetChar(bool enableShift = true, bool enableCapslock = true);
		//offset, where to start looking
		bool IsCapslockActive();
		char GetKey(int offset);
		void ResetKey(int vkey);

		//how many gamepads listen to
		//keeps this at minimum for efficiency
		//max count is XUSER_MAX_COUNT (currently 4)
		void SetGamepadsCount(UINT count) { acceptGamepads = count; }
		UINT GetGamepadsCount() const { return acceptGamepads; }
		bool IsAnyButtonDown(UINT gamepad);
		bool IsButtonDown(UINT gamepad, Buttons button);
		bool IsButtonPressed(UINT gamepad, Buttons button);
		bool IsButtonReleased(UINT gamepad, Buttons button);
		BYTE GetLeftTrigger(UINT gamepad);
		BYTE GetRightTrigger(UINT gamepad);
		SHORT GetLeftStickX(UINT gamepad);
		SHORT GetLeftStickY(UINT gamepad);
		SHORT GetRightStickX(UINT gamepad);
		SHORT GetRightStickY(UINT gamepad);
		BYTE GetLeftTriggerDelta(UINT gamepad);
		BYTE GetRightTriggerDelta(UINT gamepad);
		SHORT GetLeftStickXDelta(UINT gamepad);
		SHORT GetLeftStickYDelta(UINT gamepad);
		SHORT GetRightStickXDelta(UINT gamepad);
		SHORT GetRightStickYDelta(UINT gamepad);
		void SetMotorSpeed(UINT gamepad, USHORT speedLeft, USHORT speedRight);

		//technically: was gamepad connected during last read
		//generally: is that gamepad connected
		bool IsGamepadActive(UINT gamepad);

		//was a new gamepad detected between last and current read
		bool GamepadConnected();

		//was a gamepad lost between last and current read
		bool GamepadDisconnected();
		void Destroy();
	};
}
