#pragma once

namespace Viva
{
#define CHECKHR() Functions::Checkhr(__FILE__,__LINE__, hr)

	struct Vertex
	{
		float X, Y, Z;
		float R, G, B;
		float U, V;
		Vertex() {}
		Vertex(float x, float y, float z, float r, float g, float b, float u, float v)
			: X(x), Y(y), Z(z), R(r), G(g), B(b), U(u), V(v) {}
	};

	enum class TextureFilter { Point, Linear };

	// some functions use color where each component is between 0 and 1
	struct Color
	{
		float r, g, b, a;
		Color() :r(0), g(0), b(0), a(0) {}
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
		Color& operator = (const XMFLOAT4& other)
		{
			r = other.x;
			g = other.y;
			b = other.z;
			a = other.w;
			return *this;
		}
	};

	// some functions use pixel (which is the same as color) where each component is between 0 and 255
	struct Pixel
	{
		byte r, g, b, a;
		Pixel() :r(0), g(0), b(0), a(0) {}
		Pixel(byte _r, byte _g, byte _b, byte _a) : r(_r), g(_g), b(_b), a(_a) {}
	};

	struct Point
	{
		float x;
		float y;

		Point() :x(0), y(0) {}
		Point(float _x, float _y) :x(_x), y(_y) {}
	};

	struct Size
	{
		size_t width;
		size_t height;

		Size() :width(0), height(0) {}
		Size(size_t _width, size_t _height) :width(_width), height(_height) {}
	};

	class Resource
	{
	protected:
		wstring name;
		bool cached;
	public:
		Resource(const wstring& _name) :name(_name), cached(false) {}

		bool IsCached() const { return cached; }

		void _SetCached(bool _cached) { cached = _cached; }

		wstring GetName() const { return name; }
	};

	struct Rect
	{
		float left;
		float top;
		float right;
		float bottom;
		Rect() {}
		Rect(float _left, float _top, float _right, float _bottom) :
			left(_left), top(_top), right(_right), bottom(_bottom) {}
	};

	struct Event
	{
		double startTime;
		double delay;
		double lifeTime;
		double tick;
		double lastPulse;
		wstring Name;
		std::function<int()> Activity;
	};
	
	enum class HorizontalAlignment { Left, Center, Right };
	enum class VerticalAlignment { Top, Center, Bottom };
}
