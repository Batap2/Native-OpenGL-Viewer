// **********************************************************************
// * FILE  : Point3D.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Point3D
// *---------------------------------------------------------------------
// **********************************************************************



// ----------------------------------------------------------------------
// +---------
// | Life-cycle
// +---------
// Constructors
#include "Point3D.h"

#ifndef P3DINL_H
#define P3DINL_H

Point3D::Point3D()
{
}

Point3D::Point3D( const Float& x , const Float& y , const Float& z ) : x(x), y(y), z(z)
{
}

Point3D::Point3D( const Point3D& P ) : x(P.x), y(P.y), z(P.z)
{
}



// Set (constructors-like)
void Point3D::Set( const Float& fValue )
{
    x = y = z = fValue;
}
void Point3D::Set( const Float& x , const Float& y , const Float& z )
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point3D::Set( const Point3D& P )
{
    x = P.x;
    y = P.y;
    z = P.z;
}

void Point3D::Set( const Point3D& A, const Point3D& B)
{
	x = B.x - A.x;
	y = B.y - A.y;
	z = B.z - A.z;
}


// Assignement
Point3D& Point3D::operator =( const Point3D& V )
{
    x = V.x;
    y = V.y;
    z = V.z;
	
    return *this;
}

// Assignement
Point3D& Point3D::operator =( const Float& V )
{
    x = V;
    y = V;
	z = V;
    return *this;
}



// ----------------------------------------------------------------------
// +---------
// | Addition/Substraction
// +---------
// With another vector (Internal laws)


Point3D operator + ( const Point3D& U , const Point3D& V )
{
    return Point3D(U.x + V.x, U.y + V.y, U.z + V.z);
}


Point3D operator - ( const Point3D& U , const Point3D& V )
{
	return Point3D(U.x - V.x, U.y - V.y, U.z - V.z);
}

// With assignement
Point3D& Point3D::operator += ( const Point3D& V )
{
    x += V.x;
    y += V.y;
    z += V.z;
	
    return *this;
}
Point3D& Point3D::operator -= ( const Point3D& V )
{
    x -= V.x;
    y -= V.y;
    z -= V.z;
	
    return *this;
}

// Unary
Point3D operator - ( const Point3D& V )
{
    return Point3D(-V.x, -V.y, -V.z);
}


// ----------------------------------------------------------------------
// +---------
// | Multiplication/Division
// +---------
// With a scalar (External laws)
Point3D operator * ( const Float& fValue , const Point3D& V )
{
    return Point3D(fValue * V.x, fValue * V.y, fValue * V.z);
}

Point3D operator * ( const Point3D& V , const Float& fValue )
{
    return Point3D(V.x * fValue, V.y * fValue, V.z * fValue);
}

Point3D operator * ( Point3D V , const Point3D& W )
{
	V *= W;
	return V;
}

Point3D operator / ( const Float& fValue , const Point3D& V )
{
    return Point3D(fValue / V.x, fValue / V.y, fValue / V.z);
}

Point3D operator / ( const Point3D& V , const Float& fValue )
{
    return Point3D(V.x / fValue, V.y / fValue, V.z / fValue);
}

Point3D operator / ( Point3D V , const Point3D& W )
{
	V /= W;
	return V;
}


// With assignement
Point3D& Point3D::operator *= ( const Float& fValue )
{
    x *= fValue;
    y *= fValue;
    z *= fValue;
	
    return *this;
}

// add the same value to the 3 coords
Point3D& Point3D::operator += (const Float& fValue)
{
    x += fValue;
    y += fValue;
    z += fValue;

    return *this;
}

Point3D& Point3D::operator /= ( const Float& fValue )
{
	const Float invfValue = 1.0f / fValue;
    x *= invfValue;
    y *= invfValue;
    z *= invfValue;
	
    return *this;
}

// remove the same value to the 3 coords
Point3D& Point3D::operator -= (const Float& fValue)
{
    x -= fValue;
    y -= fValue;
    z -= fValue;

    return *this;
}



Point3D& Point3D::operator *= ( const Point3D& p )
{
    x *= p.x;
    y *= p.y;
    z *= p.z;
    return *this;
}

Point3D& Point3D::operator /= ( const Point3D& p )
{
    x /= p.x;
    y /= p.y;
    z /= p.z;
    return *this;
}


Point3D Hadamard(Point3D a, const Point3D& b)
{
	a *= b;
	return a;
}



// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
Float Norm( const Point3D& P )
{
	return sqrt( P.x*P.x + P.y*P.y + P.z*P.z );
}

Float NormSquare( const Point3D& P )
{
    return P.x*P.x + P.y*P.y + P.z*P.z;
}

Float Dist( const Point3D& P1, const Point3D& P2 )
{
	const Float dx = P2.x - P1.x;
	const Float dy = P2.y - P1.y;
	const Float dz = P2.z - P1.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

Float DistSquare( const Point3D& P1, const Point3D& P2 )
{
	const Float dx = P2.x - P1.x;
	const Float dy = P2.y - P1.y;
	const Float dz = P2.z - P1.z;
	return dx*dx + dy*dy + dz*dz;
}

#define SMALL_NUM  0.001f



// ----------------------------------------------------------------------
// +---------
// | Utilities
// +---------
void Point3D::Normalize( void )
{
    Float tmp = Norm( *this );
    if (tmp != 0.0f)
	{
		tmp = 1.0f/tmp;
		x *= tmp;
		y *= tmp;
		z *= tmp;
	}
}

inline Point3D Point3D::Normalized( void ) const
{
	Point3D result = *this;
	result.Normalize();
	return result;
}









/*
bool operator<( const Point3D& U , const Point3D& V )
{
	return (U.x < V.x && U.y < V.y && U.z < V.z);
}

bool operator<=( const Point3D& U , const Point3D& V )
{
	return (U.x <= V.x && U.y <= V.y && U.z <= V.z);
}

bool operator>( const Point3D& U , const Point3D& V )
{
	return (U.x > V.x && U.y > V.y && U.z > V.z);
}

bool operator>=( const Point3D& U , const Point3D& V )
{
	return (U.x >= V.x && U.y >= V.y && U.z >= V.z);
}*/

bool operator==( const Point3D& U , const Point3D& V )
{
	return (U.x == V.x && U.y == V.y && U.z == V.z);
}

bool operator!=( const Point3D& U , const Point3D& V )
{
	return (U.x != V.x || U.y != V.y || U.z != V.z);
}





// +---------
// | Acces Operators
// +---------
const Float& Point3D::operator[](Int i) const
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}

Float& Point3D::operator[](Int i)
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}


void Point3D::ClampMax(const Float v)
{
	if (x>v)
		x = v;
	if (y>v)
		y = v;
	if (z>v)
		z = v;
}

void Point3D::ClampMin(const Float v)
{
	if (x<v)
		x = v;
	if (y<v)
		y = v;
	if (z<v)
		z = v;
}





// ----------------------------------------------------------------------
// +---------
// | Cross product
// +---------
Point3D operator ^ ( const Point3D& U , const Point3D& V )
{
    return Point3D((U.y * V.z) - (U.z * V.y),
				   (U.z * V.x) - (U.x * V.z),
				   (U.x * V.y) - (U.y * V.x));
}

// With assignement
const Point3D& Point3D::operator ^= ( const Point3D& V )
{
	Float _x = x, _y = y;
    x = (_y * V.z) - (z * V.y);
    y = (z * V.x) - (_x * V.z);
    z = (_x * V.y) - (_y * V.x);
	
    return *this;
}

void Point3D::CrossProduct( const Point3D& V, const Point3D& W )
{
    x = (V.y * W.z) - (V.z * W.y);
    y = (V.z * W.x) - (V.x * W.z);
    z = (V.x * W.y) - (V.y * W.x);
}

// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
Float Dot( const Point3D& U , const Point3D& V )
{
    return ((U.x * V.x) + (U.y * V.y) + (U.z * V.z));
}





#undef SMALL_NUM
#endif