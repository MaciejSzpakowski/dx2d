#pragma once

namespace Viva
{
	class Polygon : public CDynamic
	{
	protected:
		Polygon() {}
	private:
		float span; //distance from origin to the furthest vertex

	public:
		Polygon(const vector<XMFLOAT2>& points);

		XMMATRIX _GetScaleMatrix() override;
		void _Draw() override;

		float GetSpan() const { return span; }

		//make exact (but independent) copy and add to draw manager
		//stuff that is not copied: children (since they can have only one parent)
		virtual Polygon* Clone();

		void Destroy() override;
	};

	class Rectangle : public Polygon
	{
	private:
		XMFLOAT2 scale;

	public:
		Rectangle(const Size& size);

		XMFLOAT2 GetScale() const { return scale; }
		void SetScale(const XMFLOAT2& _scale) { scale = _scale; }

		XMMATRIX _GetScaleMatrix() override;
	};

	class Circle : public Polygon
	{
	private:
		float radius;

	public:
		Circle(float _radius, size_t resolution);

		float GetRadius() const { return radius; }
		void SetRadius(float _radius) { radius = _radius; }
		
		XMMATRIX _GetScaleMatrix() override;
	};
}
