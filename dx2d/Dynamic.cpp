#include "Private.h"

namespace dx2d
{
	XMFLOAT3 Dynamic::GetPosition()
	{
		XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, position);
		return result;
	}

	void Dynamic::SetPosition(XMFLOAT3 _position)
	{
		position = DirectX::XMLoadFloat3(&_position);
	}
}