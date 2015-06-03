#include "Private.h"

namespace dx2d
{
	Core* Global;

	void Render(Core* d3d)
	{
		d3d->context->ClearRenderTargetView(d3d->backBuffer, d3d->backBufferColor);

		Camera->world = DirectX::XMMatrixIdentity();
		Camera->worldViewProj = Camera->world * Camera->view * Camera->proj;
		Camera->cbPerObj.WorldViewProj = DirectX::XMMatrixTranspose(Camera->worldViewProj);
		Context->UpdateSubresource(Camera->cbPerObjectBuffer, 0, NULL, &Camera->cbPerObj, 0, 0);
		Context->VSSetConstantBuffers(0, 1, &Camera->cbPerObjectBuffer);

		DrawManager->DrawAll();
		d3d->swapChain->Present(0, 0);
	}
}
