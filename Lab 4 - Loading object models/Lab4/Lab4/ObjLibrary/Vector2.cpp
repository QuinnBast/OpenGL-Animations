//
//  Vector2.cpp
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <iostream>
#include <cstdlib>	// for rand()
#include <cassert>

//
//  ObjSettings.h may be #included by header file.  It does not
//    affect this file directly.
//

#include "Vector2.h"

using namespace std;
using namespace ObjLibrary;
namespace
{
	static const double PI     = 3.1415926535897932384626433832795;
	static const double TWO_PI = 6.283185307179586476925286766559;



	//
	//  random0Exclude1 (random0 from Random.h/.cpp)
	//
	//  Purpose; To generate a psuedorandom number in the
	//	     range [0, 1).
	//  Parameter(s): N/A
	//  Precondition(s): N/A
	//  Returns: A pseudorandom number in the range [0, 1).
	//  Side Effect: N/A
	//  Note: Calls the rand() function
	//
	inline double random0Exclude1 ()
	{
		// RAND_MAX can be MAX_INT (and is on g++)
		return (double)(rand()) / (RAND_MAX + 1.0);
	}

	//
	//  random0Include1 (random0 from Random.h/.cpp)
	//
	//  Purpose; To generate a psuedorandom number in the
	//	     range [0, 1).
	//  Parameter(s): N/A
	//  Precondition(s): N/A
	//  Returns: A pseudorandom number in the range [0, 1).
	//  Side Effect: N/A
	//  Note: Calls the rand() function
	//
	inline double random0Include1 ()
	{
		return (double)(rand()) / (double)(RAND_MAX);
	}
}


const Vector2 Vector2 :: ZERO(0.0, 0.0);
const Vector2 Vector2 :: ONE (1.0, 1.0);
const Vector2 Vector2 :: UNIT_X_PLUS ( 1.0,  0.0);
const Vector2 Vector2 :: UNIT_X_MINUS(-1.0,  0.0);
const Vector2 Vector2 :: UNIT_Y_PLUS ( 0.0,  1.0);
const Vector2 Vector2 :: UNIT_Y_MINUS( 0.0, -1.0);



double Vector2 :: getCosAngle (const Vector2& other) const
{
	assert(isFinite());
	assert(!isZero());
	assert(other.isFinite());
	assert(!other.isZero());

	assert(getNorm() != 0.0);
	assert(other.getNorm() != 0.0);
	double ratio = dotProduct(other) / (getNorm() * other.getNorm());

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (ratio < -1.0) ? -1.0 : ((ratio > 1.0) ? 1.0 : ratio);
}

double Vector2 :: getCosAngleNormal (const Vector2& other) const
{
	assert(isFinite());
	assert(isNormal());
	assert(other.isFinite());
	assert(other.isNormal());

	double dot_product = dotProduct(other);

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (dot_product < -1.0) ? -1.0 : ((dot_product > 1.0) ? 1.0 : dot_product);
}

double Vector2 :: getCosAngleSafe (const Vector2& other) const
{
	assert(isFinite());
	assert(other.isFinite());

	if(isZero() || other.isZero())
		return 1.0;

	assert(getNorm() != 0.0);
	assert(other.getNorm() != 0.0);
	double ratio = dotProduct(other) / (getNorm() * other.getNorm());

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (ratio < -1.0) ? -1.0 : ((ratio > 1.0) ? 1.0 : ratio);
}

double Vector2 :: getAngle (const Vector2& other) const
{
	assert(isFinite());
	assert(!isZero());
	assert(other.isFinite());
	assert(!other.isZero());

	assert(getNorm() != 0.0);
	assert(other.getNorm() != 0.0);
	double ratio = dotProduct(other) / (getNorm() * other.getNorm());

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (ratio < -1.0) ? PI : ((ratio > 1.0) ? 0.0 : acos(ratio));
}

double Vector2 :: getAngleNormal (const Vector2& other) const
{
	assert(isFinite());
	assert(isNormal());
	assert(other.isFinite());
	assert(other.isNormal());

	double dot_product = dotProduct(other);

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (dot_product < -1.0) ? PI : ((dot_product > 1.0) ? 0.0 : acos(dot_product));
}

double Vector2 :: getAngleSafe (const Vector2& other) const
{
	assert(isFinite());
	assert(other.isFinite());

	if(isZero() || other.isZero())
		return 1.0;

	assert(getNorm() != 0.0);
	assert(other.getNorm() != 0.0);
	double ratio = dotProduct(other) / (getNorm() * other.getNorm());

	//  In theory, ratio should always be in the range [-1, 1].
	//    Sadly, in reality there are floating point errors.
	return (ratio < -1.0) ? PI : ((ratio > 1.0) ? 0.0 : acos(ratio));
}



Vector2 Vector2 :: getRotatedTowardsNormal (const Vector2& desired,
                                            double radians) const
{
	assert(isFinite());
	assert(desired.isFinite());
	assert(desired.isNormal());

	// rotating the zero vector is meaningless and impossible to calculate
	if(isZero())
		return ZERO;

	double radians_max = getAngle(desired);
	if(radians > radians_max)
		radians = radians_max;

	//  WHICH WAY SHOULD THE SIGNS BE?  <|>
	if(getPerpendicular().dotProduct(desired) > 0)
		return getRotated(radians);
	else
		return getRotated(-radians);
}

void Vector2 :: rotateTowardsNormal (const Vector2& desired,
                                     double radians)
{
	assert(isFinite());
	assert(desired.isFinite());
	assert(desired.isNormal());

	// rotating the zero vector is meaningless and impossible to calculate
	if(isZero())
		return;

	double radians_max = getAngle(desired);
	if(radians > radians_max)
		radians = radians_max;

	//  WHICH WAY SHOULD THE SIGNS BE?  <|>
	if(getPerpendicular().dotProduct(desired) > 0)
		rotate(radians);
	else
		rotate(-radians);
}



Vector2 Vector2 :: getRandomUnitVector ()
{
	double angle = random0Exclude1() * TWO_PI;
	return Vector2(cos(angle), sin(angle));
}

Vector2 Vector2 :: getPseudorandomUnitVector (double seed)
{
	assert(seed >= 0.0);
	assert(seed <  1.0);

	double angle = seed * TWO_PI;
	return Vector2(cos(angle), sin(angle));
}

Vector2 Vector2 :: getRandomSphereVector ()
{
	//
	//  We will use an infinite loop that ends when we find
	//    a vector that isn't too long.  This is faster than
	//    generating a random unit vector and adjusting the
	//    length because it avoids slow math like sqrt and
	//    trigonometric functions.
	//

	while (true)  // loop returns below
	{
		Vector2 vector(random0Include1() * 2.0 - 1.0,
		               random0Include1() * 2.0 - 1.0);
		if(vector.getNormSquared() <= 1.0)
			return vector;
	}
}

Vector2 Vector2 :: getPseudorandomSphereVector (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	//
	//  We cannot just generate vectors and test their norms
	//    this time because we have only a finite number of
	//    seeds.
	//
	//  Instead, we generate a random unit vector and then
	//    assign it a random length.   Note that the volume of a
	//    circle goes up with the square of the radius, so we
	//    will have to use the square root to get the right
	//    distribution of lengths.
	//

	double length = sqrt(seed2);
	return getPseudorandomUnitVector(seed1) * length;
}



Vector2 Vector2 :: getRandomInRange ()
{
	return Vector2(random0Exclude1(),
	               random0Exclude1());
}

Vector2 Vector2 :: getRandomInRange (double max)
{
	assert(max >= 0.0);

	return Vector2(random0Exclude1(),
	               random0Exclude1()) * max;
}

Vector2 Vector2 :: getRandomInRange (const Vector2& max)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));

	return Vector2(random0Exclude1(),
	               random0Exclude1()).getComponentProduct(max);
}

Vector2 Vector2 :: getRandomInRange (double min, double max)
{
	assert(min <= max);

	double range = max - min;
	return Vector2(min + random0Exclude1() * range,
	               min + random0Exclude1() * range);
}

Vector2 Vector2 :: getRandomInRange (const Vector2& min, const Vector2& max)
{
	assert(min.isAllComponentsLessThanOrEqual(max));

	return min + Vector2(random0Exclude1(),
	                     random0Exclude1()).getComponentProduct(max - min);
}

Vector2 Vector2 :: getPseudorandomInRange (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	return Vector2(seed1, seed2);
}

Vector2 Vector2 :: getPseudorandomInRange (double max,
                                           double seed1, double seed2)
{
	assert(max >= 0.0);
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	return Vector2(seed1, seed2) * max;
}

Vector2 Vector2 :: getPseudorandomInRange (const Vector2& max,
                                           double seed1, double seed2)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	return Vector2(seed1, seed2).getComponentProduct(max);
}

Vector2 Vector2 :: getPseudorandomInRange (double min, double max,
                                           double seed1, double seed2)
{
	assert(min <= max);
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	double range = max - min;
	return Vector2(min + seed1 * range,
	               min + seed2 * range);
}

Vector2 Vector2 :: getPseudorandomInRange (const Vector2& min, const Vector2& max,
                                           double seed1, double seed2)
{
	assert(min.isAllComponentsLessThanOrEqual(max));
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	return min + Vector2(seed1, seed2).getComponentProduct(max - min);
}

Vector2 Vector2 :: getRandomInRangeInclusive ()
{
	return Vector2(random0Include1(),
	               random0Include1());
}

Vector2 Vector2 :: getRandomInRangeInclusive (const Vector2& max)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));

	return Vector2(random0Include1(),
	               random0Include1()).getComponentProduct(max);
}

Vector2 Vector2 :: getRandomInRangeInclusive (const Vector2& min, const Vector2& max)
{
	assert(min.isAllComponentsLessThanOrEqual(max));

	return min + Vector2(random0Include1(),
	                     random0Include1()).getComponentProduct(max - min);
}



Vector2 Vector2 :: getClosestPointOnLine (const Vector2& l1,
                                          const Vector2& l2,
                                          const Vector2& p,
                                          bool bounded)
{
	assert(l1.isFinite());
	assert(l2.isFinite());
	assert(p.isFinite());
	assert(l1 != l2);

	//
	//         + p
	//       /
	//     /
	//   +-----+-------+
	//  l1     s       l2
	//     +
	//     O
	//
	//  O: The origin (0, 0, 0)
	//  l1, l2: The two ends of the line segment
	//  p: The not-on-line-point
	//  s: The point on the line segment closest to p
	//

	Vector2 line_direction = l2 - l1;
	Vector2 p_direction = p - l1;
	Vector2 s_minus_l1 = p_direction.getProjection(line_direction);

	if(bounded)
	{
		if(s_minus_l1.dotProduct(line_direction) <= 0)
			return l1;
		else if(s_minus_l1.getNormSquared() > line_direction.getNormSquared())
			return l2;
	}

	return s_minus_l1 + l1;
}



ostream& ObjLibrary :: operator<< (ostream& r_os, const Vector2& vector)
{
	r_os << "(" << vector.x << ", " << vector.y << ")";
	return r_os;
}


