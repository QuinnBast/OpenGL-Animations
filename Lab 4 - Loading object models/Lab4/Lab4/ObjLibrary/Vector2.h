//
//  Vector2.h
//
//  A module to store a math-style vector of length 2 and simple
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

#ifndef OBJ_LIBRARY_VECTOR2_H
#define OBJ_LIBRARY_VECTOR2_H

#include <cassert>
#include <iostream>
#include <cmath>
#include <cfloat>

// if you don't need glm, you can remove this line
#include "ObjSettings.h"
// if you don't have ObjSettings, but do need to use glm,
//  #define OBJ_LIBRARY_GLM_INTERACTION

#ifdef OBJ_LIBRARY_GLM_INTERACTION
  #include "../glm/vec2.hpp"
#endif



namespace ObjLibrary
{

//
//  VECTOR2_IS_FINITE
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
  #define VECTOR2_IS_FINITE(n) std::isfinite(n)
#elif _WIN32
  // Microsoft non-standard function
  #define VECTOR2_IS_FINITE(n) _finite(n)
#elif __WIN32__
  // Microsoft non-standard function
  #define VECTOR2_IS_FINITE(n) _finite(n)
#else
  //  In theory, POSIX includes the isfinite macro defined in
  //    the C99 standard.  This macro is not included in any ISO
  //    C++ standard yet (as of January 2013).  However, this
  //    function does not seem to work.
  //#define VECTOR2_IS_FINITE(n) isfinite(n)
  //  From http://www.johndcook.com/blog/IEEE_exceptions_in_cpp/
  #define VECTOR3_IS_FINITE(n) ((n) <= DBL_MAX && (n) >= -DBL_MAX)
#endif

//
//  VECTOR2_CONSTEXPR
//
//  A cross-platform macro that resolves into constexpr on
//    constructors if the compiler supports it.  Hopefully.
//

#if __cpp_constexpr >= 200704  // C++11 constexpr
  #define VECTOR2_CONSTEXPR constexpr
#else
  #define VECTOR2_CONSTEXPR
#endif

//
//  VECTOR2_NORM_TOLERANCE
//
//  This is the fudge factor used when comparing vector normals.
//    It is also used for distance comparisons, which are
//    mathematically equivilent to the norm of the difference
//    between 2 vectors, and for a few other things.
//
//  The results of floating point calculatios frequently vary by
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
const double VECTOR2_NORM_TOLERANCE = 1.0e-4;

//
//  VECTOR2_NORM_TOLERANCE_SQUARED
//
//  This is the square of VECTOR2_NORM_TOLERANCE.  It is useful
//    as an intermediate value, and storing it seperately is
//    faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector2 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR2_NORM_TOLERANCE_SQUARED =
                                        VECTOR2_NORM_TOLERANCE *
                                        VECTOR2_NORM_TOLERANCE;

//
//  VECTOR2_NORM_TOLERANCE_PLUS_ONE_SQUARED
//
//  This is the square of (VECTOR2_NORM_TOLERANCE + 1.0).  It is
//    useful as an intermediate value, and storing it seperately
//    is faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector2 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR2_NORM_TOLERANCE_PLUS_ONE_SQUARED =
                                (VECTOR2_NORM_TOLERANCE + 1.0) *
                                (VECTOR2_NORM_TOLERANCE + 1.0);

//
//  VECTOR2_ONE_MINUS_NORM_TOLERANCE_SQUARED
//
//  This is 1.0 - the square of VECTOR2_NORM_TOLERANCE.  It is
//    useful as an intermediate value, and storing it seperately
//    is faster than recalculating it every time it is used.
//
//  This constant has to be declared outside the Vector2 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR2_ONE_MINUS_NORM_TOLERANCE_SQUARED =
                           1.0 - VECTOR2_NORM_TOLERANCE_SQUARED;

//
//  VECTOR2_ZERO_TOLERENCE
//
//  This is the tolerence used when testing whether a Vector2 is
//    a zero vector.  A slight tolerence is needed to ensure
//    that a Vector2 is not detected as non-zero when it has a
//    norm of 0.0.  This can happen with very small non-zero
//    vectors because calculating the norm of a Vector2 involves
//    squaring the value and the representation has a finite
//    range.
//
//  This constant has to be declared outside the Vector2 class
//    to ensure that it is assigned its value before it is used.
//
const double VECTOR2_ZERO_TOLERENCE = 1.0e-100;



//
//  Vector2
//
//  A class to store a math-style vector of length 2.  The 2
//    numbers that compose a Vector2 are refered to as its
//    elements and may be accesed using dot notation.  The
//    associated functions are all declared inline for speed
//    reasons.  In theory, this class should be as fast (or
//    faster, when using pass-by-reference) as using double
//    values, but more convenient.
//
//  The norm of a Vector2 is its "length", the distance along
//    it.
//
class Vector2
{
public:
//
//  x
//
//  The X component of the Vector2.  This value can be queried
//    and changed freely without disrupting the operation of the
//    Vector2 instance.
//
	double x;

//
//  y
//
//  The Y component of the Vector2.  This value can be queried
//    and changed freely without disrupting the operation of the
//    Vector2 instance.
//
	double y;

//
//  These are some standard Vector2s that may be useful.
//
//  Note that C++ constants (including these ones) are not
//    necessarily initialized in any particular order.  Using
//    these to initialize global variables (or constants) may
//    query them before they are initialized, with undefined
//    results.  If anyone knows how to fix this, please tell me.
//

	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 UNIT_X_PLUS;
	static const Vector2 UNIT_X_MINUS;
	static const Vector2 UNIT_Y_PLUS;
	static const Vector2 UNIT_Y_MINUS;

public:
//
//  Default Constructor
//
//  Purpose: To create a new Vector2 that is the zero vector.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with elements
//               (0.0, 0.0).
//
	VECTOR2_CONSTEXPR Vector2 ()
			: x(0.0),
			  y(0.0)
	{}

//
//  Initializing Constructor
//
//  Purpose: To create a new Vector2 with the specified
//           elements.
//  Parameter(s):
//    <1> x
//    <2> y
//    <3> z: The elements for the new Vector2
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with elements (x, y).
//
	VECTOR2_CONSTEXPR Vector2 (double X, double Y)
			: x(X),
			  y(Y)
	{}

//
//  Constructor
//
//  Purpose: To create a new Vector2 with 2 elements taken from
//           the specified array.
//  Parameter(s):
//    <1> a_elements: The array containing the elements for the
//                    new Vector2
//  Precondition(s):
//    <1> a_elements != NULL
//  Returns: N/A
//  Side Effect: A new Vector2 is created with elements
//               (a_elements[0], a_elements[1]).
//
	Vector2 (const double a_elements[])
			: x(a_elements[0]),
			  y(a_elements[1])
	{
		assert(a_elements != NULL);
	}

//
//  Constructor
//
//  Purpose: To create a new Vector2 with the specified number
//           of elements taken from the specified array.
//  Parameter(s):
//    <1> a_elements: The array containing the elements for the
//                    new Vector2
//    <2> count: How many elements to take from the array
//  Precondition(s):
//    <1> a_elements != NULL
//    <2> count <= 2
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the first count
//               elements from array a_elements.  The remaining
//               elements of the new Vector2 are set to 0.0.
//
	Vector2 (const double a_elements[],
	         unsigned int count)
			: x((count > 0) ? a_elements[0] : 0.0),
			  y((count > 1) ? a_elements[1] : 0.0)
	{
		assert(a_elements != NULL);
		assert(count <= 2);
	}

#ifdef OBJ_LIBRARY_GLM_INTERACTION
//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector2 from the specified
//           glm::vec2.
//  Parameter(s):
//    <1> glm_vec2: The glm::vec2 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the same elements
//               as glm_vec2, except expressed as a Vector2.
//
	VECTOR2_CONSTEXPR Vector2 (const glm::vec2& glm_vec2)
			: x(glm_vec2.x),
			  y(glm_vec2.y)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector2 from the specified
//           glm::ivec2.
//  Parameter(s):
//    <1> glm_ivec2: The glm::ivec2 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the same elements
//               as glm_ivec2, except expressed as a Vector2.
//
	VECTOR2_CONSTEXPR Vector2 (const glm::ivec2& glm_ivec2)
			: x(glm_ivec2.x),
			  y(glm_ivec2.y)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector2 from the specified
//           glm::uvec2.
//  Parameter(s):
//    <1> glm_uvec2: The glm::uvec2 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the same elements
//               as glm_uvec2, except expressed as a Vector2.
//
	VECTOR2_CONSTEXPR Vector2 (const glm::uvec2& glm_uvec2)
			: x(glm_uvec2.x),
			  y(glm_uvec2.y)
	{
	}

//
//  glm Vector Constructor
//
//  Purpose: To create a new Vector2 from the specified
//           glm::dvec2.
//  Parameter(s):
//    <1> glm_dvec2: The glm::dvec2 to extract the data from
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the same elements
//               as glm_dvec2, except expressed as a Vector2.
//
	VECTOR2_CONSTEXPR Vector2 (const glm::dvec2& glm_dvec2)
			: x(glm_dvec2.x),
			  y(glm_dvec2.y)
	{
	}

#endif

//
//  Copy Constructor
//
//  Purpose: To create a new Vector2 with the same elements as
//           an existing Vector2.
//  Parameter(s):
//    <1> original: The Vector2 to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new Vector2 is created with the same elements
//               as original.
//
	VECTOR2_CONSTEXPR Vector2 (const Vector2& original)
			: x(original.x),
			  y(original.y)
	{}

//
//  Destructor
//
//  Purpose: To safely destroy this Vector2 without memeory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memeory is freed.
//
	// implicitly generate trivial destructor (required for constexpr)
	// ~Vector2 () = default;  // no good way to test for support

//
//  Assignment Operator
//
//  Purpose: To set the elements of this Vector2 to be equal to
//           the elements of another.
//  Parameter(s):
//    <1> original: The Vector2 to copy
//  Precondition(s): N/A
//  Returns: A reference to this Vector2.
//  Side Effect: The elements of this Vector2 are set to the
//               elements of original.
//
	Vector2& operator= (const Vector2& original)
	{
		//  Testing for self-assignment would take
		//    longer than just copying the values.
		x = original.x;
		y = original.y;

		return *this;
	}

#ifdef OBJ_LIBRARY_GLM_INTERACTION
//
//  glm Vector Typecast
//
//  Purpose: To convert this Vector2 to a glm::vec2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, expressed as a glm::vec2.  If the
//           contents are not reperesentable as a glm::vec2, the
//           result is undefined.
//  Side Effect: N/A
//
	operator glm::vec2 () const
	{
		return glm::vec2(x, y);
	}

//
//  glm Signed Integer Vector Typecast
//
//  Purpose: To convert this Vector2 to a glm::ivec2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, expressed as a glm::ivec2.  If the
//           contents are not reperesentable as a glm::ivec2,
//           the result is undefined.
//  Side Effect: N/A
//
	operator glm::ivec2 () const
	{
		return glm::ivec2(x, y);
	}

//
//  glm Unsigned Integer Vector Typecast
//
//  Purpose: To convert this Vector2 to a glm::uvec2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, expressed as a glm::uvec2.  If the
//           contents are not reperesentable as a glm::uvec2,
//           the result is undefined.
//  Side Effect: N/A
//
	operator glm::uvec2 () const
	{
		assert(isAllComponentsNonNegative());

		return glm::uvec2(x, y);
	}

//
//  glm Double-Percision Vector Typecast
//
//  Purpose: To convert this Vector2 to a glm::dvec2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, expressed as a glm::dvec2.
//  Side Effect: N/A
//
	operator glm::dvec2 () const
	{
		return glm::dvec2(x, y);
	}
#endif

//
//  Equality Operator
//
//  Purpose: To determine if this Vector2 is equal to another.
//           Two Vector2s are equal IFF each of their elements
//           are equal.
//  Parameter(s):
//    <1> other: The Vector2 to compare to
//  Precondition(s): N/A
//  Returns: Whether this Vector2 and other are equal.
//  Side Effect: N/A
//
	bool operator== (const Vector2& other) const
	{
		if(x != other.x) return false;
		if(y != other.y) return false;
		return true;
	}

//
//  Inequality Operator
//
//  Purpose: To determine if this Vector2 and another are
//           unequal.  Two Vector2s are equal IFF each of their
//           elements are equal.
//  Parameter(s):
//    <1> other: The Vector2 to compare to
//  Precondition(s): N/A
//  Returns: Whether this Vector2 and other are unequal.
//  Side Effect: N/A
//
	bool operator!= (const Vector2& other) const
	{
		if(x != other.x) return true;
		if(y != other.y) return true;
		return false;
	}

//
//  Negation Operator
//
//  Purpose: To create a new Vector2 that is the addative
//           inverse of this Vector2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A Vector2 with elements (-x, -y).
//  Side Effect: N/A
//
	Vector2 operator- () const
	{
		return Vector2(-x, -y);
	}

//
//  Addition Operator
//
//  Purpose: To create a new Vector2 equal to the sum of this
//           Vector2 and another.
//  Parameter(s):
//    <1> right: The other Vector2
//  Precondition(s): N/A
//  Returns: A Vector2 with elements (x + right.x, y + right.y).
//  Side Effect: N/A
//
	Vector2 operator+ (const Vector2& right) const
	{
		return Vector2(x + right.x, y + right.y);
	}

//
//  Subtraction Operator
//
//  Purpose: To create a new Vector2 equal to the difference of
//           this Vector2 and another.
//  Parameter(s):
//    <1> right: The Vector2 to subtract from this Vector2
//  Precondition(s): N/A
//  Returns: A Vector2 with elements (x - other.x, y - other.y).
//  Side Effect: N/A
//
	Vector2 operator- (const Vector2& right) const
	{
		return Vector2(x - right.x, y - right.y);
	}

//
//  Multiplication Operator
//
//  Purpose: To create a new Vector2 equal to the product of
//           this Vector2 and a scalar.
//  Parameter(s):
//    <1> factor: The scalar to multiply this Vector2 by
//  Precondition(s): N/A
//  Returns: A Vector2 with elements (x * factor, y * factor).
//  Side Effect: N/A
//
	Vector2 operator* (double factor) const
	{
		return Vector2(x * factor, y * factor);
	}

//
//  Division Operator
//
//  Purpose: To create a new Vector2 equal to this Vector2
//           divided by a scalar.
//  Parameter(s):
//    <1> divisor: The scalar to divide this Vector2 by
//  Precondition(s):
//    <1> divisor != 0.0
//  Returns: A Vector2 with elements (x / divisor, y / divisor).
//  Side Effect: N/A
//
	Vector2 operator/ (double divisor) const
	{
		assert(divisor != 0.0);

		return Vector2(x / divisor, y / divisor);
	}

//
//  Addition Assignment Operator
//
//  Purpose: To set this Vector2 to the sum of itself and
//           another Vector2.
//  Parameter(s):
//    <1> right: The other Vector2
//  Precondition(s): N/A
//  Returns: A reference to this Vector2.
//  Side Effect: The elements of this Vector2 are set to
//               (x + right.x, y + right.y).
//
	Vector2& operator+= (const Vector2& right)
	{
		x += right.x;
		y += right.y;

		return *this;
	}

//
//  Subtraction Assignment Operator
//
//  Purpose: To set this Vector2 to the difference of itself and
//           another Vector2.
//  Parameter(s):
//    <1> right: The Vector2 to subtract from this Vector2
//  Precondition(s): N/A
//  Returns: A reference to this Vector2.
//  Side Effect: The elements of this Vector2 are set to
//               (x - right.x, y - right.y).
//
	Vector2& operator-= (const Vector2& right)
	{
		x -= right.x;
		y -= right.y;

		return *this;
	}

//
//  Multiplication Assignment Operator
//
//  Purpose: To set this Vector2 to the product of itself and a
//           scalar.
//  Parameter(s):
//    <1> factor: The scalar to multiply this Vector2 by
//  Precondition(s): N/A
//  Returns: A reference to this Vector2.
//  Side Effect: The elements of this Vector2 are set to
//               (x * factor, y * factor).
//
	Vector2& operator*= (double factor)
	{
		x *= factor;
		y *= factor;

		return *this;
	}

//
//  Division Assignment Operator
//
//  Purpose: To set this Vector2 to equal to the quotient of
//           itself divided by a scalar.
//  Parameter(s):
//    <1> divisor: The scalar to divide this Vector2 by
//  Precondition(s):
//    <1> divisor != 0.0
//  Returns: A reference to this Vector2.
//  Side Effect: The elements of this Vector2 are set to
//               (x / divisor, y / divisor).
//
	Vector2& operator/= (double divisor)
	{
		assert(divisor != 0.0);

		x /= divisor;
		y /= divisor;

		return *this;
	}

//
//  getAsArray
//
//  Purpose: To retreive the components of this Vector2 as an
//           array of 2 doubles.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, reinterpreted as an array of 2
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
		return &x;
	}

//
//  getAsArray
//
//  Purpose: To retreive the components of this Vector2 as an
//           array of 2 constant doubles.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: This Vector2, reinterpreted as an array of 2 const
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
		return &x;
	}

//
//  isFinite
//
//  Purpose: To determine if all components of this Vector2 are
//           finite numbers.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector2 has only finite components.
//  Side Effect: N/A
//
	bool isFinite () const
	{
		if(!VECTOR2_IS_FINITE(x)) return false;
		if(!VECTOR2_IS_FINITE(y)) return false;
		return true;
	}

//
//  isZero
//
//  Purpose: To determine if this Vector2 is the zero vector, to
//           within tolerence VECTOR2_ZERO_TOLERENCE.  The
//           tolerance is used to ensure that the norm of this
//           Vector2 (which involves squaring the value) will
//           not be 0.0 do to limitations of the representation.
//           If you need to test if a Vector2 is exactly zero,
//           use the equality test or the isZeroStrict function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector2 is within 1.0e-100 inclusive
//           of (0.0, 0.0, 0.0) in each component.
//  Side Effect: N/A
//
	bool isZero () const
	{
		if(fabs(x) > VECTOR2_ZERO_TOLERENCE) return false;
		if(fabs(y) > VECTOR2_ZERO_TOLERENCE) return false;
		return true;
	}

//
//  isZeroStrict
//
//  Purpose: To determine if all components of this Vector2 are
//           exactly 0.0.  To avoid floating point rounding
//           errors, consider using isZero instead.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector2 is equal to (0.0, 0.0, 0.0).
//  Side Effect: N/A
//
	bool isZeroStrict () const
	{
		if(x != 0.0) return false;
		if(y != 0.0) return false;
		return true;
	}

//
//  isNormal
//
//  Purpose: To determine if this Vector2 is a unit vector,
//           according to tolerance VECTOR2_NORM_TOLERANCE.
//           This function does the same thing as isUnit.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector2 has a norm of 1.0.
//  Side Effect: N/A
//
	bool isNormal () const
	{
		double norm_sqr_minus_1 = getNormSquared() - 1;

		return (fabs(norm_sqr_minus_1) <
		        VECTOR2_NORM_TOLERANCE_SQUARED);
	}

//
//  isUnit
//
//  Purpose: To determine if this Vector2 is a unit vector,
//           according to tolerance VECTOR2_NORM_TOLERANCE.
//           This function does the same thing as isNormal.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this Vector2 has a norm of 1.0.
//  Side Effect: N/A
//
	bool isUnit () const
	{
		double norm_sqr_minus_1 = getNormSquared() - 1;

		return (fabs(norm_sqr_minus_1) <
		        VECTOR2_NORM_TOLERANCE_SQUARED);
	}

//
//  getNorm
//
//  Purpose: To determine the norm of this Vector2.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The norm of this Vector2.
//  Side Effect: N/A
//
	double getNorm () const
	{
		return sqrt(x * x + y * y);
	}

//
//  getNormSquared
//
//  Purpose: To determine the square of the norm of this
//           Vector2.  This is significantly faster than
//           calculating the norm itself.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The square of the norm of this Vector2.
//  Side Effect: N/A
//
	double getNormSquared () const
	{
		return x * x + y * y;
	}

//
//  isNormEqualTo
//
//  Purpose: To determine if the norm of this Vector2 is equal
//           to the specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  This function is
//           significantly faster than calculating the norm
//           itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector2 is equal to
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
//  Purpose: To determine if the norm of this Vector2 is less
//           than the specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           less than and less than or equal.  This function is
//           significantly faster than calculating the norm
//           itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector2 is less than
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
//  Purpose: To determine if the norm of this Vector2 is greater
//           than the specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           greater than and greater than or equal.  This
//           function is significantly faster than calculating
//           the norm itself.
//  Parameter(s):
//    <1> length: The length to check against
//  Precondition(s):
//    <1> length >= 0.0
//  Returns: Whether the norm of this Vector2 is greater than
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
//  Purpose: To determine if the norm of this Vector2 is equal
//           to the norm of the specified Vector2, according to
//           tolerance VECTOR2_NORM_TOLERANCE.  This function is
//           significantly faster than calculating the norms
//           themselves.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector2 is equal to the
//           norm of other.
//  Side Effect: N/A
//
	bool isNormEqualTo (const Vector2& other) const
	{
		return isSquareTolerantEqualTo(getNormSquared(),
		                               other.getNormSquared());
	}

//
//  isNormLessThan
//
//  Purpose: To determine if the norm of this Vector2 is less
//           than the norm of the specified Vector2, according
//           to tolerance VECTOR2_NORM_TOLERANCE.  Because a
//           tolerance is used, there is no meaningful
//           difference between less than and less than or
//           equal.  This function is significantly faster than
//           calculating the norms themselves.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector2 is less than the
//           norm of other.
//  Side Effect: N/A
//
	bool isNormLessThan (const Vector2& other) const
	{
		return isSquareTolerantLessThan(getNormSquared(),
		                                other.getNormSquared());
	}

//
//  isNormGreaterThan
//
//  Purpose: To determine if the norm of this Vector2 is greater
//           than the norm of the specified Vector2, according
//           to tolerance VECTOR2_NORM_TOLERANCE.  Because a
//           tolerance is used, there is no meaningful
//           difference between greater than and greater than or
//           equal.  This function is significantly faster than
//           calculating the norms themselves.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s): N/A
//  Returns: Whether the norm of this Vector2 is greater than
//           the norm of other.
//  Side Effect: N/A
//
	bool isNormGreaterThan (const Vector2& other) const
	{
		return isSquareTolerantLessThan(other.getNormSquared(),
		                                getNormSquared());
	}

//
//  isAllComponentsNonZero
//
//  Purpose: To determine if all the elements of this Vector2
//           are non-zero values.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector2 is equal to 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonZero () const
	{
		if(x == 0.0) return false;
		if(y == 0.0) return false;
		return true;
	}

//
//  isAllComponentsPositive
//
//  Purpose: To determine if all the elements of this Vector2
//           are positive.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector2 is less than or
//           equal to 0.0, false is returned.  Otherwise, true
//           is returned.
//  Side Effect: N/A
//
	bool isAllComponentsPositive () const
	{
		if(x <= 0.0) return false;
		if(y <= 0.0) return false;
		return true;
	}

//
//  isAllComponentsNegative
//
//  Purpose: To determine if all the elements of this Vector2
//           are negative.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector2 is reater than or
//           equal to 0.0, false is returned.  Otherwise, true
//           is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNegative () const
	{
		if(x >= 0.0) return false;
		if(y >= 0.0) return false;
		return true;
	}

//
//  isAllComponentsNonPositive
//
//  Purpose: To determine if all the elements of this Vector2
//           are non-positive.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector2 is greater than 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonPositive () const
	{
		if(x > 0.0) return false;
		if(y > 0.0) return false;
		return true;
	}

//
//  isAllComponentsNonNegative
//
//  Purpose: To determine if all the elements of this Vector2
//           are non-negative.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If any element of this Vector2 is less than 0.0,
//           false is returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isAllComponentsNonNegative () const
	{
		if(x < 0.0) return false;
		if(y < 0.0) return false;
		return true;
	}

//
//  isAllComponentsEqualTo
//
//  Purpose: To determine if all components of this Vector2 are
//           equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is equal to
//           value.
//  Side Effect: N/A
//
	bool isAllComponentsEqualTo (double value) const
	{
		assert(isFinite());

		if(x != value) return false;
		if(y != value) return false;
		return true;
	}

//
//  isAllComponentsNotEqualTo
//
//  Purpose: To determine if all components of this Vector2 are
//           not equal to the specified value.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is not equal
//           to value.
//  Side Effect: N/A
//
	bool isAllComponentsNotEqualTo (double value) const
	{
		assert(isFinite());

		if(x == value) return false;
		if(y == value) return false;
		return true;
	}

//
//  isAllComponentsLessThan
//
//  Purpose: To determine if all components of this Vector2 are
//           less than the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is less than
//           value.
//  Side Effect: N/A
//
	bool isAllComponentsLessThan (double value) const
	{
		assert(isFinite());

		if(x >= value) return false;
		if(y >= value) return false;
		return true;
	}

//
//  isAllComponentsLessThanOrEqual
//
//  Purpose: To determine if all components of this Vector2 are
//           less than or equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is less than
//           or equal to value.
//  Side Effect: N/A
//
	bool isAllComponentsLessThanOrEqual (double value) const
	{
		assert(isFinite());

		if(x > value) return false;
		if(y > value) return false;
		return true;
	}

//
//  isAllComponentsGreaterThan
//
//  Purpose: To determine if all components of this Vector2 are
//           greater than the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is greater
//           than value.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThan (double value) const
	{
		assert(isFinite());

		if(x <= value) return false;
		if(y <= value) return false;
		return true;
	}

//
//  isAllComponentsGreaterThanOrEqual
//
//  Purpose: To determine if all components of this Vector2 are
//           greater than or equal to the specified value.
//  Parameter(s):
//    <1> value: The value to compare to
//  Precondition(s):
//    <1> isFinite()
//  Returns: Whether each component of this Vector2 is greater
//           than or equal to value.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThanOrEqual (
	                                     double value) const
	{
		assert(isFinite());

		if(x < value) return false;
		if(y < value) return false;
		return true;
	}

//
//  isAllComponentsNotEqualTo
//
//  Purpose: To determine if all components of this Vector2 are
//           not equal to than the corresponding components of
//           another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector2 is not equal
//           to the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsNotEqualTo (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x == other.x) return false;
		if(y == other.y) return false;
		return true;
	}

//
//  isAllComponentsLessThan
//
//  Purpose: To determine if all components of this Vector2 are
//           less than the corresponding components of
//           another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector2 is less than
//           the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsLessThan (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x >= other.x) return false;
		if(y >= other.y) return false;
		return true;
	}

//
//  isAllComponentsLessThanOrEqual
//
//  Purpose: To determine if all components of this Vector2 are
//           less than or equal to the corresponding
//           components of another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector2 is less than
//           or equal to the corresponding component of
//           other.
//  Side Effect: N/A
//
	bool isAllComponentsLessThanOrEqual (
	                             const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x > other.x) return false;
		if(y > other.y) return false;
		return true;
	}

//
//  isAllComponentsGreaterThan
//
//  Purpose: To determine if all components of this Vector2 are
//           greater than the corresponding components of
//           another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector2 is greater
//           than the corresponding component of other.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThan (
	                             const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x <= other.x) return false;
		if(y <= other.y) return false;
		return true;
	}

//
//  isAllComponentsGreaterThanOrEqual
//
//  Purpose: To determine if all components of this Vector2 are
//           greater than or equal to the corresponding
//           components of another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether each component of this Vector2 is greater
//           than or equal to the corresponding component
//           of other.
//  Side Effect: N/A
//
	bool isAllComponentsGreaterThanOrEqual (
	                             const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(x < other.x) return false;
		if(y < other.y) return false;
		return true;
	}

//
//  getComponentX
//
//  Purpose: To create another Vector2 with the same X component
//           as this Vector2 and the Y component set to 0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector2 containing only the X
//           component.
//  Side Effect: N/A
//
	Vector2 getComponentX () const
	{
		return Vector2(x, 0.0);
	}

//
//  getComponentY
//
//  Purpose: To create another Vector2 with the same Y component
//           as this Vector2 and the X component set to 0.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A copy of this Vector2 containing only the Y
//           component.
//  Side Effect: N/A
//
	Vector2 getComponentY () const
	{
		return Vector2(0.0, y);
	}

//
//  getNormalized
//
//  Purpose: To create a normalized copy of this Vector2.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//  Returns: A Vector2 with the same direction as this Vector2
//           and a norm of 1.0.
//  Side Effect: N/A
//
	Vector2 getNormalized () const
	{
		assert(isFinite());
		assert(!isZero());

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();
		return Vector2(x * norm_ratio, y * norm_ratio);
	}

//
//  getNormalizedSafe
//
//  Purpose: To create a normalized copy of this Vector2 without
//           crashing if this Vector2 is the zero vector.  This
//           function is slower than the getNormalized function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: If this Vector2 is the zero vector, (1.0, 0.0) is
//           returned.  Otherwise, a Vector2 with the same
//           direction as this Vector2 and a norm of 1.0 is
//           returned.
//  Side Effect: N/A
//
	Vector2 getNormalizedSafe () const
	{
		assert(isFinite());

		if(isZero())
			return Vector2(1.0, 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();
		return Vector2(x * norm_ratio, y * norm_ratio);
	}

//
//  getCopyWithNorm
//
//  Purpose: To create a Vector2 with the same direction as this
//           Vector2 and the specified norm.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> norm >= 0.0
//  Returns: A Vector2 with the same direction as this Vector2
//           and a norm of norm.
//  Side Effect: N/A
//
	Vector2 getCopyWithNorm (double norm) const
	{
		assert(isFinite());
		assert(!isZero());
		assert(norm >= 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = norm / getNorm();
		return Vector2(x * norm_ratio, y * norm_ratio);
	}

//
//  getCopyWithNormSafe
//
//  Purpose: To create a Vector2 with the same direction as this
//           Vector2 and the specified norm.  This function will
//           not crash if this Vector2 is the zero vector, but
//           is slower than the getCopyWithNorm function.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: If this Vector2 is the zero vector, (norm, 0.0) is
//           returned.  Otherwise, a Vector2 with the same
//           direction as this Vector2 and a norm of norm is
//           returned.
//  Side Effect: N/A
//
	Vector2 getCopyWithNormSafe (double norm) const
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isZero())
			return Vector2(norm, 0.0);

		assert(getNorm() != 0.0);
		double norm_ratio = norm / getNorm();
		return Vector2(x * norm_ratio, y * norm_ratio);
	}

//
//  getTruncated
//
//  Purpose: To create a Vector2 with the same direction as this
//           Vector2 and a norm no greater than the specified
//           value.
//  Parameter(s):
//    <1> norm: The new maximum norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: If this Vector2 has a norm greater than norm, a
//           Vector2 with the same direction as this Vector2 and
//           a norm of norm is returned.  Otherwise, a copy of
//           this Vector2 is returned.
//  Side Effect: N/A
//
	Vector2 getTruncated (double norm) const
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isNormGreaterThan(norm))
		{
			double norm_ratio = norm / getNorm();
			return Vector2(x * norm_ratio, y * norm_ratio);
		}
		else
			return *this;  // invokes copy constructor
	}

//
//  setZero
//
//  Purpose: To change this Vector2 to be the zero vector.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector2 is set to (0.0, 0.0).
//
	void setZero ()
	{
		x = 0.0;
		y = 0.0;
	}

//
//  set
//
//  Purpose: To change this Vector2 to have the specified
//           elements.
//  Parameter(s):
//    <1> X: The new Y element
//    <2> Y: The new X element
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector2 is set to (X, Y).
//
	void set (double X, double Y)
	{
		x = X;
		y = Y;
	}

//
//  setAll
//
//  Purpose: To change all the element of this Vector2 to be the
//           specified value.  This function is slightly more
//           efficient than calling set with the same parameter
//           multiple times.
//  Parameter(s):
//    <1> v: The new value
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector2 is set to (v, v).
//
	void setAll (double v)
	{
		x = v;
		y = v;
	}

//
//  addComponents
//
//  Purpose: To increase the elements of this Vector2 by the
//           specified amounts.
//  Parameter(s):
//    <1> X: The increase for the X element
//    <2> Y: The increase for the Y element
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector2 is set to (x + X, y + Y).
//
	void addComponents (double X, double Y)
	{
		x += X;
		y += Y;
	}

//
//  addComponentsAll
//
//  Purpose: To increase all the element of this Vector2 by the
//           specified value.  This function is slightly more
//           efficient than calling add with the same parameter
//           multiple times.
//  Parameter(s):
//    <1> v: The increase for the elements
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This Vector2 is set to (x + v, y + v).
//
	void addComponentsAll (double v)
	{
		x += v;
		y += v;
	}

//
//  normalize
//
//  Purpose: To change this Vector2 have a norm of 1.0.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//  Returns: N/A
//  Side Effect: This Vector2 is set to have a norm of 1.0.  The
//               direction of this Vector2 is unchanged.
//
	void normalize ()
	{
		assert(isFinite());
		assert(!isZero());

		assert(getNorm() != 0.0);
		double norm_ratio = 1.0 / getNorm();

		x *= norm_ratio;
		y *= norm_ratio;

		assert(isNormal());
	}

//
//  normalizeSafe
//
//  Purpose: To change this Vector2 have a norm of 1.0.  This
//           function is slower than the normalize function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: N/A
//  Side Effect: If this Vector3 is the zero vector, it is set
//               to be (1.0, 0.0).  Otherwise, it is set to have
//               a norm of 1.0 and the direction of this Vector2
//               is unchanged.
//
	void normalizeSafe ()
	{
		assert(isFinite());

		if(isZero())
			set(1.0, 0.0);
		else
		{
			assert(getNorm() != 0.0);
			double norm_ratio = 1.0 / getNorm();

			x *= norm_ratio;
			y *= norm_ratio;

			assert(isNormal());
		}
	}

//
//  setNorm
//
//  Purpose: To change the norm of this Vector2.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> norm >= 0.0
//  Returns: N/A
//  Side Effect: This Vector2 is set to have a norm of norm.
//               The direction of this Vector2 is unchanged.
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

		assert(isNormEqualTo(norm));
	}

//
//  setNormSafe
//
//  Purpose: To change the norm of this Vector2 to the specified
//           value.  This function is slower than the normalize
//           function.
//  Parameter(s):
//    <1> norm: The new norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: N/A
//  Side Effect: If this Vector2 is the zero vector, it is set
//               to be (norm, 0.0).  Otherwise, it is set to
//               have a norm of 1.0 and the direction of this
//               Vector2 is unchanged.
//
	void setNormSafe (double norm)
	{
		assert(isFinite());
		assert(norm >= 0.0);

		if(isZero())
			set(norm, 0.0);
		else
		{
			assert(getNorm() != 0.0);
			double norm_ratio = norm / getNorm();

			x *= norm_ratio;
			y *= norm_ratio;

			assert(isNormEqualTo(norm));
		}
	}

//
//  truncate
//
//  Purpose: To reduce the norm of this Vector2 to the specified
//           if it is currently greater.
//  Parameter(s):
//    <1> norm: The new maximum norm
//  Precondition(s):
//    <1> isFinite()
//    <2> norm >= 0.0
//  Returns: N/A
//  Side Effect: If this Vector2 has a norm greater than norm,
//               is set to have a norm of norm.  Otherwise there
//               is no effect.  In either case, the direction of
//               this Vector2 is unchanged.
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
//           Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector2 with elements (x * other.x, y * other.y).
//  Side Effect: N/A
//
	Vector2 getComponentProduct (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector2(x * other.x, y * other.y);
	}

//
//  getComponentRatio
//
//  Purpose: To calculate the component-wise ratio of this
//           Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> other.isAllComponentsNonZero()
//  Returns: A Vector2 with elements (x / other.x, y / other.y).
//  Side Effect: N/A
//
	Vector2 getComponentRatio (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(other.isAllComponentsNonZero());

		return Vector2(x / other.x, y / other.y);
	}

//
//  getComponentRatioSafe
//
//  Purpose: To calculate the component-wise ratio of this
//           Vector2 and another Vector2 without crashing
//           if one of the elements of the second Vector2
//           is zero.  This function is slower than
//           componentRatio.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector2 with elements (x / other.x, y / other.y).
//           If either of the elements in other is zero,
//           the corresponding element of this Vector2 is
//           returned for that element instead of a ratio.
//  Side Effect: N/A
//
	Vector2 getComponentRatioSafe (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector2((other.x != 0.0) ? (x / other.x) : x,
		               (other.y != 0.0) ? (y / other.y) : y);
	}

//
//  getNormRatio
//
//  Purpose: To calculate the ratio of the norms of this Vector2
//           and another parallel Vector2.  This function takes
//           the direction of the Vector2s into account, so if
//           they are facing in opposite directions, the ratio
//           will be negative.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> isParallel(other)
//    <4> !other.isZero()
//  Returns: The ratio of the norms of this Vector2 and other.
//  Side Effect: N/A
//
	double getNormRatio (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(isParallel(other));
		assert(!other.isZero());

		if(other.x != 0.0)
			return x / other.x;
		else
		{
			assert(other.y != 0.0);
			return y / other.y;
		}
	}

//
//  getNormRatioSafe
//
//  Purpose: To calculate the ratio of the norms of this Vector2
//           and another parallel Vector2 without crashing if
//           all of the elements of the second Vector2 are zero.
//           This function takes the direction of the Vector2s
//           into account, so if the Vector2s are facing in
//           opposite directions the ratio will be negative.
//           This function is slower than getNormRatio.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> isParallel(other)
//  Returns: The ratio of the norms of this Vector2 and other.
//           If all of the elements in other are zero, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getNormRatioSafe (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());
		assert(isParallel(other));

		if(other.x != 0.0)
			return x / other.x;
		else if(other.y != 0.0)
			return y / other.y;
		else
			return 0.0;
	}

//
//  dotProduct
//
//  Purpose: To determine the dot/scaler/inner product of this
//           Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: *this (dot) other.
//  Side Effect: N/A
//
	double dotProduct (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return x * other.x + y * other.y;
	}

//
//  getPerpendicular
//
//  Purpose: To calculate a Vector2 perpendicular to this
//           Vector2.  This is a 2D analog of a cross product.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: A Vector2 the same length as this Vector2, but
//           rotated 90 degrees clockwise.  The returned Vector2
//           will be a zero vector if and only if this Vector2
//           is a zero vector.
//  Side Effect: N/A
//
	Vector2 getPerpendicular () const
	{
		assert(isFinite());

		return Vector2(y, -x);
	}

//
//  getMinComponents
//
//  Purpose: To determine the minimum values for each component
//           from this Vector2 and the specified value.
//  Parameter(s):
//    <1> n: The other value
//  Precondition(s):
//    <1> isFinite()
//  Returns: A Vector2, each component of which is the smaller
//           of n and the corresponding component of this
//           Vector2.
//  Side Effect: N/A
//
	Vector2 getMinComponents (double n) const
	{
		assert(isFinite());

		return Vector2((x < n) ? x : n,
		               (y < n) ? y : n);
	}

//
//  getMinComponents
//
//  Purpose: To determine the minimum values for each component
//           from this Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector2, each component of which is the smaller
//           of the corresponding components of this Vector2 and
//           other.
//  Side Effect: N/A
//
	Vector2 getMinComponents (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector2((x < other.x) ? x : other.x,
		               (y < other.y) ? y : other.y);
	}

//
//  getMaxComponents
//
//  Purpose: To determine the maximum values for each component
//           from this Vector2 and the specified value.
//  Parameter(s):
//    <1> n: The other value
//  Precondition(s):
//    <1> isFinite()
//  Returns: A Vector2, each component of which is the larger of
//           n and the corresponding component of this Vector2.
//  Side Effect: N/A
//
	Vector2 getMaxComponents (double n) const
	{
		assert(isFinite());

		return Vector2((x > n) ? x : n,
		               (y > n) ? y : n);
	}

//
//  getMaxComponents
//
//  Purpose: To determine the maximum values for each component
//           from this Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: A Vector2, each component of which is the larger of
//           the corresponding components of this Vector2 and
//           other.
//  Side Effect: N/A
//
	Vector2 getMaxComponents (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return Vector2((x > other.x) ? x : other.x,
		               (y > other.y) ? y : other.y);
	}

//
//  getStaturated
//
//  Purpose: To create a Vector2 with each component in the
//           range between 0.0 and 1.0.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A Vector2, each component of which is based on the
//           corresponding component of this Vector2.  If the
//           component of this Vector2 is less than 0.0, the
//           component of the new Vector2 contains 0.0.  If the
//           component is larger than 1.0, it contains the 1.0.
//           Otherwise, the new Vector2 contains the
//           corresponing component from this Vector2.
//  Side Effect: N/A
//
	Vector2 getStaturated (double min, double max) const
	{
		assert(isFinite());
		assert(min <= max);

		return Vector2((x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x,
		               (y < 0.0) ? 0.0 : (y > 1.0) ? 1.0 : y);
	}

//
//  getClampedComponents
//
//  Purpose: To create a Vector2 with each component in the
//           range between the specified values.
//  Parameter(s):
//    <1> min: The minimum value
//    <2> max: The minimum value
//  Precondition(s):
//    <1> isFinite()
//    <2> min <= max
//  Returns: A Vector2, each component of which is based on the
//           corresponding componet of this Vector2, min, and
//           max.  If the component of this Vector2 is less than
//           min, the component of the new Vector2 contains min.
//           If the component is larger than max, it contains
//           max.  Otherwise, the new Vector2 contains the
//           corresponing component from this Vector2.
//  Side Effect: N/A
//
	Vector2 getClampedComponents (double min, double max) const
	{
		assert(isFinite());
		assert(min <= max);

		return Vector2((x < min) ? min : (x > max) ? max : x,
		               (y < min) ? min : (y > max) ? max : y);
	}

//
//  getClampedComponents
//
//  Purpose: To create a Vector2 with each component in the
//           range between the specified Vector2s.
//  Parameter(s):
//    <1> min: A Vector2 containing the minimum values
//    <2> max: A Vector2 containing the minimum values
//  Precondition(s):
//    <1> isFinite()
//    <2> min.isFinite()
//    <3> max.isFinite()
//    <4> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A Vector2, each component of which is based on the
//           corresponding component of this Vector2, min, and
//           max.  If the component of this Vector2 is less than
//           the corresponing component of min, the component of
//           new Vector2 contains the component of min.  If the
//           component is larger than max, it contains the
//           component of max.  Otherwise, the new Vector2
//           contains the corresponing component from this
//           Vector2.
//  Side Effect: N/A
//
	Vector2 getClampedComponents (const Vector2& min,
	                              const Vector2& max) const
	{
		assert(isFinite());
		assert(min.isFinite());
		assert(max.isFinite());
		assert(min.isAllComponentsLessThanOrEqual(max));

		return Vector2((x < min.x) ? min.x
		                           : ((x > max.x) ? max.x : x),
		               (y < min.y) ? min.y
		                           : ((y > max.y) ? max.y : y));
	}

//
//  getDistance
//
//  Purpose: To determine the Euclidian distance between this
//           Vector2 and another Vector2.  If you only need to
//           compare the distance to another Vector2 with some
//           value, consider using one of the isDistanceEqual,
//           isDistanceLessThan, or isDistanceGreaterThan
//           funtions.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Euclidean distance between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getDistance (const Vector2& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;
		return sqrt(diff_x * diff_x + diff_y * diff_y);
	}

//
//  getDistanceSquared
//
//  Purpose: To determine the square of the Euclidian distance
//           between this Vector2 and another Vector2.  This
//           function is significantly faster than
//           getDistance().
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The square of the Euclidian distance between this
//           Vector2 and other.
//  Side Effect: N/A
//
	double getDistanceSquared (const Vector2& other) const
	{
		double diff_x = x - other.x;
		double diff_y = y - other.y;

		return diff_x * diff_x + diff_y * diff_y;
	}

//
//  isDistanceEqualTo
//
//  Purpose: To determine if the Euclidian distance between this
//           Vector2 and another Vector2 is equal to the
//           specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector2
//    <2> distance: The comparison distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector2
//           and other is equal to distance.
//  Side Effect: N/A
//
	bool isDistanceEqualTo (const Vector2& other,
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
//           Vector2 and another Vector2 is less than the
//           specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector2
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector2
//           and other is less than distance.
//  Side Effect: N/A
//
	bool isDistanceLessThan (const Vector2& other,
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
//           Vector2 and another Vector2 is greater than the
//           specified value, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  This function is
//           significantly faster than getDistance().
//  Parameter(s):
//    <1> other: The other Vector2
//    <2> distance: The cutoff distance
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//    <3> distance >= 0.0
//  Returns: Whether the Euclidian distance between this Vector2
//           and other is greater than distance.
//  Side Effect: N/A
//
	bool isDistanceGreaterThan (const Vector2& other,
	                            double distance) const
	{
		assert(distance >= 0.0);

		return isSquareTolerantLessThan(
		                             distance * distance,
		                             getDistanceSquared(other));
	}

//
//  getManhattenDistance
//
//  Purpose: To determine the Manhatten distance between this
//           Vector2 and another Vector2.  This is the sum of
//           the differences between corresponding components.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The Manhatten distance between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getManhattenDistance (const Vector2& other) const
	{
		return fabs(x - other.x) + fabs(y - other.y);
	}

//
//  getChessboardDistance
//
//  Purpose: To determine the chessboard distance between this
//           Vector2 and another Vector2.  This is the largest
//           differences between corresponding components.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The chessboard distance between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getChessboardDistance (
	                             const Vector2& other) const
	{
		double dx = fabs(x - other.x);
		double dy = fabs(y - other.y);

		return (dx < dy) ? dy : dx;
	}

//
//  isParallel
//
//  Purpose: To determine whether this Vector2 is parallel to
//           the specified Vector2, according to tolerance
//           VECTOR2_NORM_TOLERANCE.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector2 is parallel to other.  The
//           zero vector is assumed to be parallel to all
//           Vector2s.
//  Side Effect: N/A
//
	bool isParallel (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		return isOrthogonal(Vector2(other.y, -other.x));
	}

//
//  isParallelNormal
//
//  Purpose: To determine whether this Vector2 is parallel to
//           the specified normal Vector2, according to
//           tolerance VECTOR2_NORM_TOLERANCE.  This Vector2 is
//           assumed to also be a normal vector.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector2 is parallel to other.  The
//           zero vector is assumed to be parallel to all
//           Vector2s.
//  Side Effect: N/A
//
	bool isParallelNormal (const Vector2& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		return isOrthogonalNormal(Vector2(other.y, -other.x));
	}

//
//  isSameDirection
//
//  Purpose: To determine whether this Vector2 is pointing in
//           the same direction as the specified Vector2,
//           according to tolerance VECTOR2_NORM_TOLERANCE.  To
//           be pointing in the same direction, 2 Vector2s must
//           be parallel and have the same sign on each of their
//           corresponding components.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector2 is pointing in the same
//           direction as other.  The zero vector is assumed to
//           point in the same direction as all Vector2s.
//  Side Effect: N/A
//
	bool isSameDirection (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(isZero() || other.isZero())
			return true;
		else if(getNormalized().getCosAngleNormal(
		                                other.getNormalized()) >
		        VECTOR2_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isSameDirectionNormal
//
//  Purpose: To determine whether this Vector2 is pointing in
//           the same direction as the specified normal Vector2,
//           according to tolerance VECTOR2_NORM_TOLERANCE.
//           This Vector2 is assumed to also be a normal vector.
//           To be pointing in the same direction, 2 Vector2s
//           must be parallel and have the same sign on each of
//           their corresponding components.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector2 is pointing in the same
//           direction as other.  The zero vector is assumed to
//           point in the same direction as all Vector2s.
//  Side Effect: N/A
//
	bool isSameDirectionNormal (const Vector2& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		if(getCosAngleNormal(other) >
		   VECTOR2_ONE_MINUS_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isSameHemisphere
//
//  Purpose: To determine whether this Vector2 is pointing
//           within 90 degrees of the specified Vector2.  In
//           most cases, either this Vector2 or its negation
//           will be in the same hemisphere as the specified
//           Vector2, although this is not guarenteed in
//           borderline cases.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector2 is pointing within 90 degrees
//           of other.  The zero vector is assumed to be in the
//           same hemisphere as all Vector2s.
//  Side Effect: N/A
//
	bool isSameHemisphere (const Vector2& other) const
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
//  Purpose: To determine whether this Vector2 is orthogonal to
//           the specified Vector2, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  Orthogonal is the same as
//           perpendicular except that orthogonal is defined for
//           zero vectors.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: Whether this Vector2 is orthogonal to other.  The
//           zero vector is orthogonal to all Vector2s including
//           itself.
//  Side Effect: N/A
//
	bool isOrthogonal (const Vector2& other) const
	{
		assert(isFinite());
		assert(other.isFinite());

		if(isZero() || other.isZero())
			return true;
		else if(fabs(getNormalized().getCosAngleNormal(
		                               other.getNormalized())) <
		        VECTOR2_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  isOrthogonalNormal
//
//  Purpose: To determine whether this Vector2 is orthogonal to
//           the specified normla Vector2, according to
//           tolerance VECTOR2_NORM_TOLERANCE.  This Vector2 is
//           also assumed to be a normal vector.  Orthogonal is
//           the same as perpendicular except that orthogonal is
//           defined for zero vectors.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: Whether this Vector2 is orthogonal to other.  The
//           zero vector is orthogonal to all Vector2s including
//           itself.
//  Side Effect: N/A
//
	bool isOrthogonalNormal (const Vector2& other) const
	{
		assert(isFinite());
		assert(isNormal());
		assert(other.isFinite());
		assert(other.isNormal());

		if(fabs(getCosAngleNormal(other)) <
		   VECTOR2_NORM_TOLERANCE_SQUARED)
		{
			return true;
		}
		else
			return false;
	}

//
//  getProjection
//
//  Purpose: To determine the projection of this Vector2 onto
//           another Vector2.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> !project_onto.isZero()
//  Returns: The projection of this Vector2 onto project_onto.
//           This is the component of this Vector2 with the same
//           (or opposite) direction as project_onto.
//  Side Effect: N/A
//
	Vector2 getProjection (const Vector2& project_onto) const
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
//  Purpose: To determine the portion of this Vector2 that
//           remains after the projection onto another Vector2
//           is subtracted off.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> !project_onto.isZero()
//  Returns: This Vector2 minus its projection onto
//           project_onto. This is the component of this Vector2
//           at right angles to project_onto.
//  Side Effect: N/A
//
	Vector2 getAntiProjection (
	                      const Vector2& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(!project_onto.isZero());

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector2::ZERO;

		Vector2 projection = getProjection(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getProjectionSafe
//
//  Purpose: To determine the projection of this Vector2 onto
//           another Vector2, without crashing if the other
//           Vector2 is zero.  This function is slower than
//           getProjection.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//  Returns: The projection of this Vector2 onto project_onto.
//           This is the component of this Vector2 with the same
//           (or opposite) direction as project_onto.  If
//           project_onto is zero, the zero vector is returned.
//  Side Effect: N/A
//
	Vector2 getProjectionSafe (
	                      const Vector2& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());

		if(project_onto.isZero())
			return Vector2::ZERO;

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
//  Purpose: To determine the portion of this Vector2 that
//           remains after the projection onto another Vector2
//           is subtracted off, without crashing if the other
//           Vector2 is zero.  This function is slower than
//           getAntiProjection.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//  Returns: This Vector2 minus its projection onto
//           project_onto. This is the component of this Vector2
//           at right angles to project_onto.  If
//           project_onto is zero, a copy of this Vector2 is
//           returned.
//  Side Effect: N/A
//
	Vector2 getAntiProjectionSafe (
	                      const Vector2& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());

		if(project_onto.isZero())
			return *this;

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector2::ZERO;

		Vector2 projection = getProjectionSafe(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getProjectionNormal
//
//  Purpose: To determine the projection of this Vector2 onto a
//           Vector2 that is a normal vector.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> project_onto.isNormal()
//  Returns: The projection of this Vector2 onto project_onto.
//           This is the component of this Vector2 with the same
//           (or opposite) direction as project_onto.
//  Side Effect: N/A
//
	Vector2 getProjectionNormal (const Vector2& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(project_onto.isNormal());

		double dot_product = dotProduct(project_onto);

		assert((project_onto * dot_product).isParallel(project_onto));
		return project_onto * dotProduct(project_onto);
	}

//
//  getAntiProjectionNormal
//
//  Purpose: To determine the portion of this Vector2 that
//           remains after the projection onto a Vector2 that is
//           a normal vector is subtracted off.
//  Parameter(s):
//    <1> project_onto: The Vector2 to be projected onto 
//  Precondition(s):
//    <1> isFinite()
//    <2> project_onto.isFinite()
//    <3> project_onto.isNormal()
//  Returns: This Vector2 minus its projection onto
//           project_onto. This is the component of this Vector2
//           at right angles to project_onto.
//  Side Effect: N/A
//
	Vector2 getAntiProjectionNormal (
	                      const Vector2& project_onto) const
	{
		assert(isFinite());
		assert(project_onto.isFinite());
		assert(project_onto.isNormal());

		// prevent random vector of floating point rounding errors
		if(isParallel(project_onto))
			return Vector2::ZERO;

		Vector2 projection = getProjectionNormal(project_onto);
		assert((operator-(projection)).isOrthogonal(project_onto));
		return operator-(projection);
	}

//
//  getReflection
//
//  Purpose: To determine the result of reflecting this Vector2
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
//  Returns: The resulting vector when this Vector2 reflects off
//           a surface with normal vector surface_normal.
//  Side Effect: N/A
//
	Vector2 getReflection (const Vector2& surface_normal) const
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
//  Purpose: To determine the result of reflecting this Vector2
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
//  Returns: The resulting vector when this Vector2 reflects off
//           a surface with normal vector surface_normal.  If
//           surface_normal is zero, a copy of this Vector2 is
//           returned.
//  Side Effect: N/A
//
	Vector2 getReflectionSafe (
	                        const Vector2& surface_normal) const
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
//  Purpose: To determine the result of reflecting this Vector2
//           off of a surface with the specified normalized
//           normal vector.
//  Parameter(s):
//    <1> surface_normal: The normal vector of the surface to
//                        reflect off of
//  Precondition(s):
//    <1> isFinite()
//    <2> surface_normal.isFinite()
//    <3> surface_normal.isNormal()
//  Returns: The resulting vector when this Vector2 reflects off
//           a surface with normal vector surface_normal.
//  Side Effect: N/A
//
	Vector2 getReflectionNormal (
	                        const Vector2& surface_normal) const
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
//           Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> other.isFinite()
//    <4> !other.isZero()
//  Returns: The cosine of the angle between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getCosAngle (const Vector2& other) const;

//
//  getCosAngleNormal
//
//  Purpose: To determine the cosine of the angle between this
//           Vector2 and another Vector2 when both are normal
//           vectors.  This function is faster than getCosAngle.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: The cosine of the angle between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getCosAngleNormal (const Vector2& other) const;

//
//  getCosAngleSafe
//
//  Purpose: To determine the cosine of the angle between this
//           Vector2 and another Vector2, without crashing if
//           one of the Vector2s is zero.  This function is
//           slower than getCosAngle.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The cosine of the angle between this Vector2 and
//           other.  If either vector is zero, cos(0) = 1 is
//           returned.
//  Side Effect: N/A
//
	double getCosAngleSafe (const Vector2& other) const;

//
//  getAngle
//
//  Purpose: To determine the angle in radians between this
//           Vector2 and another Vector2.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//    <3> other.isFinite()
//    <4> !other.isZero()
//  Returns: The angle in radians between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getAngle (const Vector2& other) const;

//
//  getAngleNormal
//
//  Purpose: To determine the angle in radians between this
//           Vector2 and another Vector2 when both are normal
//           vectors.  This function is faster than getAngle.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> isNormal()
//    <3> other.isFinite()
//    <4> other.isNormal()
//  Returns: The angle in radians between this Vector2 and
//           other.
//  Side Effect: N/A
//
	double getAngleNormal (const Vector2& other) const;

//
//  getAngleSafe
//
//  Purpose: To determine the angle in radians between this
//           Vector2 and another Vector2, without crashing if
//           one of the Vector2s is zero.  This function is
//           slower than getAngle.
//  Parameter(s):
//    <1> other: The other Vector2
//  Precondition(s):
//    <1> isFinite()
//    <2> other.isFinite()
//  Returns: The angle in radians between this Vector2 and
//           other.  If either vector is zero, 0 is returned.
//  Side Effect: N/A
//
	double getAngleSafe (const Vector2& other) const;

//
//  getRotation
//
//  Purpose: To determine how far this Vector2 is rotated in
//           radians, assuming the original Vector2 was facing
//           in the X+ direction.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//    <2> !isZero()
//  Returns: The current rotation of this Vector2 in radians.
//  Side Effect: N/A
//
	double getRotation () const
	{
		assert(isFinite());
		assert(!isZero());

		return atan2(y, x);
	}

//
//  getRotationSafe
//
//  Purpose: To determine how far this Vector2 is rotated in
//           radians, assuming the original Vector2 was facing
//           in the X+ direction.  This function will not crash
//           if this Vector2 is the zero vector, but it is
//           slower than the getRotation function.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isFinite()
//  Returns: The current rotation of this Vector2 in radians.
//           If this Vector2 is the zero vector, 0.0 is
//           returned.
//  Side Effect: N/A
//
	double getRotationSafe () const
	{
		assert(isFinite());

		if(isZero())
			return 0.0;
		else
			return atan2(y, x);
	}

//
//  getRotated
//
//  Purpose: To create a copy of this Vector2 by the specified
//           angle in radians.
//  Parameter(s):
//    <1> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector2 rotated radians radians.
//  Side Effect: N/A
//
	Vector2 getRotated (double radians) const
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		return Vector2(cos_angle * x - sin_angle * y,
		               sin_angle * x + cos_angle * y);
	}

//
//  getRotatedTowards
//
//  Purpose: To create a copy of this Vector2 rotated towards
//           the specified direction by up to the specified
//           angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//  Returns: If desired is parallel to this Vector2, this
//           Vector2 will be returned.  Otherwise, a copy of
//           this Vector2 rotated up to radians radians towards
//           direction desired is returned.  It will be as close
//           to desired as possible, given the starting
//           direction and radians. Rotating zero vector will
//           always produce the zero vector.
//  Side Effect: N/A
//
	Vector2 getRotatedTowards (const Vector2& desired,
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
//  Purpose: To create a copy of this Vector2 rotated towards
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
//           Vector2, a copy of this Vector2 is returned.
//           Otherwise, a copy of this Vector2 rotated up to
//           radians radians towards direction desired is
//           returned.  It will be as close to desired as
//           possible, given the starting direction and radians.
//           Rotating zero vector will always produce the zero
//           vector.
//  Side Effect: N/A
//
	Vector2 getRotatedTowardsSafe (const Vector2& desired,
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
//  Purpose: To create a copy of this Vector2 rotated towards
//           the specified direction by up to the specified
//           angle.  The axis is assumed to be a normal vector.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> desired.isNormal()
//  Returns: If desired is parallel to this Vector2, this
//           Vector2 will be returned.  Otherwise, a copy of
//           this Vector2 rotated up to radians radians towards
//           direction desired is returned.  It will be as close
//           to desired as possible, given the starting
//           direction and radians. Rotating zero vector will
//           always produce the zero vector.
//  Side Effect: N/A
//
	Vector2 getRotatedTowardsNormal (const Vector2& desired,
	                                 double radians) const;

//
//  rotate
//
//  Purpose: To rotate this Vector2 by the specified angle in
//           radians.
//  Parameter(s):
//    <1> radians: The angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//  Returns: A copy of this Vector2 rotated radians radians.
//  Side Effect: N/A
//
	void rotate (double radians)
	{
		assert(isFinite());

		double sin_angle = sin(radians);
		double cos_angle = cos(radians);

		set(cos_angle * x - sin_angle * y,
		    sin_angle * x + cos_angle * y);
	}

//
//  rotateTowards
//
//  Purpose: To rotate this Vector2 towards the specified
//           direction by up to the specified angle.
//  Parameter(s):
//    <1> desired: The direction to rotate towards
//    <2> radians: The maximum angle to rotate in radians
//  Precondition(s):
//    <1> isFinite()
//    <2> desired.isFinite()
//    <3> !desired.isZero()
//  Returns: N/A
//  Side Effect: If desired is parallel to this Vector2, there
//               is no effect.  Otherwise, this Vector2 is
//               rotated up to radians radians towards direction
//               desired, putting it as close as possible given
//               the starting direction and radians.  Rotating
//               the zero vector will always produce the zero
//               vector.
//
	void rotateTowards (const Vector2& desired,
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
//  Purpose: To rotate this Vector2 towards the specified
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
//               this Vector2, there is no effect.  Otherwise,
//               this Vector2 will be rotated up to radians 
//               radians towards direction desired, putting it
//               as close as possible, given the starting
//               direction and radians.  Rotating zero vector
//               will always produce the zero vector.
//
	void rotateTowardsSafe (const Vector2& desired,
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
//  Purpose: To rotate this Vector2 towards the specified
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
//  Side Effect: If desired is parallel to this Vector2, there
//               is no effect.  Otherwise, this Vector2 is
//               rotated up to radians radians towards direction
//               desired, putting it as close as possible given
//               the starting direction and radians.  Rotating
//               the zero vector will always produce the zero
//               vector.
//
	void rotateTowardsNormal (const Vector2& desired,
	                          double radians);

//
//  getRandomUnitVector
//
//  Purpose: To generate a Vector2 of norm 1.0 and with a
//           uniform random direction.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random unit vector.
//  Side Effect: N/A
//
	static Vector2 getRandomUnitVector ();

//
//  getPseudorandomUnitVector
//
//  Purpose: To generate a Vector2 with norm 1.0 and with a
//           uniform direction based on the specified seed
//           value.  If the seed is generated pseudorandomly,
//           this function will return consistant pseudorandom
//           unit vectors.
//  Parameter(s):
//    <1> seed: The seed value
//  Precondition(s):
//    <1> seed >= 0.0
//    <2> seed <  1.0
//  Returns: A uniform unit vector calculated from seed.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomUnitVector (double seed);

//
//  getRandomSphereVector
//
//  Purpose: To generate a Vector2 of uniform distribution and
//           a norm of less than or equal to 1.0.  In effect,
//           this is a vector to somewhere within a circle of
//           radius 1.0.  This function is named "sphere"
//           instead of "circle" to correspond with the Vector2
//           class.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A uniform random vector with a norm of no more than
//           1.0.
//  Side Effect: N/A
//
	static Vector2 getRandomSphereVector ();

//
//  getPseudorandomSphereVector
//
//  Purpose: To generate a Vector2 of uniform distribution and
//           a norm of less than or equal to 1.0 based on the
//           specified seed values.  In effect, this is a vector
//           to somewhere within a sphere of radius 1.0.  If the
//           seeds are generated pseudorandomly, this function
//           will return consistant pseudorandom vectors.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A uniform vector with a norm of no more than 1.0
//           that has been calculated from seed1 and seed2.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomSphereVector (double seed1,
	                                            double seed2);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector2 with each component in
//           the range [0, 1).
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           (1.0, 1.0).  For each component, 0.0 is included
//           but 1.0 is excluded.
//  Side Effect: N/A
//
	static Vector2 getRandomInRange ();

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector2 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> max: The maximum value for components
//  Precondition(s):
//    <1> max >= 0.0
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           (max, max).  For each component, 0.0 is included
//           but max is excluded.  If max is 0.0, the zero
//           vector is returned.
//  Side Effect: N/A
//
	static Vector2 getRandomInRange (double max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector2 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> max: A Vector2 containing the maximum component values
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           max.  For each component, 0.0 is included but the
//           corresponding component of max is excluded.  If any
//           component of max is 0.0, 0.0 is returned for that
//           component.
//  Side Effect: N/A
//
	static Vector2 getRandomInRange (const Vector2& max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector2 with each component
//           less than the specified maximum.
//  Parameter(s):
//    <1> min: The minimum value for components
//    <2> max: The maximum value for components
//  Precondition(s):
//    <1> min <= max
//  Returns: A random Vector2 in the range from (min, min) to
//           (max, max).  For each component, min is included
//           but max is excluded.  If min and max are equal,
//           (min, min) is returned.
//  Side Effect: N/A
//
	static Vector2 getRandomInRange (double min, double max);

//
//  getRandomInRange
//
//  Purpose: To generate a random Vector2 with each component
//           between the corresponding components of the
//           specified vectors.
//  Parameter(s):
//    <1> min: A Vector2 containing the minimum component values
//    <2> max: A Vector2 containing the maximum component values
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A random Vector2 in the range from min to max.  For
//           each component, min is included but max is excluded.
//  Side Effect: N/A
//
	static Vector2 getRandomInRange (const Vector2& min,
	                                 const Vector2& max);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector2 with each component in the
//           range [0, 1) and based on the specified seed
//           values.
//  Parameter(s):
//    <1> seed1: The first seed value
//    <2> seed2: The second seed value
//  Precondition(s):
//    <1> seed1 >= 0.0
//    <2> seed1 <  1.0
//    <3> seed2 >= 0.0
//    <4> seed2 <  1.0
//  Returns: A Vector2 in the range from (0.0, 0.0) to
//           (1.0, 1.0) that has been calculated from seed1 and
//           seed2.  For each component, 0.0 is included but 1.0
//           is excluded.  This function really just combines
//           the seed values into a Vector2, but it does have
//           the appropriate distribution.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomInRange (double seed1,
	                                       double seed2);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector2 with each component less than
//           the specified maximum and based on the specified
//           seed values.
//  Parameter(s):
//    <1> max: The maximum value for components
//    <2> seed1: The first seed value
//    <3> seed2: The second seed value
//  Precondition(s):
//    <1> max >= 0.0
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           (max, max) that has been calculated from seed1 and
//           seed2.  For each component, 0.0 is included but max
//           is excluded.  If max is 0.0, the zero vector is
//           returned.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomInRange (double max,
	                                       double seed1,
	                                       double seed2);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector2 with each component less than
//           the specified maximum and based on the specified
//           seed values.
//  Parameter(s):
//    <1> max: A Vector2 containing the maximum component values
//    <2> seed1: The first seed value
//    <3> seed2: The second seed value
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           max that has been calculated from seed1 and seed2.
//           For each component, 0.0 is included but the
//           corresponding component of max is excluded.  If any
//           component of max is 0.0, 0.0 is returned for that
//           component.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomInRange (const Vector2& max,
	                                       double seed1,
	                                       double seed2);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector2 with each component less than
//           the specified values and based on the specified
//           seed values.
//  Parameter(s):
//    <1> min: The minimum value for components
//    <2> max: The maximum value for components
//    <3> seed1: The first seed value
//    <4> seed2: The second seed value
//  Precondition(s):
//    <1> min <= max
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//  Returns: A random Vector2 in the range from (min, min) to
//           (max max) that has been calculated from seed1 and
//           seed2.  For each component, min is included but max
//           is excluded.  If min and max are equal, (min, min)
//           is returned.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomInRange (double min,
	                                       double max,
	                                       double seed1,
	                                       double seed2);

//
//  getPseudorandomInRange
//
//  Purpose: To generate a Vector2 with each component between
//           the corresponding components of the specified
//           vectors and based on the specified seed values.
//  Parameter(s):
//    <1> min: A Vector2 containing the minimum component values
//    <2> max: A Vector2 containing the maximum component values
//    <3> seed1: The first seed value
//    <4> seed2: The second seed value
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//    <2> seed1 >= 0.0
//    <3> seed1 <  1.0
//    <4> seed2 >= 0.0
//    <5> seed2 <  1.0
//  Returns: A Vector2 in the range from min to max that has
//           been calculated from seed1 and seed2.  For each
//           component, min is included but max is excluded.  If
//           for any component min and max are equal, the shared
//           value is returned for that component.
//  Side Effect: N/A
//
	static Vector2 getPseudorandomInRange (const Vector2& min,
	                                       const Vector2& max,
	                                       double seed1,
	                                       double seed2);

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector2 with each component in
//           the range [0, 1].
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           (1.0, 1.0, 1.0).  For each component, both 0.0 and
//           1.0 are included.
//  Side Effect: N/A
//
	static Vector2 getRandomInRangeInclusive ();

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector2 with each component
//           less than or equal to the specified maximum.
//  Parameter(s):
//    <1> max: A Vector2 containing the maximum component values
//  Precondition(s):
//    <1> max.isAllComponentsGreaterThanOrEqual(ZERO)
//  Returns: A random Vector2 in the range from (0.0, 0.0) to
//           max.  For each component, both 0.0 and the
//           corresponding component of max are included.
//  Side Effect: N/A
//
	static Vector2 getRandomInRangeInclusive (const Vector2& max);

//
//  getRandomInRangeInclusive
//
//  Purpose: To generate a random Vector2 with each component
//           between the corresponding components of the
//           specified vectors, including the minimum and
//           maximum component values.
//  Parameter(s):
//    <1> min: A Vector2 containing the minimum component values
//    <2> max: A Vector2 containing the maximum component values
//  Precondition(s):
//    <1> min.isAllComponentsLessThanOrEqual(max)
//  Returns: A random Vector2 in the range from min to max.  For
//           each component, both min and max are included.
//  Side Effect: N/A
//
	static Vector2 getRandomInRangeInclusive (
	                                    const Vector2& min,
	                                    const Vector2& max);

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
	static Vector2 getClosestPointOnLine (const Vector2& l1,
	                                      const Vector2& l2,
	                                      const Vector2& p,
	                                      bool bounded);

private:
//
//  isSquareTolerantEqualTo
//
//  Purpose: To determine if the specified squares are equal,
//           according to tolerance VECTOR2_NORM_TOLERANCE.
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

		return (a <= b * VECTOR2_NORM_TOLERANCE_PLUS_ONE_SQUARED) &&
		       (b <= a * VECTOR2_NORM_TOLERANCE_PLUS_ONE_SQUARED);
	}

//
//  isSquareTolerantLessThan
//
//  Purpose: To determine if the specified square is less than
//           the other specified square, according to tolerance
//           VECTOR2_NORM_TOLERANCE.  Because a tolerance is
//           used, there is no meaningful difference between
//           less than and less than or equal.
//  Parameter(s):
//    <1> a: The first square
//    <2> b: The square square
//  Precondition(s):
//    <1> a >= 0.0
//    <2> b >= 0.0
//  Returns: Whether a is close enough to less than b that
//           sqrt(a) < sqrt(b) * (VECTOR2_NORM_TOLERANCE + 1.0).
//  Side Effect: N/A
//
	bool isSquareTolerantLessThan (double a, double b) const
	{
		assert(a >= 0.0);
		assert(b >= 0.0);

		return (a <= b * VECTOR2_NORM_TOLERANCE_PLUS_ONE_SQUARED);
	}

};  // end of Vector2 class



//
//  Multiplication Operator
//
//  Purpose: To create a new Vector2 equal to the product of
//           the specified scalar and the specified Vector2.
//  Parameter(s):
//    <1> scalar: The scalar
//    <2> vector: The Vector2
//  Precondition(s): N/A
//  Returns: A Vector2 with elements
//           (vector.x * scalar, vector.y * scalar).
//  Side Effect: N/A
//
inline Vector2 operator* (double scalar, const Vector2& vector)
{
	return Vector2(vector.x * scalar, vector.y * scalar);
}

//
//  Stream Insertion Operator
//
//  Purpose: To print the specified Vector2 to the specified
//           output stream.
//  Parameter(s):
//    <1> r_os: The output stream
//    <2> vector: The Vector2
//  Precondition(s): N/A
//  Returns: A reference to r_os.
//  Side Effect: vector is printed to r_os.
//
std::ostream& operator<< (std::ostream& r_os,
                          const Vector2& vector);



//
//  Clear preprocessor macros from above so they don't cause
//    trouble elsewhere.
//
#undef VECTOR2_IS_FINITE
#undef VECTOR2_CONSTEXPR



}  // end of namespace ObjLibrary

#endif
