#include "Viva.h"

namespace Viva
{
	using namespace DirectX;

	void IntActivity()
	{
		InputManager->_Activity();
		EventManager->_Activity();
		Core->GetCamera()->_CamTransform();
		DrawManager->zDrawAll();
		Core->zUpdateGameTime();
	}

	namespace Functions
	{
		void InitViva(Size clientSize, std::function<void()> worker, int style)
		{
			// for now core assigns itself to global variable
			new CCore(clientSize, worker, style);
			DrawManager->zInit();
		}

		void Checkhr(LPCSTR file, int line, HRESULT hr)
		{
			if (hr == 0)
				return;
			char str[128];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0,
				hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
				str, 128, 0);
			std::stringstream message;
			message << file << " line: " << line << "\n" << str;

			throw std::runtime_error(message.str().c_str());
		}
	}

	namespace Random
	{
		double RndDouble()
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_real_distribution<> dis(0, 1);
			return dis(gen);
		}

		int RndInt(int min, int max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(min, max);
			return dis(gen);
		}
	}

	namespace Collision
	{
		//the fastest possible collision check,
		//make aligned square region from zRadius
		//check if difference between x or y (position)
		//is more than the sum od radii
		bool CheckRegion(Polygon* p1, Polygon* p2)
		{
			float radi = p1->GetSpan() + p2->GetSpan();
			XMFLOAT3 pos1 = p1->GetPosition();
			XMFLOAT3 pos2 = p2->GetPosition();
			return !(abs(pos1.x - pos2.x) > radi ||
				abs(pos1.y - pos2.y) > radi);
		}

		//another fast collision check
		//circle collision
		bool CheckCircle(Polygon* p1, Polygon* p2)
		{
			float radi = p1->GetSpan() + p2->GetSpan();
			XMVECTOR dist = p1->zPosition - p2->zPosition;
			XMVECTOR len = XMVector2Length(dist);
			return !(XMVectorGetX(len) > radi);
		}

		// checks if line segments |AB| and |CD| intersect
		bool DoLinesIntersect(XMVECTOR A, XMVECTOR B, XMVECTOR C, XMVECTOR D, XMFLOAT3* intersection)
		{
			//P is intersection point of lines (not line segments)
			XMVECTOR P = XMVector2IntersectLine(A, B, C, D);

			//store points
			XMFLOAT2 a, b, c, d, p;
			XMStoreFloat2(&p, P);
			//special cases (parallel and coincide)
			if (p.x == INFINITY)
				return true;
			else if (isnan(p.x))
				return false;
			XMStoreFloat2(&a, A);
			XMStoreFloat2(&b, B);
			XMStoreFloat2(&c, C);
			XMStoreFloat2(&d, D);

			//is p on segments
			bool isOnAB, isOnCD;
			if (a.x == b.x)
				isOnAB = p.y >= min(a.y, b.y) && p.y <= max(a.y, b.y);
			else
				isOnAB = p.x >= min(a.x, b.x) && p.x <= max(a.x, b.x);
			if (c.x == d.x)
				isOnCD = p.y >= min(c.y, d.y) && p.y <= max(c.y, d.y);
			else
				isOnCD = p.x >= min(c.x, d.x) && p.x <= max(c.x, d.x);

			//check if P is on both line segments
			bool result = isOnAB && isOnCD;
			if (result && intersection != nullptr)
				XMStoreFloat3(intersection, P);
			return result;
		}

		bool IsColliding(Circle* c1, Circle* c2)
		{
			XMVECTOR v = XMVectorSubtract(c1->zPosition, c2->zPosition);
			XMVECTOR len = XMVector2Length(v);
			return XMVectorGetX(len) < (c1->GetRadius() + c2->GetRadius());
		}

		bool IsColliding(Circle* c, Rectangle* r)
		{
			return false;
		}

		bool IsColliding(Rectangle* c, Rectangle* r)
		{
			return false;
		}

		bool IsColliding(Circle* c, Polygon* p)
		{
			return false;
		}

		XMFLOAT2 GetProj(XMVECTOR edgeNormal, Polygon* p)
		{
			float min = XMVectorGetX(XMVector2Dot(edgeNormal, p->zTransformedVertices[0]));
			float max = min;
			for (int i = 1; i < p->zVertexCount; i++)
			{
				float dot = XMVectorGetX(XMVector2Dot(edgeNormal, p->zTransformedVertices[i]));
				if (dot < min)
					min = dot;
				else if (dot > max)
					max = dot;
			}
			return XMFLOAT2(min, max);
		}

		bool DoProjsOverlap(XMFLOAT2 proj1, XMFLOAT2 proj2)
		{
			if (proj1.x <= proj2.y && proj1.y >= proj2.x)
				return true;
			if (proj2.x <= proj1.y && proj2.y >= proj1.x)
				return true;
			return false;
		}

		bool IsCollidingSat(Polygon* p1, Polygon* p2)
		{
			if (!CheckRegion(p1, p2))
				return false;

			if (!CheckCircle(p1, p2))
				return false;

			//proj onto all edges normals of p1
			XMVECTOR edgeNormal;
			for (int i = 0; i < p1->zVertexCount; i++)
			{
				if (i == p1->zVertexCount - 1)
					edgeNormal = p1->zTransformedVertices[0] - p1->zTransformedVertices[i];
				else
					edgeNormal = p1->zTransformedVertices[i + 1] - p1->zTransformedVertices[i];
				edgeNormal = XMVector2Orthogonal(edgeNormal);
				XMFLOAT2 proj1 = GetProj(edgeNormal, p1);
				XMFLOAT2 proj2 = GetProj(edgeNormal, p2);
				if (!DoProjsOverlap(proj1, proj2))
					return false;
			}
			//proj onto all edges normals of p2
			for (int i = 0; i < p2->zVertexCount; i++)
			{
				if (i == p2->zVertexCount - 1)
					edgeNormal = p2->zTransformedVertices[0] - p2->zTransformedVertices[i];
				else
					edgeNormal = p2->zTransformedVertices[i + 1] - p2->zTransformedVertices[i];
				edgeNormal = XMVector2Orthogonal(edgeNormal);
				XMFLOAT2 proj1 = GetProj(edgeNormal, p1);
				XMFLOAT2 proj2 = GetProj(edgeNormal, p2);
				if (!DoProjsOverlap(proj1, proj2))
					return false;
			}

			return true;
		}

		bool IsColliding(Polygon* p1, Polygon* p2, XMFLOAT3* pointOfCollision)
		{
			if (!CheckRegion(p1, p2))
				return false;

			if (!CheckCircle(p1, p2))
				return false;

			for (int i = 0; i<p1->zVertexCount - 1; i++)
				for (int j = 0; j < p2->zVertexCount - 1; j++)
				{
					XMVECTOR A = XMVector2Transform(p1->zVertices[i], p1->zWorld);
					XMVECTOR B = XMVector2Transform(p1->zVertices[i + 1], p1->zWorld);
					XMVECTOR C = XMVector2Transform(p2->zVertices[j], p2->zWorld);
					XMVECTOR D = XMVector2Transform(p2->zVertices[j + 1], p2->zWorld);
					if (DoLinesIntersect(A, B, C, D, pointOfCollision))
						return true;
				}
			return false;
		}
	}
}
