#include "Private.h"

namespace dx2d
{
	Core* Global;
	CKey Key;

	//friend of the core
	void Render(Core* d3d)
	{
		d3d->camera->CamTransform();
		d3d->context->ClearRenderTargetView(d3d->backBuffer, d3d->backBufferColor);
		d3d->context->ClearDepthStencilView(d3d->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		d3d->drawManager->DrawAll();
		d3d->swapChain->Present(0, 0);
		d3d->inputManager->Activity();
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
