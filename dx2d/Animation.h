#pragma once

namespace Viva
{
	class Animation : public Sprite
	{
	private:
		int frameCount;
		vector<Rect> uvTable;
		double indicator;
		bool frameChanged;
		int begin;                 // first frame of currently played animation
		int end;                   // last frame of currently played animation
		int currentFrame;          // current frame displayed
		double speed;              // in frames per second

	public:
		// Create animation.
		// columns/rows: how to split the image to create frames
		Animation(const wchar_t* filename, int columns, int rows);

		// Create animation.
		// _uvTable: where are individual frames
		Animation(const wchar_t* filename, const vector<Rect>& _uvTable);

		//plays animation
		void _Play() override;

		// True if frame changed in previous game frame.		
		bool FrameChanged() { return frameChanged; }

		// Set frames order. Size of the array should be the same as frame count.
		void SetOrder(const int order[]);

		int GetFrameCount() const { return frameCount; }

		// Advances animation to the next frame.
		// If the current frame is the last then this method sets current frame to 0.
		void NextFrame()
		{
			currentFrame++;
			if (currentFrame > end)
				currentFrame = begin;
		}

		// Goes back in the animation.
		// If the current frame is the first then this method sets current frame to the last one.
		void PreviousFrame()
		{
			currentFrame--;
			if (currentFrame < begin)
				currentFrame = end;
		}

		int GetBegin() const { return begin; }
		void SetBegin(int _begin) { begin = _begin; }
		int GetEnd() const { return end; }
		void SetEnd(int _end) { end = _end; }
		double GetSpeed() const { return speed; }
		void SetSpeed(double _speed) { speed = _speed; }
		int GetCurrentFrame() const { return currentFrame; }
		void SetCurrentFrame(int _currentFrame) { currentFrame = _currentFrame; }
	};
}
