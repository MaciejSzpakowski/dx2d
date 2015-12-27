#include "Viva.h"

namespace Viva
{
	CAnimation::CAnimation(LPCWSTR file, int x, int y) : CSprite(file)
	{
		zFrameCount = x*y;
		Start = 0;
		Finish = zFrameCount - 1;
		Frame = 0;
		Speed = 1;
		zFrameChanged = true;
		zIndicator = 0;
		zUvTable.reserve(zFrameCount);
		for (int i = 0; i < y; i++)
			for (int j = 0; j < x; j++)
				zUvTable.push_back(Rect(1.0f / x*j, 1.0f / y*i, 1.0f /
					x*(j + 1), 1.0f / y*(i + 1)));
		UV = zUvTable[Frame];
	}

	void CAnimation::zPlay()
	{
		if (Speed != 0)
		{
			zIndicator += Speed * Core->GetFrameTime();
			zFrameChanged = false;
			if (zIndicator > 1)
			{
				zIndicator = 0;
				zFrameChanged = true;
				NextFrame();
			}
			else if (zIndicator < 0)
			{
				zIndicator = 1;
				zFrameChanged = true;
				PreviousFrame();
			}
		}
		UV = zUvTable[Frame];
	}

	void CAnimation::SetOrder(int order[])
	{
		vector<Rect> newUvtable;
		newUvtable.reserve(zFrameCount);
		for (int i = 0; i < zFrameCount; i++)
			newUvtable[i] = zUvTable[order[i]];
		zUvTable = newUvtable;
	}

	int CAnimation::GetFrameCount()
	{
		return zFrameCount;
	}

	void CAnimation::NextFrame()
	{
		Frame++;
		if (Frame > Finish)
			Frame = Start;
	}

	void CAnimation::PreviousFrame()
	{
		Frame--;
		if (Frame < Start)
			Frame = Finish;
	}

	bool CAnimation::FrameChanged()
	{
		return zFrameChanged;
	}
}