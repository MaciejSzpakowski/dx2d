#include "Private.h"

namespace dx2d
{
	Core* Global;

	void Render(Core* d3d)
	{
		d3d->context->ClearRenderTargetView(d3d->backBuffer, d3d->backBufferColor);
		DrawManager->DrawAll();
		d3d->swapChain->Present(0, 0);
	}
}