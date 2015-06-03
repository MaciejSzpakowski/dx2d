#include "Private.h"

namespace dx2d
{
	CCamera::CCamera()
	{
		//Create the buffer to send to the cbuffer in effect file
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(cbPerObject);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;

		Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

		//Camera information
		position = XMVectorSet( 0.0f, 0.0f, -0.5f, 0.0f );
		target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//Set the View matrix
		view = DirectX::XMMatrixLookAtLH(position, target, up);

		//Set the Projection matrix
		RECT rect;
		GetWindowRect(Global->GetWindowHandle(), &rect);
		float aspectRatio = (float)800/600;
		proj = DirectX::XMMatrixPerspectiveFovLH(0.4f*3.14f, aspectRatio, 0.1f, 100.0f);
	}

	void CCamera::Destroy()
	{
		cbPerObjectBuffer->Release();
	}
}