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

	enum class TEX_FILTER { POINT, LINEAR };

	struct Color
	{
		float r, g, b, a;
		Color() {}
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

	//used to cache texture loaded from file
	class CTexture
	{
	public:
		bool zCached;
		Size size;
		wstring zName;
		ID3D11ShaderResourceView* zShaderResource;

		CTexture(bool cached, Size _size, const wchar_t* name,	ID3D11ShaderResourceView* shaderResource) :
			zCached(cached), size(_size), zName(name),
			zShaderResource(shaderResource) {}

		size_t GetWidth() const { return size.width; }

		size_t GetHeight() const { return size.height; }

		wstring GetName() const { return zName; }

		void Destroy()
		{
			zShaderResource->Release();
			delete this;
		}
	};	

	class CBitmapFont
	{
	public:
		vector<Rect> zChars;
		CTexture* zTexture;
		ID3D11ShaderResourceView* zShaderResource;

		CBitmapFont(LPCWSTR file, vector<Rect> _chars);
		CBitmapFont(LPCWSTR file, int width, int height, int charsPerRow);
		CBitmapFont(CTexture* texture, vector<Rect> _chars);
		CBitmapFont(CTexture* texture, int width, int height, int charsPerRow);
		CTexture* GetTexture() { return zTexture; }
		void Destroy();
	};

	enum class TextHorAlign { Left, Center, Right };
	enum class TextVerAlign { Top, Center, Bottom };	
}