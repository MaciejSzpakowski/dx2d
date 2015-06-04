#include "Private.h"

namespace dx2d
{
	Core* Global;
	CKey Key;

	void Render(Core* d3d)
	{
		Camera->CamTransform();
		d3d->context->ClearRenderTargetView(d3d->backBuffer, d3d->backBufferColor);
		DrawManager->DrawAll();
		d3d->swapChain->Present(0, 0);
		InputManager->Activity();
	}

	Core* NewCore(int sizex, int sizey, std::function<void()> worker)
	{
		Core* core = new Core(sizex, sizey, worker);
		return core;
	}

	void AddFloat3(XMFLOAT3* src, XMFLOAT3* dst)
	{
		dst->x += src->x;
		dst->y += src->y;
		dst->z += src->z;
	}
}
