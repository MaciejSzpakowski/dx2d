#include "Viva.h"

namespace Viva
{
	using namespace DirectX;

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
			XMVECTOR dist = p1->_GetPositionVector() - p2->_GetPositionVector();
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
			XMVECTOR v = XMVectorSubtract(c1->_GetPositionVector(), c2->_GetPositionVector());
			XMVECTOR len = XMVector2Length(v);
			return XMVectorGetX(len) < (c1->GetRadius() + c2->GetRadius());
		}

		bool IsColliding(Circle* c, Rectangle* r)
		{
			throw VIVA_ERROR("Not implemented");
		}

		bool IsColliding(Rectangle* c, Rectangle* r)
		{
			throw VIVA_ERROR("Not implemented");
		}

		bool IsColliding(Circle* c, Polygon* p)
		{
			throw VIVA_ERROR("Not implemented");
		}

		XMFLOAT2 GetProj(XMVECTOR edgeNormal, Polygon* p)
		{
			float min = XMVectorGetX(XMVector2Dot(edgeNormal, p->GetVerticesTransformed()[0]));
			float max = min;
			int count = p->GetVertexCount();
			for (int i = 1; i < count; i++)
			{
				float dot = XMVectorGetX(XMVector2Dot(edgeNormal, p->GetVerticesTransformed()[i]));
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
			int count = p1->GetVertexCount();
			for (int i = 0; i < count; i++)
			{
				if (i == count - 1)
					edgeNormal = p1->GetVerticesTransformed()[0] - p1->GetVerticesTransformed()[i];
				else
					edgeNormal = p1->GetVerticesTransformed()[i + 1] - p1->GetVerticesTransformed()[i];
				edgeNormal = XMVector2Orthogonal(edgeNormal);
				XMFLOAT2 proj1 = GetProj(edgeNormal, p1);
				XMFLOAT2 proj2 = GetProj(edgeNormal, p2);
				if (!DoProjsOverlap(proj1, proj2))
					return false;
			}
			//proj onto all edges normals of p2
			count = p2->GetVertexCount();
			for (int i = 0; i < count; i++)
			{
				if (i == count - 1)
					edgeNormal = p2->GetVerticesTransformed()[0] - p2->GetVerticesTransformed()[i];
				else
					edgeNormal = p2->GetVerticesTransformed()[i + 1] - p2->GetVerticesTransformed()[i];
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

			for (int i = 0; i < p1->GetVertexCount() - 1; i++)
			{
				for (int j = 0; j < p2->GetVertexCount() - 1; j++)
				{
					XMVECTOR A = p1->GetVerticesTransformed()[i];
					XMVECTOR B = p1->GetVerticesTransformed()[i + 1];
					XMVECTOR C = p2->GetVerticesTransformed()[j];
					XMVECTOR D = p2->GetVerticesTransformed()[j + 1];

					if (DoLinesIntersect(A, B, C, D, pointOfCollision))
						return true;
				}
			}

			return false;
		}

		bool Push(Polygon* p1, Polygon* p2)
		{
			throw VIVA_ERROR("Not implemented");
		}
	}
}