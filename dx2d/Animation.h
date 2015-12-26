#pragma once

namespace Viva
{
	class CAnimation : public CSprite
	{
	public:
		double zLastFrameChangeTime;
		int zFrameCount;
		vector<Rect> zUvTable;
		double zIndicator;
		bool zFrameChanged;
		//plays animation
		void zPlay() override;

		CAnimation(LPCWSTR file, int x, int y);
		bool FrameChanged(); //true if frame changed in prev game frame
							 //set frames order, size of the array should be the same as frame count
		void SetOrder(int order[]);
		int GetFrameCount();
		//next frame and loop
		void NextFrame();
		//prev frame and loop
		void PreviousFrame();

		int Start; //first frame of currently played animation
		int Finish; //last frame of currently played animation
		int Frame; //current frame displayed
		double Speed; //in frames per second
	};
}
