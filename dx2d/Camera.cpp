#include "Private.h"

namespace dx2d
{
	CCamera::CCamera()
	{
		//Camera information
		XMVECTOR position = XMVectorSet( 0.0f, 0.0f, -20.0f, 0.0f );
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

	XMMATRIX CCamera::GetScaleMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}

	void CCamera::Destroy()
	{
	}
}