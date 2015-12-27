#pragma once

namespace Viva
{
	class CPolygon : public CDynamic
	{
	public:
		float zRadius; //distance from origin to the furthest vertex
		XMMATRIX zGetScaleMatrix() override;
		void zDraw() override;

		CPolygon() {}
		CPolygon(const vector<XMFLOAT2>& points);

		//make exact (but independent) copy and add to draw manager
		//stuff that is not copied: children (since they can have only one parent)
		virtual CPolygon* Clone();
		void Destroy();
	};

	class CRectangle : public CPolygon
	{
	public:
		XMMATRIX zGetScaleMatrix() override;

		XMFLOAT2 Scale;
		CRectangle(const Size& size);
	};

	class CCircle : public CPolygon
	{
	public:
		XMMATRIX zGetScaleMatrix() override;

		float Radius;
		CCircle(float radius, unsigned char resolution);
	};
}
