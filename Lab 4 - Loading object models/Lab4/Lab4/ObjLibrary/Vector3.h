//
//  Vector3.h
//
//  A module to store a math-style vector of length 3 and simple
//    operations that can be performed on it.
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

#ifndef OBJ_LIBRARY_VECTOR3_H
#define OBJ_LIBRARY_VECTOR3_H

#include <cassert>
#include <cmath>
#include <cfloat>
#include <iostream>

// if you don't need glm, you can remove this line
#include "ObjSettings.h"
// if you don't have ObjSettings, but do need to use glm,
//  #define OBJ_LIBRARY_GLM_INTERACTION

#ifdef OBJ_LIBRARY_GLM_INTERACTION
  #include "../glm/vec3.hpp"
#endif



namespace ObjLibrary
{

//
//  VECTOR3_IS_FINITE
//
//  A cross-platform macro that resolves into the appropriate
//    function to check if a floating point value is finite.
//    A floating point value is considered to be finite if it is
//    not any of the following:
//      -> positive infinity
//      -> negative infinity
//      -> NaN (not a number: 0/0)
//

#if __cplusplus >= 201103L  // C++11
  //  I haven't tested this, but it is in the standard.  I hope
  //    it works.
  #define VECTOR3_IS_FINITE(n) std::isfinite(n)
#elif _WIN32
  // Microsoft non-standard function
  #define VECTOR3_IS_FINITE(n) _finite(n)
#elif __WIN32__
  // Microsoft non-standard function
  #define VECTOR3_IS_FINITE(n) _finite(n)
#else
  //  In theory, POSIX includes the isfinite macro defined in
  //    the C99 standard.  This macro is not included in any ISO
  //    C++ standard yet (as of January 2013).  However, this
  //    function does not seem to work.
  //#define VECTOR3_IS_FINITE(n) isfinite(n)
  //  From http://www.johndcook.com/blog/IEEE_exceptions_in_cpp/
  #define VECTOR3_IS_FINITE(n) ((n) <= DBL_MAX && (n) >= -DBL_MAX)
#endif

//
//  VECTOR3_CONSTEXPR
//
//  A cross-platform macro that resolves into constexpr on
//    constructors if the compiler supports it.  Hopefully.
//

#if __cpp_constexpr >= 200704  // C++11 constexpr
  #define VECTOR3_CONSTEXPR constexpr
#else
  #define VECTOR3_CONSTEXPR
#endif

//
//  VECTOR3_NORM_TOLERANCE
//
//  This is the fudge factor used when comparing vector normals.
//    It is also used for distance comparisons, which are
//    mathematically equivilent to the norm of the difference
//    between 2 vectors, and for a few other things.
//
//  The results of floating point calculations frequently vary
//    by small amounts.  This happens because the representation
//    is finite, while real numbers are infinite.  As a result,
//    it is not even theoretically possible to represent all
//    numbers correctly.  Because of this,
//    mathematically-identical floating point calculations
//    frequently give slightly different results.
//
//  This constant has to be declared outside the Vector3 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR3_NORM_TOLERANCE = 1.0e-4;

//
//  VECTOR3_NORM_TOLERANCE_SQUARED
//
//  This is the square of VECTOR3_NORM_TOLERANCE.  It is useful
//    as an intermediate value, and storing it seperately is
//    faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector3 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR3_NORM_TOLERANCE_SQUARED =
                                        VECTOR3_NORM_TOLERANCE *
                                        VECTOR3_NORM_TOLERANCE;

//
//  VECTOR3_NORM_TOLERANCE_PLUS_ONE_SQUARED
//
//  This is the square of (VECTOR3_NORM_TOLERANCE + 1.0).  It is
//    useful as an intermediate value, and storing it seperately
//    is faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector3 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR3_NORM_TOLERANCE_PLUS_ONE_SQUARED =
                                (VECTOR3_NORM_TOLERANCE + 1.0) *
                                (VECTOR3_NORM_TOLERANCE + 1.0);

//
//  VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED
//
//  This is 1.0 - the square of VECTOR3_NORM_TOLERANCE.  It is
//    useful as an intermediate value, and storing it seperately
//    is faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector3 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED =
                           1.0 - VECTOR3_NORM_TOLERANCE_SQUARED;

//
//  VECTOR3_ZERO_TOLERENCE
//
//  This is the tolerence used when testing whether a Vector3 is
//    a zero vector.  A slight tolerence is needed to ensure
//    that a Vector3 is not detected as non-zero when it has a
//    norm of 0.0.  This can happen with very small non-zero
//    vectors because calculating the norm of a Vector3 involves
//    squaring the value and the representation has a finite
//    range.
//
//  This constant has to be declared outside the Vector3 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR3_ZERO_TOLERENCE = 1.0e-100;



//
//  Vector3
//
//  A class to store a math-style vector of length 3.  The 3
//    numbers that compose a Vector3 are refered to as its
//    elements and may be accesed using dot notation.  The
//    associated functions are mostly declared inline for speed
//    reasons.  In theory, this class should be as fast (or
//    faster, when using pass-by-reference) as using double
//    values, but more convenient.
//
//  The norm of a Vector3 is its "length", the distance along
//    it.  There also exist functions to determine the norm of a
//    Vector3 along each axis-aligned plane.  These functions
//    calculate the norm the projection of this Vector3 to the
//    specified plane.  This returns the same result as
//    calculating the norm of a copy of the Vector3 with the
//    element for the excluded axis set to 0.0.
//
class Vector3
{
public:
//
//  x
//
//  The X component of the Vector3.  This value can be queried
//    and changed freely without disrupting the operation of the
//    Vector3 instance.
//
	double x;

//
//  y
//
//  The Y component of the Vector3.  This value can be queried
//    and changed freely without disrupting the operation of the
//    Vector3 instance.
//
	double y;

//
//  z
//
//  The Z component of the Vector3.  This value can be queried
//    and changed freely without disrupting the operation of the
//    Vector3 instance.
//
	double z;


//
//  These are some standard Vector3s that may be useful.
//
//  Note that C++ constants (including these ones) are not
//    necessarily initialized in any particular order.  Using
//    these to initialize global variables (or constants) may
//    query them before they are initialized, with undefined
//    results.  If anyone knows how to fix this, please tell me.
//

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 UNIT_X_PLUS;
	static const Vector3 UNIT_X_MINUS;
	static const Vector3 UNIT_Y_PLUS;
	static const Vector3 UNIT_Y_MINUS;
	static const Vector3 UNIT_Z_PLUS;
	static const Vector3 UNIT_Z_MINUS;

public:
//
//  Default Constructor
//
//  Purpose: To create a new Vector3 that is the zero vector.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with elements
//               (0.0, 0.0, 0.0).
//
	VECTOR3_CONSTEXPR Vector3 ()
			: x(0.0),
			  y(0.0),
			  z(0.0)
	{}

//
//  Initializing Constructor
//
//  Purpose: To create a new Vector3 with the specified
//           elements.
//  Parameter(s):
//    <1> x
//    <2> y
//    <3> z: The elements for the new Vector3
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with elements
//               (x, y, z).
//
	VECTOR3_CONSTEXPR Vector3 (double X, double Y, double Z)
			: x(X),
			  y(Y),
			  z(Z)
	{}

//
//  Constructor
//
//  Purpose: To create a new Vector3 with 3 elements taken from
//           the specified array.
//  Parameter(s):
//    <1> a_elements: The array containing the elements for the
//                    new Vector3
//  Precondition(s):
//    <1> a_elements != NULL
//  Returns: N/A
//  Side Effect: A new Vector3 is created with elements
//               (a_elements[0], a_elements[1], a_elements[2]).
//
	Vector3 (const double a_elements[])
			: x(a_elements[0]),
			  y(a_elements[1]),
			  z(a_elements[2])
	{
		assert(a_elements != NULL);
	}

//
//  Constructor
//
//  Purpose: To create a new Vector3 with the specified number
//           of elements taken from the specified array.
//  Parameter(s):
//    <1> a_elements: The array containing the elements for the
//                    new Vector3
//    <2> count: How many elements to take from the array
//  Precondition(s):
//    <1> a_elements != NULL
//    <2> count <= 3
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the first count
//               elements from array a_elements.  The remaining
//               elements of the new Vector3 are set to 0.0.
//
	Vector3 (const double a_elements[],
	         unsigned int count)
			: x((count > 0) ? a_elements[0] : 0.0),
			  y((count > 1) ? a_elements[1] : 0.0),
			  z((count > 2) ? a_elements[2] : 0.0)
	{
		assert(a_elements != NULL);
		assert(count <= 3);
	}

#ifdef OBJ_LIBRARY_GLM_INTERACTION
//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector3 from the specified
//           glm::vec3.
//  Parameter(s):
//    <1> glm_vec3: The glm::vec3 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the same elements
//               as glm_vec3, except expressed as a Vector3.
//
	VECTOR3_CONSTEXPR Vector3 (const glm::vec3& glm_vec3)
			: x(glm_vec3.x),
			  y(glm_vec3.y),
			  z(glm_vec3.z)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector3 from the specified
//           glm::ivec3.
//  Parameter(s):
//    <1> glm_ivec3: The glm::ivec3 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the same elements
//               as glm_ivec3, except expressed as a Vector3.
//
	VECTOR3_CONSTEXPR Vector3 (const glm::ivec3& glm_ivec3)
			: x(glm_ivec3.x),
			  y(glm_ivec3.y),
			  z(glm_ivec3.z)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector3 from the specified
//           glm::uvec3.
//  Parameter(s):
//    <1> glm_uvec3: The glm::uvec3 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the same elements
//               as glm_uvec3, except expressed as a Vector3.
//
	VECTOR3_CONSTEXPR Vector3 (const glm::uvec3& glm_uvec3)
			: x(glm_uvec3.x),
			  y(glm_uvec3.y),
			  z(glm_uvec3.z)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector3 from the specified
//           glm::dvec3.
//  Parameter(s):
//    <1> glm_dvec3: The glm::dvec3 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the same elements
//               as glm_dvec3, except expressed as a Vector3.
//
	VECTOR3_CONSTEXPR Vector3 (const glm::dvec3& glm_dvec3)
			: x(glm_dvec3.x),
			  y(glm_dvec3.y),
			  z(glm_dvec3.z)
	{
	}
#endif

//
//  Copy Constructor
//
//  Purpose: To create a new Vector3 with the same elements as
//           an existing Vector3.
//  Parameter(s):
//    <1> original: The Vector3 to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector3 is created with the same elements
//               as original.
//
	VECTOR3_CONSTEXPR Vector3 (const Vector3& original)
			: x(original.x),
			  y(original.y),
			  z(original.z)
	{}

//
//  Destructor
//
//  Purpose: To safely destroy this Vector3 without memeory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memeory is freed.
//
	// implicitly generate trivial destructor (required for constexpr)
	// ~Vector3 () = default;  // no good way to test for support

//
//  Assignment Operator
//
//  Purpose: To set the elements of this Vector3 to be equal to
//           the elements of another.
//  Parameter(s):
//    <1> original: The Vector3 to copy
//  Precondition(s): N/A
//  Returns: A reference to this Vector3.
//  Side Effect: The elements of this Vector3 are set to the
//               elements of original.
//
	Vector3& operator= (const Vector3& original)
	{
		//  Testing for self-assignment would take
		//    longer than just copying the values.
		x = original.x;
		y = original.y;
		z = original.z;

		return *this;
	}

#ifdef OBJ_LIBRARY_GLM_INTERACTION
//
//  glm Vector Typecast
//
//  Purpose: To convert this Vector3 to a glm::vec3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, expressed as a glm::vec3.  If the
//           contents are not reperesentable as a glm::vec3, the
//           result is undefined.
//  Side Effect: N/A
//
	operator glm::vec3 () const
	{
		return glm::vec3(x, y, z);
	}

//
//  glm Vector Typecast
//
//  Purpose: To convert this Vector3 to a glm::ivec3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, expressed as a glm::ivec3.  If the
//           contents are not reperesentable as a glm::ivec3,
//           the result is undefined.
//  Side Effect: N/A
//
	operator glm::ivec3 () const
	{
		return glm::ivec3(x, y, z);
	}

//
//  glm Vector Typecast
//
//  Purpose: To convert this Vector3 to a glm::uvec3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, expressed as a glm::uvec3.  If the
//           contents are not reperesentable as a glm::uvec3,
//           the result is undefined.
//  Side Effect: N/A
//
	operator glm::uvec3 () const
	{
		return glm::uvec3(x, y, z);
	}

//
//  glm Vector Typecast
//
//  Purpose: To convert this Vector3 to a glm::dvec3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, expressed as a glm::dvec3.
//  Side Effect: N/A
//
	operator glm::dvec3 () const
	{
		return glm::dvec3(x, y, z);
	}
#endif

//
//  Equality Operator
//
//  Purpose: To determine if this Vector3 is equal to another.
//           Two Vector3s are equal IFF each of their elements
//           are equal.
//  Parameter(s):
//    <1> other: The Vector3 to compare to
//  Precondition(s): N/A
//  Returns: Whether this Vector3 and other are equal.
//  Side Effect: N/A
//
	bool operator== (const Vector3& other) const
	{
		if(x != other.x) return false;
		if(y != other.y) return false;
		if(z != other.z) return false;
		return true;
	}

//
//  Inequality Operator
//
//  Purpose: To determine if this Vector3 and another are
//           unequal.  Two Vector3s are equal IFF each of their
//           elements are equal.
//  Parameter(s):
//    <1> other: The Vector3 to compare to
//  Precondition(s): N/A
//  Returns: Whether this Vector3 and other are unequal.
//  Side Effect: N/A
//
	bool operator!= (const Vector3& other) const
	{
		if(x != other.x) return true;
		if(y != other.y) return true;
		if(z != other.z) return true;
		return false;
	}

//
//  Negation Operator
//
//  Purpose: To create a new Vector3 that is the addative
//           inverse of this Vector3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A Vector3 with elements (-x, -y, -z).
//  Side Effect: N/A
//
	Vector3 operator- () const
	{
		return Vector3(-x, -y, -z);
	}

//
//  Addition Operator
//
//  Purpose: To create a new Vector3 equal to the sum of this
//           Vector3 and another.
//  Parameter(s):
//    <1> right: The Vector3 to add to this Vector3
//  Precondition(s): N/A
//  Returns: A Vector3 with elements
//           (x + right.x, y + right.y, z + right.z).
//  Side Effect: N/A
//
	Vector3 operator+ (const Vector3& right) const
	{
		return Vector3(x + right.x,
		               y + right.y,
		               z + right.z);
	}

//
//  Subtraction Operator
//
//  Purpose: To create a new Vector3 equal to the difference of
//           this Vector3 and another.
//  Parameter(s):
//    <1> right: The Vector3 to subtract from this Vector3
//  Precondition(s): N/A
//  Returns: A Vector3 with elements
//           (x - other.x, y - other.y, z - other.z).
//  Side Effect: N/A
//
	Vector3 operator- (const Vector3& right) const
	{
		return Vector3(x - right.x,
		               y - right.y,
		               z - right.z);
	}

//
//  Multiplication Operator
//
//  Purpose: To create a new Vector3 equal to the product of
//           this Vector3 and a scalar.
//  Parameter(s):
//    <1> factor: The scalar to multiply this Vector3 by
//  Precondition(s): N/A
//  Returns: A Vector3 with elements
//           (x * factor, y * factor, z * factor).
//  Side Effect: N/A
//
	Vector3 operator* (double factor) const
	{
		return Vector3(x * factor,
		               y * factor,
		               z * factor);
	}

//
//  Division Operator
//
//  Purpose: To create a new Vector3 equal to this Vector3
//           divided by a scalar.
//  Parameter(s):
//    <1> divisor: The scalar to divide this Vector3 by
//  Precondition(s):
//    <1> divisor != 0.0
//  Returns: A Vector3 with elements
//           (x / divisor, y / divisor, z / divisor).
//  Side Effect: N/A
//
	Vector3 operator/ (double divisor) const
	{
		assert(divisor != 0.0);

		return Vector3(x / divisor,
		               y / divisor,
		               z / divisor);
	}

//
//  Addition Assignment Operator
//
//  Purpose: To set this Vector3 to the sum of itself and
//           another Vector3.
//  Parameter(s):
//    <1> right: The Vector3 to add to this Vector3
//  Precondition(s): N/A
//  Returns: A reference to this Vector3.
//  Side Effect: The elements of this Vector3 are set to
//               (x + right.x, y + right.y, z + right.z).
//
	Vector3& operator+= (const Vector3& right)
	{
		x += right.x;
		y += right.y;
		z += right.z;

		return *this;
	}

//
//  Subtraction Assignment Operator
//
//  Purpose: To set this Vector3 to the difference of itself and
//           another Vector3.
//  Parameter(s):
//    <1> right: The Vector3 to subtract from this Vector3
//  Precondition(s): N/A
//  Returns: A reference to this Vector3.
//  Side Effect: The elements of this Vector3 are set to
//               (x - right.x, y - right.y, z - right.z).
//
	Vector3& operator-= (const Vector3& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;

		return *this;
	}

//
//  Multiplication Assignment Operator
//
//  Purpose: To set this Vector3 to the product of itself and a
//           scalar.
//  Parameter(s):
//    <1> factor: The scalar to multiply this Vector3 by
//  Precondition(s): N/A
//  Returns: A reference to this Vector3.
//  Side Effect: The elements of this Vector3 are set to
//               (x * factor, y * factor, z * factor).
//
	Vector3& operator*= (double factor)
	{
		x *= factor;
		y *= factor;
		z *= factor;

		return *this;
	}

//
//  Division Assignment Operator
//
//  Purpose: To set this Vector3 to equal to the quotient of
//           itself divided by a scalar.
//  Parameter(s):
//    <1> divisor: The scalar to divide this Vector3 by
//  Precondition(s):
//    <1> divisor != 0.0
//  Returns: A reference to this Vector3.
//  Side Effect: The elements of this Vector3 are set to
//               (x / divisor, y / divisor, z / divisor).
//
	Vector3& operator/= (double divisor)
	{
		assert(divisor != 0.0);

		x /= divisor;
		y /= divisor;
		z /= divisor;

		return *this;
	}

//
//  getAsArray
//
//  Purpose: To retreive the components of this Vector3 as an
//           array of 3 doubles.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, reinterpreted as an array of 3
//           doubles.
//  Side Effect: N/A
//  Warning: This funciton assumes that the memebr fields are
//           layed out sequentially.  If the compiler lays them
//           out in some other way, an assert error will be
//           generated at runtime when this function is called.
//
	double* getAsArray ()
	{
		assert(&y == (&x + 1));
		assert(&z == (&x + 2));
		return &x;
	}

//
//  getAsArray
//
//  Purpose: To retreive the components of this Vector3 as an
//           array of 3 constant doubles.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector3, reinterpreted as an array of 3 const
//           doubles.
//  Side Effect: N/A
//  Warning: This funciton assumes that the memebr fields are
//           layed out sequentially.  If the compiler lays them
//           out in some other way, an assert error will be
//           generated at runtime when this function is called.
//
	const double* getAsArray () const
	{
		assert(&y == (&x + 1));
		assert(&z == (&x + 2));
		return &x;
	}

//
//  isFinite
//
//  Purpose: To determine if all components of this Vector3 are
//           finite numbers.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector3 has only finite components.
//  Side Effect: N/A
//
	bool isFinite () const
	{
		if(!VECTOR3_IS_FINITE(x)) return false;
		if(!VECTOR3_IS_FINITE(y)) return false;
		if(!VECTOR3_IS_FINITE(z)) return false;
		return true;
	}

//
//  isZero
//
//  Purpose: To determine if this Vector3 is the zero vector, to
//           within tolerence VECTOR3_ZERO_TOLERENCE.  The
//           tolerance is used to ensure that the norm of this
//           Vector3 (which involves squaring the value) will
//           not be 0.0 do to limitations of the representation.
//           If you need to test if a Vector3 is exactly zero,
//           use the equality test or the isZeroStrict function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector3 is within 1.0e-100 inclusive
//           of (0.0, 0.0, 0.0) in each component.
//  Side Effect: N/A
//
	bool isZero () const
	{
		if(fabs(x) > VECTOR3_ZERO_TOLERENCE) return false;
		if(fabs(y) > VECTOR3_ZERO_TOLERENCE) return false;
		if(fabs(z) > VECTOR3_ZERO_TOLERENCE) return false;
		return true;
	}

//
//  isZeroStrict
//
//  Purpose: To determine if all components of this Vector3 are
//           exactly 0.0.  To avoid floating point rounding
//           errors, consider using isZero instead.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector3 is equal to (0.0, 0.0, 0.0).
//  Side Effect: N/A
//
	bool isZeroStrict () const
	{
		if(x != 0.0) return false;
		if(y != 0.0) return false;
		if(z != 0.0) return false;
		return true;
	}

//
//  isNormal
//
//  Purpose: To determine if this Vector3 is a unit vector,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This function does the same thing as isUnit.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector3 has a norm of 1.0.
//  Side Effect: N/A
//
	bool isNormal () const
	{
		double norm_sqr_minus_1 = getNormSquared() - 1;

		return (fabs(norm_sqr_minus_1) <
		        VECTOR3_NORM_TOLERANCE_SQUARED);
	}

//
//  isUnit
//
//  Purpose: To determine if this Vector3 is a unit vector,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This function does the same thing as isNormal.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector3 has a norm of 1.0.
//  Side Effect: N/A
//
	bool isUnit () const
	{
		double norm_sqr_minus_1 = getNormSquared() - 1;

		return (fabs(norm_sqr_minus_1) <
		        VECTOR3_NORM_TOLERANCE_SQUARED);
	}

//
//  getNorm
//
//  Purpose: To determine the norm of this Vector3.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The norm of this Vector3.
//  Side Effect: N/A
//
	double getNorm () const
	{
		return sqrt(x * x + y * y + z * z);
	}

//
//  getNormSquared
//
//  Purpose: To determine the square of the norm of this
//           Vector3.  This is significantly faster than
//           calculating the norm itself.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The square of the norm of this Vector3.
//  Side Effect: N/A
//
	double getNormSquared () const
	{
		return x * x + y * y + z * z;
	}

//
//  isNormEqualTo
//
//  Purpose: To determine if the norm of this Vector3 is equal
//           to the specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than calculating the norm
//           itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector3 is equal to
//           length.
//  Side Effect: N/A
//
	bool isNormEqualTo (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantEqualTo(getNormSquared(),
		                               length * length);
	}

//
//  isNormLessThan
//
//  Purpose: To determine if the norm of this Vector3 is less
//           than the specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           less than and less than or equal.  This function is
//           significantly faster than calculating the norm
//           itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector3 is less than
//           length.
//  Side Effect: N/A
//
	bool isNormLessThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(getNormSquared(),
		                                length * length);
	}

//
//  isNormGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 is greater
//           than the specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           greater than and greater than or equal.  This
//           function is significantly faster than calculating
//           the norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector3 is greater than
//           length.
//  Side Effect: N/A
//
	bool isNormGreaterThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(length * length,
		                                getNormSquared());
	}

//
//  isNormEqualTo
//
//  Purpose: To determine if the norm of this Vector3 is equal
//           to the norm of the specified Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than calculating the norms
//           themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector3 is equal to the
//           norm of other.
//  Side Effect: N/A
//
	bool isNormEqualTo (const Vector3& other) const
	{
		return isSquareTolerantEqualTo(getNormSquared(),
		                               other.getNormSquared());
	}

//
//  isNormLessThan
//
//  Purpose: To determine if the norm of this Vector3 is less
//           than the norm of the specified Vector3, according
//           to tolerance VECTOR3_NORM_TOLERANCE.  Because a
//           tolerance is used, there is no meaningful
//           difference between less than and less than or
//           equal.  This function is significantly faster than
//           calculating the norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector3 is less than the
//           norm of other.
//  Side Effect: N/A
//
	bool isNormLessThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(getNormSquared(),
		                                other.getNormSquared());
	}

//
//  isNormGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 is greater
//           than the norm of the specified Vector3, according
//           to tolerance VECTOR3_NORM_TOLERANCE.  Because a
//           tolerance is used, there is no meaningful
//           difference between greater than and greater than or
//           equal.  This function is significantly faster than
//           calculating the norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector3 is greater than
//           the norm of other.
//  Side Effect: N/A
//
	bool isNormGreaterThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(other.getNormSquared(),
		                                getNormSquared());
	}

//
//  getNormXY
//
//  Purpose: To determine the norm of this Vector3 projected to
//           the XY plane.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The norm of this Vector3 projected to the XY plane.
//  Side Effect: N/A
//
	double getNormXY () const
	{
		return sqrt(x * x + y * y);
	}

//
//  getNormXZ
//
//  Purpose: To determine the norm of this Vector3 projected to
//           the XZ plane.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The norm of this Vector3 projected to the XZ plane.
//  Side Effect: N/A
//
	double getNormXZ () const
	{
		return sqrt(x * x + z * z);
	}

//
//  getNormYZ
//
//  Purpose: To determine the norm of this Vector3 projected to
//           the YZ plane.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The norm of this Vector3 projected to the YZ plane.
//  Side Effect: N/A
//
	double getNormYZ () const
	{
		return sqrt(y * y + z * z);
	}

//
//  getNormXYSquared
//
//  Purpose: To determine the square of the norm of this Vector3
//           projected to the XY plane.  This is significantly
//           faster than caculating the projected norm itself.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The square of the norm of this Vector3 projected to
//           the XY plane.
//  Side Effect: N/A
//
	double getNormXYSquared () const
	{
		return x * x + y * y;
	}

//
//  getNormXZSquared
//
//  Purpose: To determine the square of the norm of this Vector3
//           projected to the XZ plane.  This is significantly
//           faster than caculating the projected norm itself.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The square of the norm of this Vector3 projected to
//           the XZ plane.
//  Side Effect: N/A
//
	double getNormXZSquared () const
	{
		return x * x + z * z;
	}

//
//  getNormYZSquared
//
//  Purpose: To determine the square of the norm of this Vector3
//           projected to the YZ plane.  This is significantly
//           faster than caculating the projected norm itself.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The square of the norm of this Vector3 projected to
//           the YZ plane.
//  Side Effect: N/A
//
	double getNormYZSquared () const
	{
		return y * y + z * z;
	}

//
//  isNormXYEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is equal to the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is equal to length.
//  Side Effect: N/A
//
	double isNormXYEqualTo (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantEqualTo(getNormXYSquared(),
		                               length * length);
	}

//
//  isNormXYLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is less than the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is less than length.
//  Side Effect: N/A
//
	double isNormXYLessThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(getNormXYSquared(),
		                                length * length);
	}

//
//  isNormXYGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is greater than the specified
//           value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is greater than length.
//  Side Effect: N/A
//
	double isNormXYGreaterThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(length * length,
		                                getNormXYSquared());
	}

//
//  isNormXYEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is equal to the projected norm of
//           pecified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is equal to the projection of the
//           norm of other.
//  Side Effect: N/A
//
	double isNormXYEqualTo (const Vector3& other) const
	{
		return isSquareTolerantEqualTo(
		                              getNormXYSquared(),
		                              other.getNormXYSquared());
	}

//
//  isNormXYLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is less than the projected norm
//           of the specified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is less than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormXYLessThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                              getNormXYSquared(),
		                              other.getNormXYSquared());
	}

//
//  isNormXYGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XY plane is greater than the projected
//           norm of the specified Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This is
//           significantly faster than calculating the 2 norms
//           themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XY plane is greater than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormXYGreaterThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                               other.getNormXYSquared(),
		                               getNormXYSquared());
	}

//
//  isNormXZEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is equal to the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is equal to length.
//  Side Effect: N/A
//
	double isNormXZEqualTo (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantEqualTo(getNormXZSquared(),
		                               length * length);
	}

//
//  isNormXZLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is less than the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is less than length.
//  Side Effect: N/A
//
	double isNormXZLessThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(getNormXZSquared(),
		                                length * length);
	}

//
//  isNormXZGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is greater than the specified
//           value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is greater than length.
//  Side Effect: N/A
//
	double isNormXZGreaterThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(length * length,
		                                getNormXZSquared());
	}

//
//  isNormXZEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is equal to the projected norm of
//           pecified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is equal to the projection of the
//           norm of other.
//  Side Effect: N/A
//
	double isNormXZEqualTo (const Vector3& other) const
	{
		return isSquareTolerantEqualTo(
		                              getNormXZSquared(),
		                              other.getNormXZSquared());
	}

//
//  isNormXZLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is less than the projected norm
//           of the specified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is less than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormXZLessThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                              getNormXZSquared(),
		                              other.getNormXZSquared());
	}

//
//  isNormXZGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the XZ plane is greater than the projected
//           norm of the specified Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This is
//           significantly faster than calculating the 2 norms
//           themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the XZ plane is greater than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormXZGreaterThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                               other.getNormXZSquared(),
		                               getNormXZSquared());
	}

//
//  isNormYZEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is equal to the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is equal to length.
//  Side Effect: N/A
//
	double isNormYZEqualTo (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantEqualTo(getNormYZSquared(),
		                               length * length);
	}

//
//  isNormYZLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is less than the specified value,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This is significantly faster than calculating the
//           norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is less than length.
//  Side Effect: N/A
//
	double isNormYZLessThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(getNormYZSquared(),
		                                length * length);
	}

//
//  isNormYZGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is greater than the specified
//           value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is greater than length.
//  Side Effect: N/A
//
	double isNormYZGreaterThan (double length) const
	{
		assert(length >= 0.0);

		return isSquareTolerantLessThan(length * length,
		                                getNormYZSquared());
	}

//
//  isNormYZEqualTo
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is equal to the projected norm of
//           pecified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is equal to the projection of the
//           norm of other.
//  Side Effect: N/A
//
	double isNormYZEqualTo (const Vector3& other) const
	{
		return isSquareTolerantEqualTo(
		                              getNormYZSquared(),
		                              other.getNormYZSquared());
	}

//
//  isNormYZLessThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is less than the projected norm
//           of the specified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This is significantly
//           faster than calculating the 2 norms themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is less than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormYZLessThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                              getNormYZSquared(),
		                              other.getNormYZSquared());
	}

//
//  isNormYZGreaterThan
//
//  Purpose: To determine if the norm of this Vector3 projected
//           onto the YZ plane is greater than the projected
//           norm of the specified Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This is
//           significantly faster than calculating the 2 norms
//           themselves.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s): N/A
//  Returns: Whether the norm of the projection of this Vector3
//           onto the YZ plane is greater than the projection of
//           the norm of other.
//  Side Effect: N/A
//
	double isNormYZGreaterThan (const Vector3& other) const
	{
		return isSquareTolerantLessThan(
		                               other.getNormYZSquared(),
		                               getNormYZSquared());
	}

//
//  isAllComponentsNonZero
//
//  Purpose: To determine if all the elements of this Vector3
//           are non-zero values.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector3 is equal to 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonZero () const
	{
		if(x == 0.0) return false;
		if(y == 0.0) return false;
		if(z == 0.0) return false;
		return true;
	}

//
//  isAllComponentsPositive
//
//  Purpose: To determine if all the elements of this Vector3
//           are positive.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector3 is less than or
//           equal to 0.0, false is returned.  Otherwise, true
//           is returned.
//  Side Effect: N/A
//
	bool isAllComponentsPositive () const
	{
		if(x <= 0.0) return false;
		if(y <= 0.0) return false;
		if(z <= 0.0) return false;
		return true;
	}

//
//  isAllComponentsNegative
//
//  Purpose: To determine if all the elements of this Vector3
//           are negative.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector3 is reater than or
//           equal to 0.0, false is returned.  Otherwise, true
//           is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNegative () const
	{
		if(x >= 0.0) return false;
		if(y >= 0.0) return false;
		if(z >= 0.0) return false;
		return true;
	}

//
//  isAllComponentsNonPositive
//
//  Purpose: To determine if all the elements of this Vector3
//           are non-positive.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector3 is greater than 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonPositive () const
	{
		if(x > 0.0) return false;
		if(y > 0.0) return false;
		if(z > 0.0) return false;
		return true;
	}

//
//  isAllComponentsNonNegative
//
//  Purpose: To determine if all the elements of this Vector3
//           are non-negative.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector3 is less than 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonNegative () const
	{
		if(x < 0.0) return false;
		if(y < 0.0) return false;
		if(z < 0.0) return false;
		return true;
	}

//
//  isAllComponentsEqualTo
//
//  Purpose: To determine if all components of this Vector3 are
//           equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is equal to
//           value.
//  Side Effect: N/A
//
	bool isAllComponentsEqualTo (double value) const
	{
		assert(isFinite());

		if(x != value) return false;
		if(y != value) return false;
		if(z != value) return false;
		return true;
	}

//
//  isAllComponentsNotEqualTo
//
//  Purpose: To determine if all components of this Vector3 are
//           not equal to the specified value.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is not equal
//           to value.
//  Side Effect: N/A
//
	bool isAllComponentsNotEqualTo (double value) const
	{
		assert(isFinite());

		if(x == value) return false;
		if(y == value) return false;
		if(z == value) return false;
		return true;
	}

//
//  isAllComponentsLessThan
//
//  Purpose: To determine if all components of this Vector3 are
//           less than the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is less than
//           value.
//  Side Effect: N/A
//
	bool isAllComponentsLessThan (double value) const
	{
		assert(isFinite());

		if(x >= value) return false;
		if(y >= value) return false;
		if(z >= value) return false;
		return true;
	}

//
//  isAllComponentsLessThanOrEqual
//
//  Purpose: To determine if all components of this Vector3 are
//           less than or equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is less than
//           or equal to value.
//  Side Effect: N/A
//
	bool isAllComponentsLessThanOrEqual (double value) const
	{
		assert(isFinite());

		if(x > value) return false;
		if(y > value) return false;
		if(z > value) return false;
		return true;
	}

//
//  isAllComponentsGreaterThan
//
//  Purpose: To determine if all components of this Vector3 are
//           greater than the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is greater
//           than value.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThan (double value) const
	{
		assert(isFinite());

		if(x <= value) return false;
		if(y <= value) return false;
		if(z <= value) return false;
		return true;
	}

//
//  isAllComponentsGreaterThanOrEqual
//
//  Purpose: To determine if all components of this Vector3 are
//           greater than or equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector3 is greater
//           than or equal to value.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThanOrEqual (
	                                     double value) const
	{
		assert(isFinite());

		if(x < value) return false;
		if(y < value) return false;
		if(z < value) return false;
		return true;
	}

//
//  isAllComponentsNotEqualTo
//
//  Purpose: To determine if all components of this Vector3 are
//           not equal to than the corresponding components of
//           another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector3 is not equal
//           to the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsNotEqualTo (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x == other.x) return false;
		if(y == other.y) return false;
		if(z == other.z) return false;
		return true;
	}

//
//  isAllComponentsLessThan
//
//  Purpose: To determine if all components of this Vector3 are
//           less than the corresponding components of
//           another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector3 is less than
//           the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsLessThan (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x >= other.x) return false;
		if(y >= other.y) return false;
		if(z >= other.z) return false;
		return true;
	}

//
//  isAllComponentsLessThanOrEqual
//
//  Purpose: To determine if all components of this Vector3 are
//           less than or equal to the corresponding
//           components of another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector3 is less than
//           or equal to the corresponding component of
//           other.
//  Side Effect: N/A
//
	bool isAllComponentsLessThanOrEqual (
	                             const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x > other.x) return false;
		if(y > other.y) return false;
		if(z > other.z) return false;
		return true;
	}

//
//  isAllComponentsGreaterThan
//
//  Purpose: To determine if all components of this Vector3 are
//           greater than the corresponding components of
//           another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector3 is greater
//           than the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThan (
	                             const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x <= other.x) return false;
		if(y <= other.y) return false;
		if(z <= other.z) return false;
		return true;
	}

//
//  isAllComponentsGreaterThanOrEqual
//
//  Purpose: To determine if all components of this Vector3 are
//           greater than or equal to the corresponding
//           components of another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector3 is greater
//           than or equal to the corresponding component
//           of other.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThanOrEqual (
	                             const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x < other.x) return false;
		if(y < other.y) return false;
		if(z < other.z) return false;
		return true;
	}

//
//  getComponentX
//
//  Purpose: To create another Vector3 with the same X component
//           as this Vector3 and the Y and Z components set to
//           0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the X
//           component.
//  Side Effect: N/A
//
	Vector3 getComponentX () const
	{
		return Vector3(x, 0.0, 0.0);
	}

//
//  getComponentY
//
//  Purpose: To create another Vector3 with the same Y component
//           as this Vector3 and the X and Z components set to
//           0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the Y
//           component.
//  Side Effect: N/A
//
	Vector3 getComponentY () const
	{
		return Vector3(0.0, y, 0.0);
	}

//
//  getComponentZ
//
//  Purpose: To create another Vector3 with the same Z component
//           as this Vector3 and the X and Y components set to
//           0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the Z
//           component.
//  Side Effect: N/A
//
	Vector3 getComponentZ () const
	{
		return Vector3(0.0, 0.0, z);
	}

//
//  getComponentXY
//
//  Purpose: To create another Vector3 with the same X and Y
//           components  as this Vector3 and the Z component set
//           to 0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the X and Y
//           components.
//  Side Effect: N/A
//
	Vector3 getComponentXY () const
	{
		return Vector3(x, y, 0.0);
	}

//
//  getComponentXZ
//
//  Purpose: To create another Vector3 with the same X and Z
//           components  as this Vector3 and the Y component set
//           to 0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the X and Z
//           components.
//  Side Effect: N/A
//
	Vector3 getComponentXZ () const
	{
		return Vector3(x, 0.0, z);
	}

//
//  getComponentYZ
//
//  Purpose: To create another Vector3 with the same Y and Z
//           components  as this Vector3 and the X component set
//           to 0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector3 containing only the Y and Z
//           components.
//  Side Effect: N/A
//
	Vector3 getComponentYZ () const
	{
		return Vector3(0.0, y, z);
	}

//
//  getNormalized
//
//  Purpose: To create a normalized copy of this Vector3.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//  Returns: A Vector3 with the same direction as this Vector3
//           and a norm of 1.0.
//  Side Effect: N/A
//
	Vector3 getNormalized () const
	{
		assert(isFinite());
		assert(!isZero());

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();
		return Vector3(x * norm_ratio,
		               y * norm_ratio,
		               z * norm_ratio);
	}

//
//  getNormalizedSafe
//
//  Purpose: To create a normalized copy of this Vector3 without
//           crashing if this Vector3 is the zero vector.  This
//           function is slower than the getNormalized function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: If this Vector3 is the zero vector, (1.0, 0.0, 0.0)
//           is returned.  Otherwise, a Vector3 with the same
//           direction as this Vector3 and a norm of 1.0 is
//           returned.
//  Side Effect: N/A
//
	Vector3 getNormalizedSafe () const
	{
		assert(isFinite());

		if(isZero())
			return Vector3(1.0, 0.0, 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();
		return Vector3(x * norm_ratio,
		               y * norm_ratio,
		               z * norm_ratio);
	}

//
//  getCopyWithNorm
//
//  Purpose: To create a Vector3 with the same direction as this
//           Vector3 and the specified norm.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> norm >= 0.0
//  Returns: A Vector3 with the same direction as this Vector3
//           and a norm of norm.
//  Side Effect: N/A
//
	Vector3 getCopyWithNorm (double norm) const
	{
		assert(isFinite());
		assert(!isZero());
		assert(norm >= 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = norm / getNorm();
		return Vector3(x * norm_ratio,
		               y * norm_ratio,
		               z * norm_ratio);
	}

//
//  getCopyWithNormSafe
//
//  Purpose: To create a Vector3 with the same direction as this
//           Vector3 and the specified norm.  This function will
//           not crash if this Vector3 is the zero vector, but
//           is slower than the getCopyWithNorm function.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: If this Vector3 is the zero vector,
//           (norm, 0.0, 0.0) is returned.  Otherwise, a Vector3
//           with the same direction as this Vector3 and a norm
//           of norm is returned.
//  Side Effect: N/A
//
	Vector3 getCopyWithNormSafe (double norm) const
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isZero())
			return Vector3(norm, 0.0, 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = norm / getNorm();
		return Vector3(x * norm_ratio,
		               y * norm_ratio,
		               z * norm_ratio);
	}

//
//  getTruncated
//
//  Purpose: To create a Vector3 with the same direction as this
//           Vector3 and a norm no greater than the specified
//           value.
//  Parameter(s):
//    <1> norm: The new maximum norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: If this Vector3 has a norm greater than norm, a
//           Vector3 with the same direction as this Vector3 and
//           a norm of norm is returned.  Otherwise, a copy of
//           this Vector3 is returned.
//  Side Effect: N/A
//
	Vector3 getTruncated (double norm) const
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isNormGreaterThan(norm))
		{
			double norm_ratio = norm / getNorm();
			return Vector3(x * norm_ratio,
			               y * norm_ratio,
			               z * norm_ratio);
		}
		else
			return *this;  // invokes copy constructor
	}

//
//  setZero
//
//  Purpose: To change this Vector3 to be the zero vector.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector3 is set to (0.0, 0.0, 0.0).
//
	void setZero ()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

//
//  set
//
//  Purpose: To change this Vector3 to have the specified
//           elements.
//  Parameter(s):
//    <1> X: The new X element
//    <2> Y: The new Y element
//    <3> Z: The new Z element
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector3 is set to (X, Y, Z).
//
	void set (double X, double Y, double Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

//
//  setAll
//
//  Purpose: To change all the element of this Vector3 to be the
//           specified value.  This function is slightly more
//           efficient than calling set with the same parameter
//           multiple times.
//  Parameter(s):
//    <1> v: The new value
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector3 is set to (v, v, v).
//
	void setAll (double v)
	{
		x = v;
		y = v;
		z = v;
	}

//
//  addComponents
//
//  Purpose: To increase the elements of this Vector3 by the
//           specified amounts.
//  Parameter(s):
//    <1> X: The increase for the X element
//    <2> Y: The increase for the Y element
//    <3> Z: The increase for the Z element
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector3 is set to (x + X, y + Y, z + Z).
//
	void addComponents (double X, double Y, double Z)
	{
		x += X;
		y += Y;
		z += Z;
	}

//
//  addComponentsAll
//
//  Purpose: To increase all the element of this Vector3 by the
//           specified value.  This function is slightly more
//           efficient than calling add with the same parameter
//           multiple times.
//  Parameter(s):
//    <1> v: The increase for the elements
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector3 is set to (x + v, y + v, z + v).
//
	void addComponentsAll (double v)
	{
		x += v;
		y += v;
		z += v;
	}

//
//  normalize
//
//  Purpose: To change this Vector3 have a norm of 1.0.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//  Returns: N/A
//  Side Effect: This Vector3 is set to have a norm of 1.0.  The
//               direction of this Vector3 is unchanged.
//
	void normalize ()
	{
		assert(isFinite());
		assert(!isZero());

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();

		x *= norm_ratio;
		y *= norm_ratio;
		z *= norm_ratio;

		assert(isNormal());
	}

//
//  normalizeSafe
//
//  Purpose: To change this Vector3 have a norm of 1.0.  This
//           function is slower than the normalize function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: If this Vector3 is the zero vector, it is set
//               to be (1.0, 0.0, 0.0).  Otherwise, it is set
//               to have a norm of 1.0 and the direction of this
//               Vector3 is unchanged.
//
	void normalizeSafe ()
	{
		assert(isFinite());

		if(isZero())
			set(1.0, 0.0, 0.0);
		else
		{
			assert(getNorm() != 0.0);
			double norm_ratio = 1.0 / getNorm();

			x *= norm_ratio;
			y *= norm_ratio;
			z *= norm_ratio;

			assert(isNormal());
		}
	}

//
//  setNorm
//
//  Purpose: To change the norm of this Vector3.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> norm >= 0.0
//  Returns: N/A
//  Side Effect: This Vector3 is set to have a norm of norm.
//               The direction of this Vector3 is unchanged.
//
	void setNorm (double norm)
	{
		assert(isFinite());
		assert(!isZero());
		assert(norm >= 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = norm / getNorm();

		x *= norm_ratio;
		y *= norm_ratio;
		z *= norm_ratio;

		assert(isNormEqualTo(norm));
	}

//
//  setNormSafe
//
//  Purpose: To change the norm of this Vector3 to the specified
//           value.  This function is slower than the normalize
//           function.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: N/A
//  Side Effect: If this Vector3 is the zero vector, it is set
//               to be (norm, 0.0, 0.0).  Otherwise, it is set
//               to have a norm of 1.0 and the direction of this
//               Vector3 is unchanged.
//
	void setNormSafe (double norm)
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isZero())
			set(norm, 0.0, 0.0);
		else
		{
			assert(getNorm() != 0.0);
			double norm_ratio = norm / getNorm();

			x *= norm_ratio;
			y *= norm_ratio;
			z *= norm_ratio;

			assert(isNormEqualTo(norm));
		}
	}

//
//  truncate
//
//  Purpose: To reduce the norm of this Vector3 to the specified
//           if it is currently greater.
//  Parameter(s):
//    <1> norm: The new maximum norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: N/A
//  Side Effect: If this Vector3 has a norm greater than norm,
//               is set to have a norm of norm.  Otherwise there
//               is no effect.  In either case, the direction of
//               this Vector3 is unchanged.
//
	void truncate (double norm)
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isNormGreaterThan(norm))
			setNorm(norm);
		assert(isNormLessThan(norm));
	}

//
//  getComponentProduct
//
//  Purpose: To calculate the component-wise product of this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector3 with elements
//           (x * other.x, y * other.y, z * other.z).
//  Side Effect: N/A
//
	Vector3 getComponentProduct (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector3(x * other.x,
		               y * other.y,
		               z * other.z);
	}

//
//  getComponentRatio
//
//  Purpose: To calculate the component-wise ratio of this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> other.isAllComponentsNonZero()
//  Returns: A Vector3 with elements
//           (x / other.x, y / other.y, z / other.z).
//  Side Effect: N/A
//
	Vector3 getComponentRatio (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(other.isAllComponentsNonZero());

		return Vector3(x / other.x,
		               y / other.y,
		               z / other.z);
	}

//
//  getComponentRatioSafe
//
//  Purpose: To calculate the component-wise ratio of this
//           Vector3 and another Vector3 without crashing
//           if one of the elements of the second Vector3
//           is zero.  This function is slower than
//           componentRatio.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector3 with elements
//           (x / other.x, y / other.y, z / other.z).
//           If any of the elements in other is zero,
//           the corresponding element of this Vector3 is
//           returned for that element instead of a ratio.
//  Side Effect: N/A
//
	Vector3 getComponentRatioSafe (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector3((other.x != 0.0) ? (x / other.x) : x,
		               (other.y != 0.0) ? (y / other.y) : y,
		               (other.z != 0.0) ? (z / other.y) : z);
	}

//
//  getNormRatio
//
//  Purpose: To calculate the ratio of the norms of this Vector3
//           and another parallel Vector3.  This function takes
//           the direction of the Vector3s into account, so if
//           they are facing in opposite directions, the ratio
//           will be negative.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> isParallel(other)
//    <4> !other.isZero()
//  Returns: The ratio of the norms of this Vector3 and other.
//  Side Effect: N/A
//
	double getNormRatio (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(isParallel(other));
		assert(!other.isZero());

		if(other.x != 0.0)
			return x / other.x;
		else if(other.y != 0.0)
			return y / other.y;
		else
		{
			assert(other.z != 0.0);
			return z / other.z;
		}
	}

//
//  getNormRatioSafe
//
//  Purpose: To calculate the ratio of the norms of this Vector3
//           and another parallel Vector3 without crashing if
//           all of the elements of the second Vector3 are zero.
//           This function takes the direction of the Vector3s
//           into account, so if the Vector3s are facing in
//           opposite directions the ratio will be negative.
//           This function is slower than getNormRatio.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> isParallel(other)
//  Returns: The ratio of the norms of this Vector3 and other.
//           If all of the elements in other are zero, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getNormRatioSafe (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(isParallel(other));

		if(other.x != 0.0)
			return x / other.x;
		else if(other.y != 0.0)
			return y / other.y;
		else if(other.z != 0.0)
			return z / other.z;
		else
			return 0.0;
	}

//
//  dotProduct
//
//  Purpose: To determine the dot/scaler/inner product of this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: *this (dot) other.
//  Side Effect: N/A
//
	double dotProduct (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return x * other.x + y * other.y + z * other.z;
	}

//
//  crossProduct
//
//  Purpose: To determine the cross/vector product of this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: *this (cross) other.
//  Side Effect: N/A
//
	Vector3 crossProduct (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector3(y * other.z - z * other.y,
		               z * other.x - x * other.z,
		               x * other.y - y * other.x);
	}

//
//  getMinComponents
//
//  Purpose: To determine the minimum values for each component
//           from this Vector3 and the specified value.
//  Parameter(s):
//    <1> n: The other value
//  Precondition(s):
//    <1> isFinite()
//  Returns: A Vector3, each component of which is the smaller
//           of n and the corresponding component of this
//           Vector3.
//  Side Effect: N/A
//
	Vector3 getMinComponents (double n) const
	{
		assert(isFinite());

		return Vector3((x < n) ? x : n,
		               (y < n) ? y : n,
		               (z < n) ? z : n);
	}

//
//  getMinComponents
//
//  Purpose: To determine the minimum values for each component
//           from this Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector3, each component of which is the smaller
//           of the corresponding components of this Vector3 and
//           other.
//  Side Effect: N/A
//
	Vector3 getMinComponents (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector3((x < other.x) ? x : other.x,
		               (y < other.y) ? y : other.y,
		               (z < other.z) ? z : other.z);
	}

//
//  getMaxComponents
//
//  Purpose: To determine the maximum values for each component
//           from this Vector3 and the specified value.
//  Parameter(s):
//    <1> n: The other value
//  Precondition(s):
//    <1> isFinite()
//  Returns: A Vector3, each component of which is the larger of
//           n and the corresponding component of this Vector3.
//  Side Effect: N/A
//
	Vector3 getMaxComponents (double n) const
	{
		assert(isFinite());

		return Vector3((x > n) ? x : n,
		               (y > n) ? y : n,
		               (z > n) ? z : n);
	}

//
//  getMaxComponents
//
//  Purpose: To determine the maximum values for each component
//           from this Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector3, each component of which is the larger of
//           the corresponding components of this Vector3 and
//           other.
//  Side Effect: N/A
//
	Vector3 getMaxComponents (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector3((x > other.x) ? x : other.x,
		               (y > other.y) ? y : other.y,
		               (z > other.z) ? z : other.z);
	}

//
//  getStaturated
//
//  Purpose: To create a Vector3 with each component in the
//           range between 0.0 and 1.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A Vector3, each component of which is based on the
//           corresponding component of this Vector3.  If the
//           component of this Vector3 is less than 0.0, the
//           component of the new Vector3 contains 0.0.  If the
//           component is larger than 1.0, it contains the 1.0.
//           Otherwise, the new Vector3 contains the
//           corresponing component from this Vector3.
//  Side Effect: N/A
//
	Vector3 getStaturated (double min, double max) const
	{
		assert(isFinite());
		assert(min <= max);

		return Vector3((x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x,
		               (y < 0.0) ? 0.0 : (y > 1.0) ? 1.0 : y,
		               (z < 0.0) ? 0.0 : (z > 1.0) ? 1.0 : z);
	}

//
//  getClampedComponents
//
//  Purpose: To create a Vector3 with each component in the
//           range between the specified values.
//  Parameter(s):
//    <1> min: The minimum value
//    <2> max: The minimum value
//  Precondition(s):
//    <1> isFinite()
//    <2> min <= max
//  Returns: A Vector3, each component of which is based on the
//           corresponding component of this Vector3, min, and
//           max.  If the component of this Vector3 is less than
//           min, the component of the new Vector3 contains min.
//           If the component is larger than max, it contains
//           max.  Otherwise, the new Vector3 contains the
//           corresponing component from this Vector3.
//  Side Effect: N/A
//
	Vector3 getClampedComponents (double min, double max) const
	{
		assert(isFinite());
		assert(min <= max);

		return Vector3((x < min) ? min : (x > max) ? max : x,
		               (y < min) ? min : (y > max) ? max : y,
		               (z < min) ? min : (z > max) ? max : z);
	}

//
//  getClampedComponents
//
//  Purpose: To create a Vector3 with each component in the
//           range between the specified Vector3s.
//  Parameter(s):
//    <1> min: A Vector3 containing the minimum values
//    <2> max: A Vector3 containing the minimum values
//  Precondition(s):
//    <1> isFinite()
//    <2> min.isFinite()
//    <3> max.isFinite()
//    <4> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A Vector3, each component of which is based on the
//           corresponding component of this Vector3, min, and
//           max.  If the component of this Vector3 is less than
//           the corresponing component of min, the component of
//           new Vector3 contains the component of min.  If the
//           component is larger than max, it contains the
//           component of max.  Otherwise, the new Vector3
//           contains the corresponing component from this
//           Vector3.
//  Side Effect: N/A
//
	Vector3 getClampedComponents (const Vector3& min,
	                              const Vector3& max) const
	{
		assert(isFinite());
		assert(min.isFinite());
		assert(max.isFinite());
		assert(min.isAllComponentsLessThanOrEqual(max));

		return Vector3((x < min.x) ? min.x
		                           : ((x > max.x) ? max.x : x),
		               (y < min.y) ? min.y
		                           : ((y > max.y) ? max.y : y),
		               (z < min.z) ? min.z
		                           : ((z > max.z) ? max.z : z));
	}

//
//  getDistance
//
//  Purpose: To determine the Euclidian distance between this
//           Vector3 and another Vector3.  If you only need to
//           compare the distance to another Vector3 with some
//           value, consider using one of the isDistanceEqual,
//           isDistanceLessThan, or isDistanceGreaterThan
//           funtions.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Euclidean distance between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getDistance (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;
		double diff_z = z - other.z;

		return sqrt(diff_x * diff_x +
		            diff_y * diff_y +
		            diff_z * diff_z);
	}

//
//  getDistanceSquared
//
//  Purpose: To determine the square of the Euclidian distance
//           between this Vector3 and another Vector3.  This
//           function is significantly faster than
//           getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The square of the Euclidian distance between this
//           Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceSquared (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;
		double diff_z = z - other.z;

		return diff_x * diff_x +
		       diff_y * diff_y +
		       diff_z * diff_z;
	}

//
//  isDistanceEqualTo
//
//  Purpose: To determine if the Euclidian distance between this
//           Vector3 and another Vector3 is equal to the
//           specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The comparison distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector3
//           and other is equal to distance.
//  Side Effect: N/A
//
	bool isDistanceEqualTo (const Vector3& other,
	                        double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantEqualTo(
		                              getDistanceSquared(other),
		                              distance * distance);
	}

//
//  isDistanceLessThan
//
//  Purpose: To determine if the Euclidian distance between this
//           Vector3 and another Vector3 is less than the
//           specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector3
//           and other is less than distance.
//  Side Effect: N/A
//
	bool isDistanceLessThan (const Vector3& other,
	                         double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                              getDistanceSquared(other),
		                              distance * distance);
	}

//
//  isDistanceGreaterThan
//
//  Purpose: To determine if the Euclidian distance between this
//           Vector3 and another Vector3 is greater than the
//           specified value, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector3
//           and other is greater than distance.
//  Side Effect: N/A
//
	bool isDistanceGreaterThan (const Vector3& other,
	                            double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                             distance * distance,
		                             getDistanceSquared(other));
	}

//
//  getDistanceXY
//
//  Purpose: To determine the Euclidian distance along the XY
//           plane between this Vector3 and another Vector3.  If
//           you only need to compare the distance to another
//           value, consider using one of the isDistanceEqual,
//           isDistanceLessThan, or isDistanceGreaterThan
//           funtions.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Euclidean distance along the XY plane between
//           this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceXY (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;

		return sqrt(diff_x * diff_x +
		            diff_y * diff_y);
	}

//
//  getDistanceXZ
//
//  Purpose: To determine the Euclidian distance along the XZ
//           plane between this Vector3 and another Vector3.  If
//           you only need to compare the distance to another
//           value, consider using one of the isDistanceEqual,
//           isDistanceLessThan, or isDistanceGreaterThan
//           funtions.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Euclidean distance along the XZ plane between
//           this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceXZ (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_z = z - other.z;

		return sqrt(diff_x * diff_x +
		            diff_z * diff_z);
	}

//
//  getDistanceYZ
//
//  Purpose: To determine the Euclidian distance along the YZ
//           plane between this Vector3 and another Vector3.  If
//           you only need to compare the distance to another
//           value, consider using one of the isDistanceEqual,
//           isDistanceLessThan, or isDistanceGreaterThan
//           funtions.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Euclidean distance along the YZ plane between
//           this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceYZ (const Vector3& other) const
	{
		double diff_y = y - other.y;
		double diff_z = z - other.z;

		return sqrt(diff_y * diff_y +
		            diff_z * diff_z);
	}

//
//  getDistanceXYSquared
//
//  Purpose: To determine the square of the Euclidian distance
//           along the XY plane between this Vector3 and another
//           Vector3.  This function is significantly faster
//           than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The square of the Euclidean distance along the XY
//           plane between this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceXYSquared (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;

		return diff_x * diff_x +
		       diff_y * diff_y;
	}

//
//  getDistanceXZSquared
//
//  Purpose: To determine the square of the Euclidian distance
//           along the XZ plane between this Vector3 and another
//           Vector3.  This function is significantly faster
//           than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The square of the Euclidean distance along the XZ
//           plane between this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceXZSquared (const Vector3& other) const
	{
		double diff_x = x - other.x;
		double diff_z = z - other.z;

		return diff_x * diff_x +
		       diff_z * diff_z;
	}

//
//  getDistanceYZSquared
//
//  Purpose: To determine the square of the Euclidian distance
//           along the YZ plane between this Vector3 and another
//           Vector3.  This function is significantly faster
//           than getDistance().
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The square of the Euclidean distance along the YZ
//           plane between this Vector3 and other.
//  Side Effect: N/A
//
	double getDistanceYZSquared (const Vector3& other) const
	{
		double diff_y = y - other.y;
		double diff_z = z - other.z;

		return diff_y * diff_y +
		       diff_z * diff_z;
	}

//
//  isDistanceXYEqualTo
//
//  Purpose: To determine if the Euclidian distance along the XY
//           plane between this Vector3 and another Vector3 is
//           equal to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXY().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The comparison distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XY plane
//           between this Vector3 and other is equal to
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXYEqualTo (const Vector3& other,
	                          double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantEqualTo(
		                            getDistanceXYSquared(other),
		                            distance * distance);
	}

//
//  isDistanceXYLessThan
//
//  Purpose: To determine if the Euclidian distance along the XY
//           plane between this Vector3 and another Vector3 is
//           less than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXY().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XY plane
//           between this Vector3 and other is less than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXYLessThan (const Vector3& other,
	                           double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                            getDistanceXYSquared(other),
		                            distance * distance);
	}

//
//  isDistanceXYGreaterThan
//
//  Purpose: To determine if the Euclidian distance along the XY
//           plane between this Vector3 and another Vector3 is
//           greater than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXY().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XY plane
//           between this Vector3 and other is greater than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXYGreaterThan (const Vector3& other,
	                              double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                           distance * distance,
		                           getDistanceXYSquared(other));
	}

//
//  isDistanceXZEqualTo
//
//  Purpose: To determine if the Euclidian distance along the XZ
//           plane between this Vector3 and another Vector3 is
//           equal to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The comparison distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XZ plane
//           between this Vector3 and other is equal to
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXZEqualTo (const Vector3& other,
	                          double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantEqualTo(
		                            getDistanceXZSquared(other),
		                            distance * distance);
	}

//
//  isDistanceXZLessThan
//
//  Purpose: To determine if the Euclidian distance along the XZ
//           plane between this Vector3 and another Vector3 is
//           less than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XZ plane
//           between this Vector3 and other is less than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXZLessThan (const Vector3& other,
	                           double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                            getDistanceXZSquared(other),
		                            distance * distance);
	}

//
//  isDistanceXZGreaterThan
//
//  Purpose: To determine if the Euclidian distance along the XZ
//           plane between this Vector3 and another Vector3 is
//           greater than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceXZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the XZ plane
//           between this Vector3 and other is greater than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceXZGreaterThan (const Vector3& other,
	                              double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                           distance * distance,
		                           getDistanceXZSquared(other));
	}

//
//  isDistanceYZEqualTo
//
//  Purpose: To determine if the Euclidian distance along the YZ
//           plane between this Vector3 and another Vector3 is
//           equal to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceYZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The comparison distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the YZ plane
//           between this Vector3 and other is equal to
//           distance.
//  Side Effect: N/A
//
	bool isDistanceYZEqualTo (const Vector3& other,
	                          double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantEqualTo(
		                            getDistanceYZSquared(other),
		                            distance * distance);
	}

//
//  isDistanceYZLessThan
//
//  Purpose: To determine if the Euclidian distance along the YZ
//           plane between this Vector3 and another Vector3 is
//           less than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceYZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the YZ plane
//           between this Vector3 and other is less than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceYZLessThan (const Vector3& other,
	                           double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                            getDistanceYZSquared(other),
		                            distance * distance);
	}

//
//  isDistanceYZGreaterThan
//
//  Purpose: To determine if the Euclidian distance along the YZ
//           plane between this Vector3 and another Vector3 is
//           greater than to the specified value, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This function is
//           significantly faster than getDistanceYZ().
//  Parameter(s):
//    <1> other: The other Vector3
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance along the YZ plane
//           between this Vector3 and other is greater than
//           distance.
//  Side Effect: N/A
//
	bool isDistanceYZGreaterThan (const Vector3& other,
	                              double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                           distance * distance,
		                           getDistanceYZSquared(other));
	}

//
//  getManhattenDistance
//
//  Purpose: To determine the Manhatten distance between this
//           Vector3 and another Vector3.  This is the sum of
//           the differences between corresponding components.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Manhatten distance between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getManhattenDistance (const Vector3& other) const
	{
		return fabs(x - other.x) +
		       fabs(y - other.y) +
		       fabs(z - other.z);
	}

//
//  getChessboardDistance
//
//  Purpose: To determine the chessboard distance between this
//           Vector3 and another Vector3.  This is the largest
//           differences between corresponding components.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The chessboard distance between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getChessboardDistance (
	                             const Vector3& other) const
	{
		double dx = fabs(x - other.x);
		double dy = fabs(y - other.y);
		double dz = fabs(z - other.z);

		return (dx < dy) ? ((dy < dz) ? dz : dy)
		                 : ((dx < dz) ? dz : dx);
	}

//
//  isParallel
//
//  Purpose: To determine whether this Vector3 is parallel to
//           the specified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector3 is parallel to other.  The
//           zero vector is assumed to be parallel to all
//           Vector3s.
//  Side Effect: N/A
//
	bool isParallel (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(isZero() || other.isZero())
			return true;
		else if(fabs(getNormalized().getCosAngleNormal(
		                               other.getNormalized())) >
		        VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isParallelNormal
//
//  Purpose: To determine whether this Vector3 is parallel to
//           the specified normal Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This Vector3 is
//           assumed to also be a normal vector.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector3 is parallel to other.  The
//           zero vector is assumed to be parallel to all
//           Vector3s.
//  Side Effect: N/A
//
	bool isParallelNormal (const Vector3& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		if(fabs(getCosAngleNormal(other)) >
		   VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isSameDirection
//
//  Purpose: To determine whether this Vector3 is pointing in
//           the same direction as the specified Vector3,
//           according to tolerance VECTOR3_NORM_TOLERANCE.  To
//           be pointing in the same direction, 2 Vector3s must
//           be parallel and have the same sign on each of their
//           corresponding components.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector3 is pointing in the same
//           direction as other.  The zero vector is assumed to
//           point in the same direction as all Vector3s.
//  Side Effect: N/A
//
	bool isSameDirection (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(isZero() || other.isZero())
			return true;
		else if(getNormalized().getCosAngleNormal(
		                                other.getNormalized()) >
		        VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isSameDirectionNormal
//
//  Purpose: To determine whether this Vector3 is pointing in
//           the same direction as the specified normal Vector3,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//           This Vector3 is assumed to also be a normal vector.
//           To be pointing in the same direction, 2 Vector3s
//           must be parallel and have the same sign on each of
//           their corresponding components.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector3 is pointing in the same
//           direction as other.  The zero vector is assumed to
//           point in the same direction as all Vector3s.
//  Side Effect: N/A
//
	bool isSameDirectionNormal (const Vector3& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		if(getCosAngleNormal(other) >
		   VECTOR3_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isSameHemisphere
//
//  Purpose: To determine whether this Vector3 is pointing
//           within 90 degrees of the specified Vector3.  In
//           most cases, either this Vector3 or its negation
//           will be in the same hemisphere as the specified
//           Vector3, although this is not guarenteed in
//           borderline cases.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector3 is pointing within 90 degrees
//           of other.  The zero vector is assumed to be in the
//           same hemisphere as all Vector3s.
//  Side Effect: N/A
//
	bool isSameHemisphere (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(dotProduct(other) >= 0.0)
			return true;
		else
			return false;
	}

//
//  isOrthogonal
//
//  Purpose: To determine whether this Vector3 is orthogonal to
//           the specified Vector3, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  Orthogonal is the same as
//           perpendicular except that orthogonal is defined for
//           zero vectors.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector3 is orthogonal to other.  The
//           zero vector is orthogonal to all Vector3s including
//           itself.
//  Side Effect: N/A
//
	bool isOrthogonal (const Vector3& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(isZero() || other.isZero())
			return true;
		else if(fabs(getNormalized().getCosAngleNormal(
		                               other.getNormalized())) <
		        VECTOR3_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isOrthogonalNormal
//
//  Purpose: To determine whether this Vector3 is orthogonal to
//           the specified normla Vector3, according to
//           tolerance VECTOR3_NORM_TOLERANCE.  This Vector3 is
//           also assumed to be a normal vector.  Orthogonal is
//           the same as perpendicular except that orthogonal is
//           defined for zero vectors.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector3 is orthogonal to other.  The
//           zero vector is orthogonal to all Vector3s including
//           itself.
//  Side Effect: N/A
//
	bool isOrthogonalNormal (const Vector3& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		if(fabs(getCosAngleNormal(other)) <
		   VECTOR3_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  getProjection
//
//  Purpose: To determine the projection of this Vector3 onto
//           another Vector3.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> !project_onto.isZero()
//  Returns: The projection of this Vector3 onto project_onto.
//           This is the component of this Vector3 with the same
//           (or opposite) direction as project_onto.
//  Side Effect: N/A
//
	Vector3 getProjection (const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(!project_onto.isZero());

		double dot_product = dotProduct(project_onto);
		assert(project_onto.getNormSquared() != 0.0);
		double norm = dot_product /
		              project_onto.getNormSquared();

		assert((project_onto * norm).isParallel(project_onto));
		return project_onto * norm;
	}

//
//  getAntiProjection
//
//  Purpose: To determine the portion of this Vector3 that
//           remains after the projection onto another Vector3
//           is subtracted off.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> !project_onto.isZero()
//  Returns: This Vector3 minus its projection onto
//           project_onto. This is the component of this Vector3
//           at right angles to project_onto.
//  Side Effect: N/A
//
	Vector3 getAntiProjection (
	                      const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(!project_onto.isZero());

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector3::ZERO;

		Vector3 projection = getProjection(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getProjectionSafe
//
//  Purpose: To determine the projection of this Vector3 onto
//           another Vector3, without crashing if the other
//           Vector3 is zero.  This function is slower than
//           getProjection.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//  Returns: The projection of this Vector3 onto project_onto.
//           This is the component of this Vector3 with the same
//           (or opposite) direction as project_onto.  If
//           project_onto is zero, the zero vector is returned.
//  Side Effect: N/A
//
	Vector3 getProjectionSafe (
	                      const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());

		if(project_onto.isZero())
			return Vector3::ZERO;

		double dot_product = dotProduct(project_onto);
		assert(project_onto.getNormSquared() != 0.0);
		double norm = dot_product /
		              project_onto.getNormSquared();

		assert((project_onto * norm).isParallel(project_onto));
		return project_onto * norm;
	}

//
//  getAntiProjectionSafe
//
//  Purpose: To determine the portion of this Vector3 that
//           remains after the projection onto another Vector3
//           is subtracted off, without crashing if the other
//           Vector3 is zero.  This function is slower than
//           getAntiProjection.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//  Returns: This Vector3 minus its projection onto
//           project_onto. This is the component of this Vector3
//           at right angles to project_onto.  If
//           project_onto is zero, a copy of this Vector3 is
//           returned.
//  Side Effect: N/A
//
	Vector3 getAntiProjectionSafe (
	                      const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());

		if(project_onto.isZero())
			return *this;

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector3::ZERO;

		Vector3 projection = getProjectionSafe(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getProjectionNormal
//
//  Purpose: To determine the projection of this Vector3 onto a
//           Vector3 that is a normal vector.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> project_onto.isNormal()
//  Returns: The projection of this Vector3 onto project_onto.
//           This is the component of this Vector3 with the same
//           (or opposite) direction as project_onto.
//  Side Effect: N/A
//
	Vector3 getProjectionNormal (const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(project_onto.isNormal());

		double dot_product = dotProduct(project_onto);

		assert((project_onto * dot_product).isParallel(project_onto));
		return project_onto * dot_product;
	}

//
//  getAntiProjectionNormal
//
//  Purpose: To determine the portion of this Vector3 that
//           remains after the projection onto a Vector3 that is
//           a normal vector is subtracted off.
//  Parameter(s):
//    <1> project_onto: The Vector3 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> project_onto.isNormal()
//  Returns: This Vector3 minus its projection onto
//           project_onto. This is the component of this Vector3
//           at right angles to project_onto.
//  Side Effect: N/A
//
	Vector3 getAntiProjectionNormal (
	                      const Vector3& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(project_onto.isNormal());

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector3::ZERO;

		Vector3 projection = getProjectionNormal(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getReflection
//
//  Purpose: To determine the result of reflecting this Vector3
//           off of a surface with the specified normal vector.
//           Despite the name, the surface normal does not have
//           to have a norm of 1.0.
//  Parameter(s):
//    <1> surface_normal: The normal vector of the surface to
//                        reflect off of
//  Precondition(s):
//    <1> isFinite()
//    <2> surface_normal.isFinite()
//    <3> !surface_normal.isZero()
//  Returns: The resulting vector when this Vector3 reflects off
//           a surface with normal vector surface_normal.
//  Side Effect: N/A
//
	Vector3 getReflection (const Vector3& surface_normal) const
	{
		assert(isFinite());
		assert(surface_normal.isFinite());
		assert(!surface_normal.isZero());

		return getReflectionNormal(
	                            surface_normal.getNormalized());
	}

//
//  getReflectionSafe
//
//  Purpose: To determine the result of reflecting this Vector3
//           off of a surface with the specified normal vector,
//           without crashing if the surface normal is the zero
//           vector.  Despite the name, the surface normal does
//           not have to have a norm of 1.0.
//  Parameter(s):
//    <1> surface_normal: The normal vector of the surface to
//                        reflect off of
//  Precondition(s):
//    <1> isFinite()
//    <2> surface_normal.isFinite()
//  Returns: The resulting vector when this Vector3 reflects off
//           a surface with normal vector surface_normal.  If
//           surface_normal is zero, a copy of this Vector3 is
//           returned.
//  Side Effect: N/A
//
	Vector3 getReflectionSafe (
	                        const Vector3& surface_normal) const
	{
		assert(isFinite());
		assert(surface_normal.isFinite());

		if(surface_normal.isZero())
			return *this;

		return getReflectionNormal(
	                            surface_normal.getNormalized());
	}

//
//  getReflectionNormal
//
//  Purpose: To determine the result of reflecting this Vector3
//           off of a surface with the specified normalized
//           normal vector.
//  Parameter(s):
//    <1> surface_normal: The normal vector of the surface to
//                        reflect off of
//  Precondition(s):
//    <1> isFinite()
//    <2> surface_normal.isFinite()
//    <3> surface_normal.isNormal()
//  Returns: The resulting vector when this Vector3 reflects off
//           a surface with normal vector surface_normal.
//  Side Effect: N/A
//
	Vector3 getReflectionNormal (
	                        const Vector3& surface_normal) const
	{
		assert(isFinite());
		assert(surface_normal.isFinite());
		assert(surface_normal.isNormal());

		return *this - getProjection(surface_normal) * 2.0;
	}

//
//  getCosAngle
//
//  Purpose: To determine the cosine of the angle between this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> other.isFinite()
//    <4> !other.isZero()
//  Returns: The cosine of the angle between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getCosAngle (const Vector3& other) const;

//
//  getCosAngleNormal
//
//  Purpose: To determine the cosine of the angle between this
//           Vector3 and another Vector3 when both are normal
//           vectors.  This function is faster than getCosAngle.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: The cosine of the angle between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getCosAngleNormal (const Vector3& other) const;

//
//  getCosAngleSafe
//
//  Purpose: To determine the cosine of the angle between this
//           Vector3 and another Vector3, without crashing if
//           one of the Vector3s is zero.  This function is
//           slower than getCosAngle.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The cosine of the angle between this Vector3 and
//           other.  If either vector is zero, cos(0) = 1 is
//           returned.
//  Side Effect: N/A
//
	double getCosAngleSafe (const Vector3& other) const;

//
//  getAngle
//
//  Purpose: To determine the angle in radians between this
//           Vector3 and another Vector3.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> other.isFinite()
//    <4> !other.isZero()
//  Returns: The angle in radians between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getAngle (const Vector3& other) const;

//
//  getAngleNormal
//
//  Purpose: To determine the angle in radians between this
//           Vector3 and another Vector3 when both are normal
//           vectors.  This function is faster than getAngle.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: The angle in radians between this Vector3 and
//           other.
//  Side Effect: N/A
//
	double getAngleNormal (const Vector3& other) const;

//
//  getAngleSafe
//
//  Purpose: To determine the angle in radians between this
//           Vector3 and another Vector3, without crashing if
//           one of the Vector3s is zero.  This function is
//           slower than getAngle.
//  Parameter(s):
//    <1> other: The other Vector3
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The angle in radians between this Vector3 and
//           other.  If either vector is zero, 0 is returned.
//  Side Effect: N/A
//
	double getAngleSafe (const Vector3& other) const;

//
//  getRotationX
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the x-axis, assuming the
//           yz-component of the original Vector3 was on the
//           xy-plane and had a positive y-component.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> y != 0.0 || z != 0.0
//  Returns: The current rotation of this Vector3 around the
//           x-axis in radians.
//  Side Effect: N/A
//
	double getRotationX () const
	{
		assert(isFinite());
		assert(y != 0.0 || z != 0.0);

		return atan2(z, y);
	}

//
//  getRotationXSafe
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the x-axis, assuming the
//           yz-component of the original Vector3 was on the
//           xy-plane and had a positive y-component.  This
//           function will not crash if the y- and z-components
//           are 0.0, but it is slower than the getRotationX
//           function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: The current rotation of this Vector3 around the
//           x-axis in radians.  If y == 0.0 && z == 0.0, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getRotationXSafe () const
	{
		assert(isFinite());

		if(y == 0.0 && z == 0.0)
			return 0.0;
		else
			return atan2(z, y);
	}

//
//  getRotationY
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the y-axis, assuming the
//           zx-component of the original Vector3 was on the
//           yz-plane and had a positive z-component.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> z != 0.0 || x != 0.0
//  Returns: The current rotation of this Vector3 around the
//           y-axis in radians.
//  Side Effect: N/A
//
	double getRotationY () const
	{
		assert(isFinite());
		assert(z != 0.0 || x != 0.0);

		return atan2(x, z);
	}

//
//  getRotationYSafe
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the y-axis, assuming the
//           zx-component of the original Vector3 was on the
//           yz-plane and had a positive z-component.  This
//           function will not crash if the x- and z-components
//           are 0.0, but it is slower than the getRotationX
//           function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: The current rotation of this Vector3 around the
//           y-axis in radians.  If x == 0.0 && z == 0.0, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getRotationYSafe () const
	{
		assert(isFinite());

		if(x == 0.0 && z == 0.0)
			return 0.0;
		else
			return atan2(x, z);
	}

//
//  getRotationZ
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the z-axis, assuming the
//           xy-component of the original Vector3 was on the
//           zx-plane and had a positive x-component.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> x != 0.0 || y != 0.0
//  Returns: The current rotation of this Vector3 around the
//           z-axis in radians.
//  Side Effect: N/A
//
	double getRotationZ () const
	{
		assert(isFinite());
		assert(x != 0.0 || y != 0.0);

		return atan2(y, x);
	}

//
//  getRotationZSafe
//
//  Purpose: To determine how far this Vector3 is rotated in
//           radians around the z-axis, assuming the
//           xy-component of the original Vector3 was on the
//           zx-plane and had a positive x-component.  This
//           function will not crash if the x- and y-components
//           are 0.0, but it is slower than the getRotationZ
//           function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: The current rotation of this Vector3 around the
//           z-axis in radians.  If x == 0.0 && y == 0.0, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getRotationZSafe () const
	{
		assert(isFinite());

		if(x == 0.0 && y == 0.0)
			return 0.0;
		else
			return atan2(y, x);
	}

//
//  getRotatedX
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           X-axis by the specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector3 rotated radians radians
//           around the X-axis.
//  Side Effect: N/A
//
	Vector3 getRotatedX (double radians) const
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		return Vector3(x,
		               cos_angle * y - sin_angle * z,
		               sin_angle * y + cos_angle * z);
	}

//
//  getRotatedY
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           Y-axis by the specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector3 rotated radians radians
//           around the Y-axis.
//  Side Effect: N/A
//
	Vector3 getRotatedY (double radians) const
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		return Vector3(sin_angle * z + cos_angle * x,
		               y,
		               cos_angle * z - sin_angle * x);
	}

//
//  getRotatedZ
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           Z-axis by the specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector3 rotated radians radians
//           around the Z-axis.
//  Side Effect: N/A
//
	Vector3 getRotatedZ (double radians) const
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		return Vector3(cos_angle * x - sin_angle * y,
		               sin_angle * x + cos_angle * y,
		               z);
	}

//
//  getRotatedXZAxes
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           x- and z-axes by the specified angles.
//  Parameter(s):
//    <1> radians_x
//    <2> radians_z: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector3 rotated radians_x radians
//           around the x-axis and then radians_z radians around
//           the z-axis.
//  Side Effect: N/A
//
	Vector3 getRotatedXZAxes (double radians_x,
	                          double radians_z) const
	{
		assert(isFinite());

		return getRotatedX(radians_x).
		       getRotatedZ(radians_z);
	}

//
//  getRotatedArbitrary
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           specified axis by the specified angle.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//    <3> !axis.isZero()
//  Returns: A copy of this Vector3 rotated radians radians
//           around axis axis.
//  Side Effect: N/A
//
	Vector3 getRotatedArbitrary (const Vector3& axis,
	                             double radians) const
	{
		assert(isFinite());
		assert(axis.isFinite());
		assert(!axis.isZero());

		return getRotatedArbitraryNormal(axis.getNormalized(),
		                                 radians);
	}

//
//  getRotatedArbitrarySafe
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           specified axis by the specified angle without
//           crashing if the axis is the zero vector.  This
//           function is slower than the getRotatedArbitrary
//           function.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//  Returns: If axis is the zero vector, a copy of this Vector3
//           is returned.  Otherwise, a copy of this Vector3
//           rotated radians radians around axis axis is
//           returned.
//  Side Effect: N/A
//
	Vector3 getRotatedArbitrarySafe (const Vector3& axis,
	                                 double radians) const
	{
		assert(isFinite());
		assert(axis.isFinite());

		if(axis.isZero())
			return *this;

		return getRotatedArbitraryNormal(axis.getNormalized(),
		                                 radians);
	}

//
//  getRotatedArbitraryNormal
//
//  Purpose: To create a copy of this Vector3 rotated around the
//           specified axis by the specified angle.  The axis is
//           assumed to be a normal vector.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//    <3> axis.isNormal()
//  Returns: A copy of this Vector3 rotated radians radians
//           around axis axis.
//  Side Effect: N/A
//
	Vector3 getRotatedArbitraryNormal (const Vector3& axis,
	                                   double radians) const;


//
//  getRotatedTowards
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction by up to the specified
//           angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//  Returns: If desired is parallel to this Vector3, this
//           Vector3 will be returned.  Otherwise, a copy of
//           this Vector3 rotated up to radians radians towards
//           direction desired is returned.  It will be as close
//           to desired as possible, given the starting
//           direction and radians. Rotating zero vector will
//           always produce the zero vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowards (const Vector3& desired,
	                           double radians) const
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(!desired.isZero());

		return getRotatedTowardsNormal(desired.getNormalized(),
		                               radians);
	}

//
//  getRotatedTowardsSafe
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction by up to the specified
//           angle.  This function will not crash if the desired
//           direction is the zero vector.  This function is
//           slower than the getRotatedTowards function.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//  Returns: If desired is the zero vector or parallel to this
//           Vector3, a copy of this Vector3 is returned.
//           Otherwise, a copy of this Vector3 rotated up to
//           radians radians towards direction desired is
//           returned.  It will be as close to desired as
//           possible, given the starting direction and radians.
//           Rotating zero vector will always produce the zero
//           vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowardsSafe (const Vector3& desired,
	                               double radians) const
	{
		assert(isFinite());
		assert(desired.isFinite());

		if(desired.isZero())
			return *this;

		assert(!desired.isZero());
		return getRotatedTowardsNormal(desired.getNormalized(),
		                               radians);
	}

//
//  getRotatedTowardsNormal
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction by up to the specified
//           angle.  The axis is assumed to be a normal vector.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> desired.isNormal()
//  Returns: If desired is parallel to this Vector3, this
//           Vector3 will be returned.  Otherwise, a copy of
//           this Vector3 rotated up to radians radians towards
//           direction desired is returned.  It will be as close
//           to desired as possible, given the starting
//           direction and radians. Rotating zero vector will
//           always produce the zero vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowardsNormal (const Vector3& desired,
	                                 double radians) const;

//
//  getRotatedTowardsAroundAxis
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction around the specified axis
//           by up to the specified angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//    <4> axis.isFinite()
//    <5> !axis.isZero()
//  Returns: A copy of this Vector3 rotated up to radians
//           radians around axis axis towards direction desired.
//           If axis is parallel to this Vector3 or to desired,
//           this Vector3 will be returned.  Otherwise, the
//           resulting vector will be as close to desired as
//           possible, given the starting diection, radians, and
//           axis.  Rotating zero vector will always produce the
//           zero vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowardsAroundAxis (
	                                  const Vector3& desired,
	                                  double radians,
	                                  const Vector3& axis) const
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(!desired.isZero());
		assert(axis.isFinite());
		assert(!axis.isZero());

		// no normalization needed
		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return *this;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		return getRotatedTowardsNormal(best_possible, radians);
	}

//
//  getRotatedTowardsAroundAxisSafe
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction around the specified axis
//           by up to the specified angle.  This function will
//           not crash if the desired direction or the axis is
//           the zero vector.  This function is slower than the
//           getRotatedTowardsAroundArbitrary function.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> axis.isFinite()
//  Returns: If desired or axis is the zero vector, or if axis
//           is parallel to this Vector3 or to desired, a copy
//           of this Vector3 is returned.  Otherwise, a copy of
//           this Vector3 rotated up to radians radians around
//           axis axis towards direction desired is returned.
//           The resulting vector will be as close to desired as
//           possible, given the starting diection, radians, and
//           axis.  Rotating zero vector will always produce the
//           zero vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowardsAroundAxisSafe (
	                                  const Vector3& desired,
	                                  double radians,
	                                  const Vector3& axis) const
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(axis.isFinite());

		// desired.isZero() shows up as parallel
		if(axis.isZero())
			return *this;

		// no normalization needed
		assert(!axis.isZero());
		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return *this;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		return getRotatedTowardsNormal(best_possible, radians);
	}

//
//  getRotatedTowardsAroundAxisNormal
//
//  Purpose: To create a copy of this Vector3 rotated towards
//           the specified direction around the specified axis
//           by up to the specified angle.  The axis is assumed
//           to be a normal vector.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> desired.isNormal()
//    <4> axis.isFinite()
//    <5> axis.isNormal()
//  Returns: A copy of this Vector3 rotated up to radians
//           radians around axis axis towards direction desired.
//           If axis is parallel to this Vector3 or to desired,
//           this Vector3 will be returned.  Otherwise, the
//           resulting vector will be as close to desired as
//           possible, given the starting diection, radians, and
//           axis.  Rotating zero vector will always produce the
//           zero vector.
//  Side Effect: N/A
//
	Vector3 getRotatedTowardsAroundAxisNormal (
	                                  const Vector3& desired,
	                                  double radians,
	                                  const Vector3& axis) const
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(desired.isNormal());
		assert(axis.isFinite());
		assert(axis.isNormal());

		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return *this;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		return getRotatedTowardsNormal(best_possible, radians);
	}

//
//  rotateX
//
//  Purpose: To rotate this Vector3 around the X-axis by the
//           specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated radians radians around
//               the X-axis.
//
	void rotateX (double radians)
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		set(x,
		    cos_angle * y - sin_angle * z,
		    sin_angle * y + cos_angle * z);
	}

//
//  rotateY
//
//  Purpose: To rotate this Vector3 around the Y-axis by the
//           specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated radians radians around
//               the Y-axis.
//
	void rotateY (double radians)
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		set(sin_angle * z + cos_angle * x,
		    y,
		    cos_angle * z - sin_angle * x);
	}

//
//  rotateZ
//
//  Purpose: To rotate this Vector3 around the Z-axis by the
//           specified angle.
//  Parameter(s):
//    <1> radians: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated radians radians around
//               the Z-axis.
//
	void rotateZ (double radians)
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		set(cos_angle * x - sin_angle * y,
		    sin_angle * x + cos_angle * y,
		    z);
	}

//
//  rotateXZAxes
//
//  Purpose: To rotate this Vector3 around the x- and z-axes by
//           the specified angles.
//  Parameter(s):
//    <1> radians_x
//    <2> radians_z: The angles to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated around the x-axis by
//               radians_x radians and then around the z-axis
//               by radians_z radians.
//
	void rotateXZAxes (double radians_x, double radians_z)
	{
		assert(isFinite());

		rotateX(radians_x);
		rotateZ(radians_z);
	}

//
//  rotateArbitrary
//
//  Purpose: To rotate this Vector3 around the specified axis by
//           the specified angle.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//    <3> !axis.isZero()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated by radians radians
//               around axis axis.
//
	void rotateArbitrary (const Vector3& axis,
	                      double radians)
	{
		assert(isFinite());
		assert(axis.isFinite());
		assert(!axis.isZero());

		rotateArbitraryNormal(axis.getNormalized(), radians);
	}

//
//  rotateArbitrarySafe
//
//  Purpose: To rotate this Vector3 around the specified axis by
//           the specified angle without crashing if the axis is
//           the zero vector.  This function is slower than the
//           rotateArbitrary function.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//    <3> axis.isNormal()
//  Returns: N/A
//  Side Effect: If axis is the zero vector, there is no effect.
//               Otherwise, this Vector3 is rotated by radians
//               radians around axis axis.
//
	void rotateArbitrarySafe (const Vector3& axis,
	                          double radians)
	{
		assert(isFinite());
		assert(axis.isFinite());

		if(axis.isZero())
			return;

		assert(!axis.isZero());
		rotateArbitraryNormal(axis.getNormalized(), radians);
	}

//
//  rotateArbitraryNormal
//
//  Purpose: To rotate this Vector3 around the specified axis by
//           the specified angle.
//  Parameter(s):
//    <1> axis: The axis to rotate around
//    <2> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> axis.isFinite()
//    <3> axis.isNormal()
//  Returns: N/A
//  Side Effect: This Vector3 is rotated by radians radians
//               around axis axis.
//
	void rotateArbitraryNormal (const Vector3& axis,
	                            double radians);

//
//  rotateTowards
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction by up to the specified angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//  Returns: N/A
//  Side Effect: If desired is parallel to this Vector3, there
//               is no effect.  Otherwise, this Vector3 is
//               rotated up to radians radians towards direction
//               desired, putting it as close as possible given
//               the starting direction and radians.  Rotating
//               the zero vector will always produce the zero
//               vector.
//
	void rotateTowards (const Vector3& desired,
	                    double radians)
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(!desired.isZero());

		return rotateTowardsNormal(desired.getNormalized(),
		                           radians);
	}

//
//  rotateTowardsSafe
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction by up to the specified angle without
//           crashing if the desired direction is the zero
//           vector.  This function is slower than the
//           rotateTowards function.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//  Returns: N/A
//  Side Effect: If desired is the zero vector or parallel to
//               this Vector3, there is no effect.  Otherwise,
//               this Vector3 will be rotated up to radians 
//               radians towards direction desired, putting it
//               as close as possible, given the starting
//               direction and radians.  Rotating zero vector
//               will always produce the zero vector.
//
	void rotateTowardsSafe (const Vector3& desired,
	                        double radians)
	{
		assert(isFinite());
		assert(desired.isFinite());

		if(desired.isZero())
			return;

		assert(!desired.isZero());
		return rotateTowardsNormal(desired.getNormalized(),
		                           radians);
	}

//
//  rotateTowardsNormal
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction by up to the specified angle.  The axis
//           is assumed to be a normal vector.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> desired.isNormal()
//  Returns: N/A
//  Side Effect: If desired is parallel to this Vector3, there
//               is no effect.  Otherwise, this Vector3 is
//               rotated up to radians radians towards direction
//               desired, putting it as close as possible given
//               the starting direction and radians.  Rotating
//               the zero vector will always produce the zero
//               vector.
//
	void rotateTowardsNormal (const Vector3& desired,
	                          double radians);

//
//  rotateTowardsAroundAxis
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction around the specified axis by up to the
//           specified angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//    <4> axis.isFinite()
//    <5> !axis.isZero()
//  Returns: N/A
//  Side Effect: If axis is parallel to this Vector3 or to
//               desired, there is no effect.  Otherwise, this
//               Vector3 is rotated up to radians radians around
//               axis axis, putting it as close to desired as
//               possible given the starting direction and
//               radians.  Rotating the zero vector will always
//               produce the zero vector.
//
	void rotateTowardsAroundAxis (const Vector3& desired,
	                              double radians,
	                              const Vector3& axis)
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(!desired.isZero());
		assert(axis.isFinite());
		assert(!axis.isZero());

		// no normalization needed
		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		rotateTowardsNormal(best_possible, radians);
	}

//
//  rotateTowardsAroundAxisSafe
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction around the specified axis by up to the
//           specified angle without crashing if the desired
//           direction is the zero vector.  This function is
//           slower than the getRotatedTowardsArbitrary
//           function.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> axis.isFinite()
//  Returns: N/A
//  Side Effect: If desired or axis is the zero vector, or if
//               axis is parallel to this Vector3 or to desired,
//               there is no effect.  Otherwise, this Vector3 is
//               rotated up to radians radians around axis axis,
//               putting it as close to desired as possible
//               given the starting direction and radians.
//               Rotating the zero vector will always produce
//               the zero vector.
//
	void rotateTowardsAroundAxisSafe (const Vector3& desired,
	                                  double radians,
	                                  const Vector3& axis)
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(axis.isFinite());

		// desired.isZero() shows up as parallel
		if(axis.isZero())
			return;

		// no normalization needed
		assert(!axis.isZero());
		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		rotateTowardsNormal(best_possible, radians);
	}

//
//  rotateTowardsAroundAxisNormal
//
//  Purpose: To rotate this Vector3 towards the specified
//           direction around the specified axis by up to the
//           specified angle.  The desired direction and axis
//           are assumed to be normal vectors.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//    <3> axis: The axis to rotate around
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> desired.isNormal()
//    <4> axis.isFinite()
//    <5> axis.isNormal()
//  Returns: N/A
//  Side Effect: If axis is parallel to this Vector3 or to
//               desired, there is no effect.  Otherwise, this
//               Vector3 is rotated up to radians radians around
//               axis axis, putting it as close to desired as
//               possible given the starting direction and
//               radians.  Rotating the zero vector will always
//               produce the zero vector.
//
	void rotateTowardsAroundAxisNormal (const Vector3& desired,
	                                    double radians,
	                                    const Vector3& axis)
	{
		assert(isFinite());
		assert(desired.isFinite());
		assert(desired.isNormal());
		assert(axis.isFinite());
		assert(axis.isNormal());

		Vector3 best_possible =
		                  desired.getAntiProjectionNormal(axis);
		if(best_possible.isZero())
			return;  // desired and axis are parallel
		best_possible.normalize();
		assert(best_possible.isNormal());
		assert(isOrthogonal(best_possible));

		rotateTowardsNormal(best_possible, radians);
	}

//
//  getMatrixProduct
//
//  Purpose: To create a copy of this Vector3 multiplied by the
//           specified 3x3 matrix.
//  Parameter(s):
//    <1> e11        +-----+-----+-----+
//    <2> e12        | e11 | e12 | e13 |
//    <3> e13        +-----+-----+-----+
//    <4> e21        | e21 | e22 | e23 |
//    <5> e22        +-----+-----+-----+
//    <6> e23        | e31 | e32 | e33 |
//    <7> e31        +-----+-----+-----+
//    <8> e32
//    <9> e33: The matrix elements
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector3 multiplied by the matrix
//           composed of the 9 specified elements.
//  Side Effect: N/A
//
	Vector3 getMatrixProduct (double e11,
	                          double e12,
	                          double e13,
	                          double e21,
	                          double e22,
	                          double e23,
	                          double e31,
	                          double e32,
	                          double e33) const;

//
//  getMatrixProductRows
//
//  Purpose: To create a copy of this Vector3 multiplied by a
//           3x3 matrix expressed as row vectors.
//  Parameter(s):
//    <1> r1                       +------+------+------+
//    <2> r2                       | r1.x | r1.y | r1.z |
//    <3> r3: The row vectors      +------+------+------+
//  Precondition(s):               | r2.x | r2.y | r2.z |
//    <1> isFinite()               +------+------+------+
//    <2> r1.isFinite()            | r3.x | r3.y | r3.z |
//    <3> r2.isFinite()            +------+------+------+
//    <4> r3.isFinite()
//  Returns: A copy of this Vector3 multiplied by the matrix
//           composed of the 9 elements in row vectors r1, r2,
//           and r3.
//  Side Effect: N/A
//
	Vector3 getMatrixProductRows (const Vector3& r1,
	                              const Vector3& r2,
	                              const Vector3& r3) const;

//
//  getMatrixProductColumns
//
//  Purpose: To create a copy of this Vector3 multiplied by a
//           3x3 matrix expressed as column vectors. The
//           Vector3s correspond to the X, Y, and Z vectors of a
//           basis matrix.
//  Parameter(s):
//    <1> c1                          +------+------+------+
//    <2> c2                          | c1.x | c2.x | c3.x |
//    <3> c3: The column vectors      +------+------+------+
//  Precondition(s):                  | c1.y | c2.y | c3.y |
//    <1> isFinite()                  +------+------+------+
//    <2> c1.isFinite()               | c1.z | c2.z | c3.z |
//    <3> c2.isFinite()               +------+------+------+
//    <4> c3.isFinite()
//  Returns: A copy of this Vector3 multiplied by the matrix
//           composed of the 9 elements in column vectors c1,
//           c2, and c3.
//  Side Effect: N/A
//
	Vector3 getMatrixProductColumns (
	                               const Vector3& c1,
	                               const Vector3& c2,
	                               const Vector3& c3) const;

//
//  getRandomUnitVector
//
//  Purpose: To generate a Vector3 of norm 1.0 and with a
//           uniform random direction.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random unit vector.
//  Side Effect: N/A
//
	static Vector3 getRandomUnitVector ();

//
//  getPseudorandomUnitVector
//
//  Purpose: To generate a Vector3 of norm 1.0 and with a
//           uniform direction based on the specified seed
//           values.  If the seeds are generated
//           pseudorandomly, this function will return
//           consistant pseudorandom unit vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A uniform unit vector calculated from seed1 and
//           seed2.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomUnitVector (double seed1,
	                                          double seed2);

//
//  getRandomUnitVectorXY
//
//  Purpose: To generate a Vector3 on the XY plane with norm 1.0
//           and a uniform random direction.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random unit vector on the XY plane.
//  Side Effect: N/A
//
	static Vector3 getRandomUnitVectorXY ();

//
//  getRandomUnitVectorXZ
//
//  Purpose: To generate a Vector3 on the XZ plane with norm 1.0
//           and a uniform random direction.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random unit vector on the XZ plane.
//  Side Effect: N/A
//
	static Vector3 getRandomUnitVectorXZ ();

//
//  getRandomUnitVectorYZ
//
//  Purpose: To generate a Vector3 on the YZ plane with norm 1.0
//           and a uniform random direction.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random unit vector on the YZ plane.
//  Side Effect: N/A
//
	static Vector3 getRandomUnitVectorYZ ();

//
//  getPseudorandomUnitVectorXY
//
//  Purpose: To generate a Vector3 on the XY plane with norm 1.0
//           and with a uniform direction based on the specified
//           seed value.  If the seed is generated
//           pseudorandomly, this function will return
//           consistant pseudorandom unit vectors.
//  Parameter(s):
//    <1> seed: The seed value
//  Precondition(s):
//    <1> seed >= 0.0
//    <2> seed <  1.0
//  Returns: A uniform unit vector on the XY plane calculated
//           from seed.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomUnitVectorXY (double seed);

//
//  getPseudorandomUnitVectorXZ
//
//  Purpose: To generate a Vector3 on the XZ plane with norm 1.0
//           and with a uniform direction based on the specified
//           seed value.  If the seed is generated
//           pseudorandomly, this function will return
//           consistant pseudorandom unit vectors.
//  Parameter(s):
//    <1> seed: The seed value
//  Precondition(s):
//    <1> seed >= 0.0
//    <2> seed <  1.0
//  Returns: A uniform unit vector on the XZ plane calculated
//           from seed.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomUnitVectorXZ (double seed);

//
//  getPseudorandomUnitVectorYZ
//
//  Purpose: To generate a Vector3 on the YZ plane with norm 1.0
//           and with a uniform direction based on the specified
//           seed value.  If the seed is generated
//           pseudorandomly, this function will return
//           consistant pseudorandom unit vectors.
//  Parameter(s):
//    <1> seed: The seed value
//  Precondition(s):
//    <1> seed >= 0.0
//    <2> seed <  1.0
//  Returns: A uniform unit vector on the YZ plane calculated
//           from seed.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomUnitVectorYZ (double seed);

//
//  getRandomSphereVector
//
//  Purpose: To generate a Vector3 of uniform distribution and
//           a norm of less than or equal to 1.0.  In effect,
//           this is a vector to somewhere within a sphere of
//           radius 1.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random vector with a norm of no more than
//           1.0.
//  Side Effect: N/A
//
	static Vector3 getRandomSphereVector ();

//
//  getPseudorandomSphereVector
//
//  Purpose: To generate a Vector3 of uniform distribution and
//           a norm of less than or equal to 1.0 based on the
//           specified seed values.  In effect, this is a vector
//           to somewhere within a sphere of radius 1.0.  If the
//           seeds are generated pseudorandomly, this function
//           will return consistant pseudorandom vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//    <3> seed3: The third seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//    <5> seed3 >= 0.0
//    <6> seed3 <  1.0
//  Returns: A uniform vector with a norm of no more than 1.0
//           that has been calculated from seed1, seed2, and
//           seed3.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomSphereVector (double seed1,
	                                            double seed2,
	                                            double seed3);

//
//  getRandomSphereVectorXY
//
//  Purpose: To generate a Vector3 on the XY plane of uniform
//           distribution and a norm of less than or equal to
//           1.0.  In effect, this is a vector to somewhere
//           within a circle of radius 1.0.  This function is
//           named "sphere" instead of "circle" to correspond
//           with the getRandomSphereVector function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random vector on the XY pane with a norm
//           of no more than 1.0.
//  Side Effect: N/A
//
	static Vector3 getRandomSphereVectorXY ();

//
//  getRandomSphereVectorXZ
//
//  Purpose: To generate a Vector3 on the XZ plane of uniform
//           distribution and a norm of less than or equal to
//           1.0.  In effect, this is a vector to somewhere
//           within a circle of radius 1.0.  This function is
//           named "sphere" instead of "circle" to correspond
//           with the getRandomSphereVector function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random vector on the XZ pane with a norm
//           of no more than 1.0.
//  Side Effect: N/A
//
	static Vector3 getRandomSphereVectorXZ ();

//
//  getRandomSphereVectorYZ
//
//  Purpose: To generate a Vector3 on the YZ plane of uniform
//           distribution and a norm of less than or equal to
//           1.0.  In effect, this is a vector to somewhere
//           within a circle of radius 1.0.  This function is
//           named "sphere" instead of "circle" to correspond
//           with the getRandomSphereVector function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random vector on the YZ pane with a norm
//           of no more than 1.0.
//  Side Effect: N/A
//
	static Vector3 getRandomSphereVectorYZ ();

//
//  getPseudorandomSphereVectorXY
//
//  Purpose: To generate a Vector2 based on the specified seed
//           values that is of uniform distribution on the XY
//           plane and has a norm of less than or equal to 1.0.
//           In effect, this is a vector to somewhere within a
//           circle of radius 1.0.  If the seeds are generated
//           pseudorandomly, this function will return
//           consistant pseudorandom vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A uniform vector on the XY plane with a norm of no
//           more than 1.0 that has been calculated from seed1
//           and seed2.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomSphereVectorXY (double seed1,
	                                              double seed2);

//
//  getPseudorandomSphereVectorXZ
//
//  Purpose: To generate a Vector2 based on the specified seed
//           values that is of uniform distribution on the XZ
//           plane and has a norm of less than or equal to 1.0.
//           In effect, this is a vector to somewhere within a
//           circle of radius 1.0.  If the seeds are generated
//           pseudorandomly, this function will return
//           consistant pseudorandom vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A uniform vector on the XZ plane with a norm of no
//           more than 1.0 that has been calculated from seed1
//           and seed2.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomSphereVectorXZ (double seed1,
	                                              double seed2);

//
//  getPseudorandomSphereVectorYZ
//
//  Purpose: To generate a Vector2 based on the specified seed
//           values that is of uniform distribution on the YZ
//           plane and has a norm of less than or equal to 1.0.
//           In effect, this is a vector to somewhere within a
//           circle of radius 1.0.  If the seeds are generated
//           pseudorandomly, this function will return
//           consistant pseudorandom vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A uniform vector on the YZ plane with a norm of no
//           more than 1.0 that has been calculated from seed1
//           and seed2.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomSphereVectorYZ (double seed1,
	                                              double seed2);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector3 with each component in
//           the range [0, 1).
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to (1.0, 1.0, 1.0).  For each component, 0.0 is
//           included but 1.0 is excluded.
//  Side Effect: N/A
//
	static Vector3 getRandomInRange ();

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector3 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> max: The maximum value for components
//  Precondition(s):
//    <1> max >= 0.0
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to (max, max, max).  For each component, 0.0 is
//           included but max is excluded.  If max is 0.0, the
//           zero vector is returned.
//  Side Effect: N/A
//
	static Vector3 getRandomInRange (double max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector3 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> max: A Vector3 containing the maximum component values
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to max.  For each component, 0.0 is included but
//           the corresponding component of max is excluded.  If
//           any component of max is 0.0, 0.0 is returned for
//           that component.
//  Side Effect: N/A
//
	static Vector3 getRandomInRange (const Vector3& max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector3 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> min: The minimum value for components
//    <2> max: The maximum value for components
//  Precondition(s):
//    <1> min <= max
//  Returns: A random Vector3 in the range from (min, min, min)
//           to (max, max, max).  For each component, min is
//           included but max is excluded.  If min and max are
//           equal, (min, min, min) is returned.
//  Side Effect: N/A
//
	static Vector3 getRandomInRange (double min, double max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector3 with each component
//           between the corresponding components of the
//           specified vectors.
//  Parameter(s):
//    <1> min: A Vector3 containing the minimum component values
//    <2> max: A Vector3 containing the maximum component values
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A random Vector3 in the range from min to max.  For
//           each component, min is included but max is excluded.
//           If for any component min and max are equal, the
//           shared value is returned for that component.
//  Side Effect: N/A
//
	static Vector3 getRandomInRange (const Vector3& min,
	                                 const Vector3& max);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector3 with each component in the
//           range [0, 1) and based on the specified seed
//           values.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//    <3> seed3: The third seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//    <5> seed3 >= 0.0
//    <6> seed3 <  1.0
//  Returns: A Vector3 in the range from (0.0, 0.0, 0.0) to
//           (1.0, 1.0, 1.0) that has been calculated from
//           seed1, seed2, and seed3.  For each component, 0.0
//           is included but 1.0 is excluded.  This function
//           really just combines the seed values into a
//           Vector3, but it does have the appropriate
//           distribution.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomInRange (double seed1,
	                                       double seed2,
	                                       double seed3);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector3 with each component less than
//           the specified maximum and based on the specified
//           seed values.
//  Parameter(s):
//    <1> max: The maximum value for components
//    <2> seed1: The first seed value
//    <3> seed2: The second seed value
//    <4> seed3: The third seed value
//  Precondition(s):
//    <1> max >= 0.0
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//    <6> seed3 >= 0.0
//    <7> seed3 <  1.0
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to (max, max, max) that has been calculated from
//           seed1, seed2, and seed3.  For each component, 0.0
//           is included but max is excluded.  If max is 0.0,
//           the zero vector is returned.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomInRange (double max,
	                                       double seed1,
	                                       double seed2,
	                                       double seed3);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector3 with each component less than
//           the specified maximum and based on the specified
//           seed values.
//  Parameter(s):
//    <1> max: A Vector3 containing the maximum component values
//    <2> seed1: The first seed value
//    <3> seed2: The second seed value
//    <4> seed3: The third seed value
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//    <6> seed3 >= 0.0
//    <7> seed3 <  1.0
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to max that has been calculated from seed1, seed2,
//           and seed3.  For each component, 0.0 is included but
//           the corresponding component of max is excluded.  If
//           any component of max is 0.0, 0.0 is returned for
//           that component.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomInRange (const Vector3& max,
	                                       double seed1,
	                                       double seed2,
	                                       double seed3);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector3 with each component less than
//           the specified values and based on the specified
//           seed values.
//  Parameter(s):
//    <1> min: The minimum value for components
//    <2> max: The maximum value for components
//    <3> seed1: The first seed value
//    <4> seed2: The second seed value
//    <5> seed3: The third seed value
//  Precondition(s):
//    <1> min <= max
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//    <6> seed3 >= 0.0
//    <7> seed3 <  1.0
//  Returns: A random Vector3 in the range from (min, min, min)
//           to (max, max, max) that has been calculated from
//           seed1, seed2, and seed3.  For each component, min
//           is included but max is excluded.  If min and max
//           are equal, (min, min, min) is returned.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomInRange (double min,
	                                       double max,
	                                       double seed1,
	                                       double seed2,
	                                       double seed3);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector3 with each component between
//           the corresponding components of the specified
//           vectors and based on the specified seed values.
//  Parameter(s):
//    <1> min: A Vector3 containing the minimum component values
//    <2> max: A Vector3 containing the maximum component values
//    <3> seed1: The first seed value
//    <4> seed2: The second seed value
//    <5> seed3: The third seed value
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//    <6> seed3 >= 0.0
//    <7> seed3 <  1.0
//  Returns: A Vector3 in the range from min to max that has
//           been calculated from seed1, seed2, and seed3.  For
//           each component, min is included but max is excluded.
//           If for any component min and max are equal, the
//           shared value is returned for that component.
//  Side Effect: N/A
//
	static Vector3 getPseudorandomInRange (const Vector3& min,
	                                       const Vector3& max,
	                                       double seed1,
	                                       double seed2,
	                                       double seed3);

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector3 with each component in
//           the range [0, 1].
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to (1.0, 1.0, 1.0).  For each component, both 0.0
//           and 1.0 are included.
//  Side Effect: N/A
//
	static Vector3 getRandomInRangeInclusive ();

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector3 with each component
//           less than or equal to the specified maximum.
//  Parameter(s):
//    <1> max: A Vector3 containing the maximum component values
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//  Returns: A random Vector3 in the range from (0.0, 0.0, 0.0)
//           to max.  For each component, both 0.0 and the
//           corresponding component of max are included.
//  Side Effect: N/A
//
	static Vector3 getRandomInRangeInclusive (const Vector3& max);

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector3 with each component
//           between the corresponding components of the
//           specified vectors, including the minimum and
//           maximum component values.
//  Parameter(s):
//    <1> min: A Vector3 containing the minimum component values
//    <2> max: A Vector3 containing the maximum component values
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A random Vector3 in the range from min to max.  For
//           each component, both min and max are included.
//  Side Effect: N/A
//
	static Vector3 getRandomInRangeInclusive (
	                                    const Vector3& min,
	                                    const Vector3& max);

//
//  getPseudorandomInRangeInclusive (3 variants)
//
//  These functions cannot be implemented without knowing the
//    exact maximum random seed, and thus the exact range of
//    values the random numbers were calculated from.  This
//    depends on the implementation of the random number
//    generator.  Note that even the C standard rand() function
//    returns values in different ranges on different platforms.
//

//
//  getClosestPointOnLine
//
//  Purpose: To determine the point on a specified line segment
//           closest to the specified point.
//  Parameter(s):
//    <1> l1
//    <2> l2: The two ends of the line segment
//    <3> p: The point
//    <4> bounded: Whether the solution must line between the
//                 ends of the line segment
//  Precondition(s):
//    <1> l1.isFinite()
//    <2> l2.isFinite()
//    <3> p.isFinite()
//    <4> l1 != l2
//  Returns: The point on the line from l1 to l2 that is closest
//           to point p.  If bounded == true, the point returned
//           will lie between or on points l1 and l2.
//           Otherwise, the point returned may lie anywhere
//           along the line defined by l1 and l2.
//  Side Effect: N/A
//
	static Vector3 getClosestPointOnLine (const Vector3& l1,
	                                      const Vector3& l2,
	                                      const Vector3& p,
	                                      bool bounded);

private:
//
//  isSquareTolerantEqualTo
//
//  Purpose: To determine if the specified squares are equal,
//           according to tolerance VECTOR3_NORM_TOLERANCE.
//  Parameter(s):
//    <1> a: The first square
//    <2> b: The square square
//  Precondition(s):
//    <1> a >= 0.0
//    <2> b >= 0.0
//  Returns: Whether a is close enough to equal to b that the
//           unsquared values are within fractional tolerance
//           NORM_TOLERANCE.
//  Side Effect: N/A
//
	bool isSquareTolerantEqualTo (double a, double b) const
	{
		assert(a >= 0.0);
		assert(b >= 0.0);

		return (a <= b * VECTOR3_NORM_TOLERANCE_PLUS_ONE_SQUARED) &&
		       (b <= a * VECTOR3_NORM_TOLERANCE_PLUS_ONE_SQUARED);
	}

//
//  isSquareTolerantLessThan
//
//  Purpose: To determine if the specified square is less than
//           the other specified square, according to tolerance
//           VECTOR3_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           less than and less than or equal.
//  Parameter(s):
//    <1> a: The first square
//    <2> b: The square square
//  Precondition(s):
//    <1> a >= 0.0
//    <2> b >= 0.0
//  Returns: Whether a is close enough to less than b that
//           sqrt(a) < sqrt(b) * (VECTOR3_NORM_TOLERANCE + 1.0).
//  Side Effect: N/A
//
	bool isSquareTolerantLessThan (double a, double b) const
	{
		assert(a >= 0.0);
		assert(b >= 0.0);

		return (a <= b * VECTOR3_NORM_TOLERANCE_PLUS_ONE_SQUARED);
	}

};  // end of Vector3 class



//
//  Multiplication Operator
//
//  Purpose: To create a new Vector3 equal to the product of
//           the specified scalar and the specified Vector3.
//  Parameter(s):
//    <1> scalar: The scalar
//    <2> vector: The Vector3
//  Precondition(s): N/A
//  Returns: A Vector3 with elements (vector.x * scalar,
//           vector.y * scalar, vector.z * scalar).
//  Side Effect: N/A
//
inline Vector3 operator* (double scalar, const Vector3& vector)
{
	return Vector3(vector.x * scalar,
	               vector.y * scalar,
	               vector.z * scalar);
}

//
//  Stream Insertion Operator
//
//  Purpose: To print the specified Vector3 to the specified
//           output stream.
//  Parameter(s):
//    <1> r_os: The output stream
//    <2> vector: The Vector3
//  Precondition(s): N/A
//  Returns: A reference to r_os.
//  Side Effect: vector is printed to r_os.
//
std::ostream& operator<< (std::ostream& r_os,
                          const Vector3& vector);



//
//  Clear preprocessor macros from above so they don't cause
//    trouble elsewhere.
//
#undef VECTOR2_IS_FINITE
#undef VECTOR2_CONSTEXPR



}  // end of namespace ObjLibrary

#endif
