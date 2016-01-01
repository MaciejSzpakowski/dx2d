#pragma once

namespace Viva
{
	namespace Collision
	{
		bool IsColliding(Circle* c1, Circle* c2);

		bool IsColliding(Circle* c, Rectangle* r);

		bool IsColliding(Circle* c, Polygon* p);

		// Very efficient collision check for rectangles.
		// If rectangle Rotation.z is 0 it's considered axis aligned.
		// If r1 and r2 are axis aligned, the test is even faster
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

		// If p1 and p2 are colliding, p2 is pushed away.
		// Returns true if p2 has been pushed.
		bool Push(Polygon* p1, Polygon* p2);
	}
}
