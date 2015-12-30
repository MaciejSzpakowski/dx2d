#pragma once

namespace Viva
{
	namespace Functions
	{
		//creates new CCore object so you dont have to use new operator
		void InitViva(Size clientSize, std::function<void()> worker, int style = WS_OVERLAPPEDWINDOW);
				
		//check for HR when directx init functions are called
		void Checkhr(LPCSTR file, int line, HRESULT hr);
	}

	namespace Random
	{
		//return double between 0(inclusive) and 1(exclusive)
		double RndDouble();

		//return integer
		//between min and max (both inclusive)
		int RndInt(int min, int max);
	}

	namespace Collision
	{
		bool IsColliding(Circle* c1, Circle* c2);

		bool IsColliding(Circle* c, Rectangle* r);

		bool IsColliding(Circle* c, Polygon* p);

		//very efficient collision check for rectangles
		//if rectangle Rotation.z is 0 it's considered axis aligned
		//if r1 and r2 are axis aligned, the test is even faster
		bool IsColliding(Rectangle* r1, Rectangle* r2);

		//the most powerful (and inefficient) collision check
		//pointOfCollision can be used to get the first point of collision
		//this one checks collision line by line, if any of the two polygons
		//lines intersects it counts as collision
		//otherwise it's not a collision even if one poly is inside the another
		//polygons dont have to be closed
		bool IsColliding(Polygon* p1, Polygon* p2, XMFLOAT3* pointOfCollision);

		//uses separated axis theorem to check for collision
		//faster than line by line check but polygons must be closed and convex !
		bool IsCollidingSat(Polygon* p1, Polygon* p2);
	}
}
