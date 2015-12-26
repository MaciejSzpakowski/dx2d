#pragma once

namespace Viva
{
	class CPolygon : public CDynamic
	{
	public:
		float zRadius; //distance from origin to the furthest vertex
		XMMATRIX zGetScaleMatrix() override;
		void zDraw() override;

		CPolygon();
		CPolygon(XMFLOAT2 points[], int n);

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
		CRectangle(float scalex, float scaley);
	};

	class CCircle : public CPolygon
	{
	public:
		XMMATRIX zGetScaleMatrix() override;

		float Radius;
		CCircle(float radius, unsigned char resolution);
	};
}
