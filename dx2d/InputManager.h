#pragma once

namespace Viva
{
	class CInputManager
	{
	private:
        HWND hwnd;
		int keyCount;
		bool* curState;
		bool* prevState;
		POINT curMouse;
        POINT curDelta;
		int mouseWheel;
		bool activeGamepads[XUSER_MAX_COUNT];
		XINPUT_STATE* gamepadStatePrev;
		XINPUT_STATE* gamepadStateCur;
		bool gamepadConnected;
		bool gamepadDisconnected;
		UINT acceptGamepads;
        bool cursorVisible;

		void _CheckGamepads();

	public:
		CInputManager();
		
		void _Activity();
		void _Test();
		void _SetMouseWheel(int _mouseWheel) { mouseWheel = _mouseWheel; }
        void _SetCursorDelta(int x, int y);

		// MOUSE
		int GetMouseWheel();
		POINT GetCursorDelta() const;
		POINT GetCursorClientPos() const;

        // sets Windows cursor position in desktop cooridnates
        void SetCursorPos(int x, int y);

        // in desktop cooridnates
        POINT GetCursorPos() const;
        void ShowCursor(bool visible);

        bool IsCursorVisible() const;

		// KEYS (aslo mouse buttons) and KEYBOARD
		bool IsKeyDown(Keys vkey) const;
		bool IsKeyPressed(Keys vkey) const;
		bool IsKeyReleased(Keys vkey) const;
		bool IsAnyKeyDown() const;
		// Returns key typed including all ASCII chars between 32 and 127 and \n \t \b
		// Shift and capslock for upper case is included by default but can be disabled.
		// If not applicable char was typed, the function returns 0.
		char GetChar(bool enableShift = true, bool enableCapslock = true) const;
		//offset, where to start looking
		bool IsCapslockActive() const;
		char GetKey(int offset) const;
		void ResetKey(int vkey);

		//how many gamepads listen to
		//keeps this at minimum for efficiency
		//max count is XUSER_MAX_COUNT (currently 4)
		void SetGamepadsCount(UINT count) { acceptGamepads = count; }
		UINT GetGamepadsCount() const { return acceptGamepads; }
		bool IsAnyButtonDown(UINT gamepad) const;
		bool IsButtonDown(UINT gamepad, Buttons button) const;
		bool IsButtonPressed(UINT gamepad, Buttons button) const;
		bool IsButtonReleased(UINT gamepad, Buttons button) const;
		BYTE GetLeftTrigger(UINT gamepad) const;
		BYTE GetRightTrigger(UINT gamepad) const;
		SHORT GetLeftStickX(UINT gamepad) const;
		SHORT GetLeftStickY(UINT gamepad) const;
		SHORT GetRightStickX(UINT gamepad) const;
		SHORT GetRightStickY(UINT gamepad) const;
		BYTE GetLeftTriggerDelta(UINT gamepad) const;
		BYTE GetRightTriggerDelta(UINT gamepad) const;
		SHORT GetLeftStickXDelta(UINT gamepad) const;
		SHORT GetLeftStickYDelta(UINT gamepad) const;
		SHORT GetRightStickXDelta(UINT gamepad) const;
		SHORT GetRightStickYDelta(UINT gamepad) const;
		void SetMotorSpeed(UINT gamepad, USHORT speedLeft, USHORT speedRight);

		//technically: was gamepad connected during last read
		//generally: is that gamepad connected
		bool IsGamepadActive(UINT gamepad) const;

		//was a new gamepad detected between last and current read
		bool GamepadConnected() const;

		//was a gamepad lost between last and current read
		bool GamepadDisconnected() const;
		void Destroy();
	};
}
