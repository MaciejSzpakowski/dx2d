#include "Viva.h"

namespace Viva
{
	Animation::Animation(const wchar_t* filename, int columns, int rows) : Sprite(filename)
	{
		frameCount = columns*rows;
		begin = 0;
		end = frameCount - 1;
		currentFrame = 0;
		speed = 1;
		frameChanged = true;
		indicator = 0;
		uvTable.reserve(frameCount);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				uvTable.push_back(Rect(1.0f / columns*j, 1.0f / rows*i, 1.0f /
					columns*(j + 1), 1.0f / rows*(i + 1)));
		uv = uvTable[0];
	}

	Animation::Animation(const wchar_t* filename, const vector<Rect>& _uvTable)
		:Sprite(filename)
	{
		uvTable = _uvTable;
		frameCount = (int)_uvTable.size();
		begin = 0;
		end = frameCount - 1;
		currentFrame = 0;
		speed = 1;
		frameChanged = true;
		indicator = 0;
		uv = uvTable[0];
	}

	void Animation::_Play()
	{
		if (speed != 0)
		{
			indicator += speed * Core->GetFrameTime();
			frameChanged = false;
			if (indicator > 1)
			{
				indicator = 0;
				frameChanged = true;
				NextFrame();
			}
			else if (indicator < 0)
			{
				indicator = 1;
				frameChanged = true;
				PreviousFrame();
			}
		}
		uv = uvTable[currentFrame];
	}

	void Animation::SetOrder(const int order[])
	{
		vector<Rect> newUvtable;
		newUvtable.reserve(frameCount);
		for (int i = 0; i < frameCount; i++)
			newUvtable[i] = uvTable[order[i]];
		uvTable = newUvtable;
	}
}