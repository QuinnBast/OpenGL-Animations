//
//  Vector3.cpp
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

#include "Vector3.h"

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
	//	     range [0, 1].
	//  Parameter(s): N/A
	//  Precondition(s): N/A
	//  Returns: A pseudorandom number in the range [0, 1].
	//  Side Effect: N/A
	//  Note: Calls the rand() function
	//
	inline double random0Include1 ()
	{
		return (double)(rand()) / (double)(RAND_MAX);
	}



	//
	//  Matrix3x3
	//
	//  A minimal class to encapsulate a 3x3 matrix.  This
	//    class is used when rotating a Vector3 around an
	//    arbitrary axis.
	//
	class Matrix3x3
	{
	public:
		static const unsigned int ROWS        = 3;
		static const unsigned int COLUMNS     = 3;
		static const unsigned int ENTRY_COUNT = ROWS * COLUMNS;

	public:
		Matrix3x3(double e11, double e12, double e13,
		          double e21, double e22, double e23,
		          double e31, double e32, double e33)
		{
			ma_entries[0] = e11;
			ma_entries[1] = e12;
			ma_entries[2] = e13;
			ma_entries[3] = e21;
			ma_entries[4] = e22;
			ma_entries[5] = e23;
			ma_entries[6] = e31;
			ma_entries[7] = e32;
			ma_entries[8] = e33;
		}

		Matrix3x3(const Matrix3x3& original)
		{
			for(unsigned int i = 0; i < ENTRY_COUNT; i++)
				ma_entries[i] = original.ma_entries[i];
		}

		Matrix3x3 operator+ (const Matrix3x3& other) const
		{
			Matrix3x3 result(*this);

			for(unsigned int i = 0; i < ENTRY_COUNT; i++)
				result.ma_entries[i] += other.ma_entries[i];

			return result;
		}

		Matrix3x3 operator- (const Matrix3x3& other) const
		{
			Matrix3x3 result(*this);

			for(unsigned int i = 0; i < ENTRY_COUNT; i++)
				result.ma_entries[i] -= other.ma_entries[i];

			return result;
		}

		Matrix3x3 operator* (double constant) const
		{
			Matrix3x3 result(*this);

			for(unsigned int i = 0; i < ENTRY_COUNT; i++)
				result.ma_entries[i] *= constant;

			return result;
		}

		Vector3 operator* (const Vector3& right) const
		{
			return Vector3(right.x * ma_entries[0] + right.y * ma_entries[1] + right.z * ma_entries[2],
			               right.x * ma_entries[3] + right.y * ma_entries[4] + right.z * ma_entries[5],
			               right.x * ma_entries[6] + right.y * ma_entries[7] + right.z * ma_entries[8]);
		}

		inline double getEntry (unsigned int row, unsigned int column) const
		{
			assert(row    < ROWS);
			assert(column < COLUMNS);

			return ma_entries[row * COLUMNS + column];
		}

	private:
		double ma_entries[ENTRY_COUNT];
	};
}	// end of anonymous namespace



const Vector3 Vector3 :: ZERO(0.0, 0.0, 0.0);
const Vector3 Vector3 :: ONE (1.0, 1.0, 1.0);
const Vector3 Vector3 :: UNIT_X_PLUS ( 1.0,  0.0,  0.0);
const Vector3 Vector3 :: UNIT_X_MINUS(-1.0,  0.0,  0.0);
const Vector3 Vector3 :: UNIT_Y_PLUS ( 0.0,  1.0,  0.0);
const Vector3 Vector3 :: UNIT_Y_MINUS( 0.0, -1.0,  0.0);
const Vector3 Vector3 :: UNIT_Z_PLUS ( 0.0,  0.0,  1.0);
const Vector3 Vector3 :: UNIT_Z_MINUS( 0.0,  0.0, -1.0);



double Vector3 :: getCosAngle (const Vector3& other) const
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

double Vector3 :: getCosAngleNormal (const Vector3& other) const
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

double Vector3 :: getCosAngleSafe (const Vector3& other) const
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

double Vector3 :: getAngle (const Vector3& other) const
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

double Vector3 :: getAngleNormal (const Vector3& other) const
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

double Vector3 :: getAngleSafe (const Vector3& other) const
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



Vector3 Vector3 :: getRotatedArbitraryNormal (const Vector3& axis, double radians) const
{
	assert(isFinite());
	assert(axis.isFinite());
	assert(axis.isNormal());

	static const Matrix3x3 A_INDENTITY_MATRIX(1, 0, 0, 0, 1, 0, 0, 0, 1);

	// www2.cs.uregina.ca/~anima/408/Notes/ObjectModels/Rotation.htm

	double aa = axis.x * axis.x;
	double bb = axis.y * axis.y;
	double cc = axis.z * axis.z;
	double ab = axis.x * axis.y;
	double ac = axis.x * axis.z;
	double bc = axis.y * axis.z;

	Matrix3x3 A_hat(aa, ab, ac, ab, bb, bc, ac, bc, cc);
	Matrix3x3 A_star( 0,      -axis.z,  axis.y,
	                  axis.z,  0,      -axis.x,
	                 -axis.y,  axis.x,  0);

	Matrix3x3 M = A_hat + (A_INDENTITY_MATRIX - A_hat) * cos(radians) + A_star * sin(radians);

	return M * (*this);
}

Vector3 Vector3 :: getRotatedTowardsNormal (const Vector3& desired,
                                            double radians) const
{
	assert(isFinite());
	assert(desired.isFinite());
	assert(desired.isNormal());

	// rotating the zero vector is meaningless and impossible to calculate
	if(isZero())
		return ZERO;

	Vector3 axis = crossProduct(desired);
	if(axis.isZero())
		return *this;  // *this and desired are parallel
	assert(!axis.isZero());
	axis.normalize();

	double radians_max = getAngle(desired);
	if(radians > radians_max)
		radians = radians_max;

	return getRotatedArbitraryNormal(axis, radians);
}

void Vector3 :: rotateArbitraryNormal (const Vector3& axis, double radians)
{
	assert(isFinite());
	assert(axis.isFinite());
	assert(axis.isNormal());

	static const Matrix3x3 A_INDENTITY_MATRIX(1, 0, 0, 0, 1, 0, 0, 0, 1);

	// www2.cs.uregina.ca/~anima/408/Notes/ObjectModels/Rotation.htm

	double aa = axis.x * axis.x;
	double bb = axis.y * axis.y;
	double cc = axis.z * axis.z;
	double ab = axis.x * axis.y;
	double ac = axis.x * axis.z;
	double bc = axis.y * axis.z;

	Matrix3x3 A_hat(aa, ab, ac, ab, bb, bc, ac, bc, cc);
	Matrix3x3 A_star( 0,      -axis.z,  axis.y,
	                  axis.z,  0,      -axis.x,
	                 -axis.y,  axis.x,  0);

	Matrix3x3 M = A_hat + (A_INDENTITY_MATRIX - A_hat) * cos(radians) + A_star * sin(radians);

	operator=(M * (*this));
}

void Vector3 :: rotateTowardsNormal (const Vector3& desired,
                                     double radians)
{
	assert(isFinite());
	assert(desired.isFinite());
	assert(desired.isNormal());

	// rotating the zero vector is meaningless and impossible to calculate
	if(isZero())
		return;

	Vector3 axis = crossProduct(desired);
	if(axis.isZero())
		return;  // *this and desired are parallel
	assert(!axis.isZero());
	axis.normalize();

	double radians_max = getAngle(desired);
	if(radians > radians_max)
		radians = radians_max;

	rotateArbitraryNormal(axis, radians);
}

Vector3 Vector3 :: getMatrixProduct (double e11, double e12, double e13,
				     double e21, double e22, double e23,
				     double e31, double e32, double e33) const
{
	assert(isFinite());

	return Matrix3x3(e11, e12, e13,
	                 e21, e22, e23,
	                 e31, e32, e33) * (*this);
}

Vector3 Vector3 :: getMatrixProductRows (const Vector3& r1,
					 const Vector3& r2,
					 const Vector3& r3) const
{
	assert(isFinite());
	assert(r1.isFinite());
	assert(r2.isFinite());
	assert(r3.isFinite());

	return Matrix3x3(r1.x, r1.y, r1.z,
	                 r2.x, r2.y, r2.z,
	                 r3.x, r3.y, r3.z) * (*this);
}

Vector3 Vector3 :: getMatrixProductColumns (const Vector3& c1,
					    const Vector3& c2,
					    const Vector3& c3) const
{
	assert(isFinite());
	assert(c1.isFinite());
	assert(c2.isFinite());
	assert(c3.isFinite());

	return Matrix3x3(c1.x, c2.x, c3.x,
	                 c1.y, c2.y, c3.y,
	                 c1.z, c2.z, c3.z) * (*this);
}



Vector3 Vector3 :: getRandomUnitVector ()
{
	//
	//  The following WDL script is included because it was
	//    used to derive the algorithm for this function.
	//

/*
From Spheretest.wdl (with variable names added)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// This function creates a hollow sphere of "equally" spaced balls
ACTION Spheremaker
{
	z_spacing = 360/around_z;

	z_current = z_spacing;
	while(z_current < 180)
	{
		xy_radius = sin(z_current)*radius;
		z_radius = cos(z_current)*radius;

		around_count = int(sin(z_current)*360/z_spacing);

		around_current = 0;
		while(around_current < around_count)
		{
			glow_pos.x = sin(around_current/around_count*360)*xy_radius;
			glow_pos.y = cos(around_current/around_count*360)*xy_radius;
			glow_pos.z = z_radius;
			ent_create(ball_mdl,glow_pos,Spherepoint);

			around_current++;
		}

		z_current += z_spacing;
	}
}
*/

	//
	//  Generate a random unit vector.
	//
	//  1. Pick a random direction on the xy plane
	//  2. Pick a random angle towards to z-axis, weighted
	//     so that the values on the xy plane are the most
	//     likely.  Not that this means the z values are
	//     equally distruibuted.
	//
	//  This function does not call getPseudoandomUnitVector
	//    for speed reasons.
	//

	double xy_angle = random0Exclude1() * TWO_PI;
	double z = random0Include1() * 2.0 - 1.0;
	double radius_xy = sqrt(1.0 - z * z);
	double x = radius_xy * cos(xy_angle);
	double y = radius_xy * sin(xy_angle);

	return Vector3(x, y, z);
}

Vector3 Vector3 :: getPseudorandomUnitVector (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	//
	//  This function is the same as getRandomUnitVector except:
	//    -> Seeds are used in place of random numbers
	//    -> This function cannot return a vector of (0, 0, 1)
	//       because the second seed value cannot be 1.0
	//

	double xy_angle = seed1 * TWO_PI;
	double z = seed2 * 2.0 - 1.0;
	double radius_xy = sqrt(1.0 - z * z);
	double x = radius_xy * cos(xy_angle);
	double y = radius_xy * sin(xy_angle);

	return Vector3(x, y, z);
}

Vector3 Vector3 :: getRandomUnitVectorXY ()
{
	double angle = random0Exclude1() * TWO_PI;
	return Vector3(cos(angle), sin(angle), 0.0);
}

Vector3 Vector3 :: getRandomUnitVectorXZ ()
{
	double angle = random0Exclude1() * TWO_PI;
	return Vector3(cos(angle), 0.0, sin(angle));
}

Vector3 Vector3 :: getRandomUnitVectorYZ ()
{
	double angle = random0Exclude1() * TWO_PI;
	return Vector3(0.0, cos(angle), sin(angle));
}

Vector3 Vector3 :: getPseudorandomUnitVectorXY (double seed)
{
	assert(seed >= 0.0);
	assert(seed <  1.0);

	double angle = seed * TWO_PI;
	return Vector3(cos(angle), sin(angle), 0.0);
}

Vector3 Vector3 :: getPseudorandomUnitVectorXZ (double seed)
{
	assert(seed >= 0.0);
	assert(seed <  1.0);

	double angle = seed * TWO_PI;
	return Vector3(cos(angle), 0.0, sin(angle));
}

Vector3 Vector3 :: getPseudorandomUnitVectorYZ (double seed)
{
	assert(seed >= 0.0);
	assert(seed <  1.0);

	double angle = seed * TWO_PI;
	return Vector3(0.0, cos(angle), sin(angle));
}

Vector3 Vector3 :: getRandomSphereVector ()
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
		Vector3 vector(random0Include1() * 2.0 - 1.0,
		               random0Include1() * 2.0 - 1.0,
		               random0Include1() * 2.0 - 1.0);
		if(vector.getNormSquared() <= 1.0)
			return vector;
	}
}

Vector3 Vector3 :: getPseudorandomSphereVector (double seed1,
                                                double seed2,
                                                double seed3)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	//
	//  We cannot just generate vectors and test their norms
	//    this time because we have only a finite number of
	//    seeds.
	//
	//  Instead, we generate a random unit vector and then
	//    assign it a random length.   Note that the volume of a
	//    sphere goes up with the cube of the radius, so we will
	//    have to use the cube root to get the right
	//    distribution of lengths.
	//

#if __cplusplus >= 201103L  // C++11
	// use the better function if we have it
	double length = cbrt(seed3);
#else
	static const double ONE_THIRD = 0.3333333333333333333333333;
	double length = pow(seed3, ONE_THIRD);
#endif

	return getPseudorandomUnitVector(seed1, seed2) * length;
}

Vector3 Vector3 :: getRandomSphereVectorXY ()
{
	while (true)  // loop returns below
	{
		Vector3 vector(random0Include1() * 2.0 - 1.0,
		               random0Include1() * 2.0 - 1.0,
		               0.0);
		if(vector.getNormSquared() <= 1.0)
			return vector;
	}
}

Vector3 Vector3 :: getRandomSphereVectorXZ ()
{
	while (true)  // loop returns below
	{
		Vector3 vector(random0Include1() * 2.0 - 1.0,
		               0.0,
		               random0Include1() * 2.0 - 1.0);
		if(vector.getNormSquared() <= 1.0)
			return vector;
	}
}

Vector3 Vector3 :: getRandomSphereVectorYZ ()
{
	while (true)  // loop returns below
	{
		Vector3 vector(0.0,
		               random0Include1() * 2.0 - 1.0,
		               random0Include1() * 2.0 - 1.0);
		if(vector.getNormSquared() <= 1.0)
			return vector;
	}
}

Vector3 Vector3 :: getPseudorandomSphereVectorXY (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	//
	//  The volume of a circle goes up with the square of the
	//    radius, so we will have to use the square root to get
	//    the right distribution of lengths.
	//

	double length = sqrt(seed2);
	return getPseudorandomUnitVectorXY(seed1) * length;
}

Vector3 Vector3 :: getPseudorandomSphereVectorXZ (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	double length = sqrt(seed2);
	return getPseudorandomUnitVectorXZ(seed1) * length;
}

Vector3 Vector3 :: getPseudorandomSphereVectorYZ (double seed1, double seed2)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);

	double length = sqrt(seed2);
	return getPseudorandomUnitVectorYZ(seed1) * length;
}



Vector3 Vector3 :: getRandomInRange ()
{
	return Vector3(random0Exclude1(),
	               random0Exclude1(),
	               random0Exclude1());
}

Vector3 Vector3 :: getRandomInRange (double max)
{
	assert(max >= 0.0);

	return Vector3(random0Exclude1(),
	               random0Exclude1(),
	               random0Exclude1()) * max;
}

Vector3 Vector3 :: getRandomInRange (const Vector3& max)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));

	return Vector3(random0Exclude1(),
	               random0Exclude1(),
	               random0Exclude1()).getComponentProduct(max);
}

Vector3 Vector3 :: getRandomInRange (double min, double max)
{
	assert(min <= max);

	double range = max - min;
	return Vector3(min + random0Exclude1() * range,
	               min + random0Exclude1() * range,
	               min + random0Exclude1() * range);
}

Vector3 Vector3 :: getRandomInRange (const Vector3& min, const Vector3& max)
{
	assert(min.isAllComponentsLessThanOrEqual(max));

	return min + Vector3(random0Exclude1(),
	                     random0Exclude1(),
	                     random0Exclude1()).getComponentProduct(max - min);
}

Vector3 Vector3 :: getPseudorandomInRange (double seed1,
                                           double seed2,
                                           double seed3)
{
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	return Vector3(seed1, seed2, seed3);
}

Vector3 Vector3 :: getPseudorandomInRange (double max,
                                           double seed1, double seed2, double seed3)
{
	assert(max >= 0.0);
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	return Vector3(seed1, seed2, seed3) * max;
}

Vector3 Vector3 :: getPseudorandomInRange (const Vector3& max,
                                           double seed1, double seed2, double seed3)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	return Vector3(seed1, seed2, seed3).getComponentProduct(max);
}

Vector3 Vector3 :: getPseudorandomInRange (double min, double max,
                                           double seed1, double seed2, double seed3)
{
	assert(min <= max);
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	double range = max - min;
	return Vector3(min + seed1 * range,
	               min + seed2 * range,
	               min + seed3 * range);
}

Vector3 Vector3 :: getPseudorandomInRange (const Vector3& min, const Vector3& max,
                                           double seed1, double seed2, double seed3)
{
	assert(min.isAllComponentsLessThanOrEqual(max));
	assert(seed1 >= 0.0);
	assert(seed1 <  1.0);
	assert(seed2 >= 0.0);
	assert(seed2 <  1.0);
	assert(seed3 >= 0.0);
	assert(seed3 <  1.0);

	return min + Vector3(seed1, seed2, seed3).getComponentProduct(max - min);
}

Vector3 Vector3 :: getRandomInRangeInclusive ()
{
	return Vector3(random0Include1(),
	               random0Include1(),
	               random0Include1());
}

Vector3 Vector3 :: getRandomInRangeInclusive (const Vector3& max)
{
	assert(max.isAllComponentsGreaterThanOrEqual(ZERO));

	return Vector3(random0Include1(),
	               random0Include1(),
	               random0Include1()).getComponentProduct(max);
}

Vector3 Vector3 :: getRandomInRangeInclusive (const Vector3& min, const Vector3& max)
{
	assert(min.isAllComponentsLessThanOrEqual(max));

	return min + Vector3(random0Include1(),
	                     random0Include1(),
	                     random0Include1()).getComponentProduct(max - min);
}



Vector3 Vector3 :: getClosestPointOnLine (const Vector3& l1,
                                          const Vector3& l2,
                                          const Vector3& p,
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

	Vector3 line_direction = l2 - l1;
	Vector3 p_direction = p - l1;
	Vector3 s_minus_l1 = p_direction.getProjection(line_direction);

	if(bounded)
	{
		if(s_minus_l1.dotProduct(line_direction) <= 0)
			return l1;
		else if(s_minus_l1.getNormSquared() > line_direction.getNormSquared())
			return l2;
	}

	return s_minus_l1 + l1;
}



ostream& ObjLibrary :: operator<< (ostream& r_os, const Vector3& vector)
{
	r_os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return r_os;
}
