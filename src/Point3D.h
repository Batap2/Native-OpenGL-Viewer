// **********************************************************************
// * FILE  : Point3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 3D Point
// * COMMENT :Only difference with Vector3D is that Matrix3x4 tranform applies the translation
// *---------------------------------------------------------------------
// ***********************************************************************

#include <cmath>
#include <cassert>

#ifndef _Point3D_h_
#define _Point3D_h_

// ----------------------------------------------------------------------
// **************************************************
// * Point3D
// *-------------------------------------------------
// * - Definition of 3D vector
// * - 
// **************************************************
using Float = float;
using Int = int;
#define KFLOAT_ONE 1.0f;

struct Point3D
{
	// +---------
    // | Life-cyle
    // +---------
    // Constructors
    inline Point3D(); // DO NOTHING! (For immediate creation)
	
	// Set all coordinates to fValue
    inline Point3D( const Float& x , const Float& y , const Float& z );
    inline Point3D( const Point3D& P );


    // Set (constructors-like)
    inline void Set( const Float& fValue );
    inline void Set( const Float& x , const Float& y , const Float& z );
    inline void Set( const Point3D& V );
	// Vector A to B = B-A
	inline void Set( const Point3D& A, const Point3D& B);
    //inline void Set( const Vector3D& P );
	
    // Assignement
    inline Point3D& operator =( const Point3D& V );
	inline Point3D& operator =(const Float& V);
	
    // +---------
    // | Addition/Substraction
    // +---------
    // With another vector (Internal laws)
    friend inline Point3D operator + ( const Point3D& U , const Point3D& V );
    friend inline Point3D operator - ( const Point3D& U , const Point3D& V );
	
    // With assignement
    inline Point3D& operator += ( const Point3D& V );
    inline Point3D& operator -= ( const Point3D& V );
	
    // Unary
    friend inline Point3D operator - ( const Point3D& V );
	
    
    // +---------
    // | Multiplication/Division
    // +---------
    // With a scalar (External laws)
    friend inline Point3D operator * ( const Float& fValue , const Point3D& V );
    friend inline Point3D operator * ( const Point3D& V , const Float& fValue );
	
	// Avoid using this, it's too ambiguous (dot, cross or hadamard)
	friend inline Point3D operator * ( Point3D V , const Point3D& W );
	
	// Same as a*b but the intent is more clear
	friend inline Point3D Hadamard(Point3D a, const Point3D& b);
	
    friend inline Point3D operator / ( const Float& fValue , const Point3D& V );
    friend inline Point3D operator / ( const Point3D& V , const Float& fValue );
	
	friend inline Point3D operator / ( Point3D V , const Point3D& W );
	
    // With assignement
    inline Point3D& operator *= ( const Float& fValue );
	inline Point3D& operator += ( const Float& fValue );
	inline Point3D& operator -= ( const Float& fValue );
    inline Point3D& operator /= ( const Float& fValue );
	
	inline Point3D& operator *= ( const Point3D& p);
	inline Point3D& operator /= ( const Point3D& p);
	
    // +---------
    // | Euclidian operations
    // +---------
    friend inline Float Norm( const Point3D& P );
    friend inline Float NormSquare( const Point3D& P );
	friend inline Float Dist( const Point3D& P1, const Point3D& P2 );
	friend inline Float DistSquare( const Point3D& P1, const Point3D& P2 );
	friend inline Float SegmentDist(const Point3D& Pt1, const Point3D& Pt2,const Point3D& Pt3, const Point3D& Pt4);
	friend inline Float PointToSegmentDist(const Point3D& Pt, const Point3D& Pt1,const Point3D& Pt2,Point3D& nearest,bool& insideSegment);
	
	
	
	// +---------
    // | Cross product
    // +---------
    friend inline Point3D operator ^ ( const Point3D& U , const Point3D& V );
	
    // With assignement
    inline const Point3D& operator ^= ( const Point3D& V );
	inline void CrossProduct( const Point3D& V, const Point3D& W );
	
    // +---------
    // | Euclidian operations
    // +---------
    friend inline Float Dot( const Point3D& U , const Point3D& V );
	
	
	
    // +---------
    // | Affine operations
    // +---------

	
    // +---------
    // | Utilities
    // +---------
    inline void Normalize( void );
	inline Point3D Normalized( void ) const;
	inline void ClampMax(const Float v);
	inline void ClampMin(const Float v);
	
	friend inline bool operator< (const Point3D& U, const Point3D& V);
	friend inline bool operator<=(const Point3D& U, const Point3D& V);
	friend inline bool operator> (const Point3D& U, const Point3D& V);
	friend inline bool operator>=(const Point3D& U, const Point3D& V);
	friend inline bool operator==(const Point3D& a, const Point3D& b);
	friend inline bool operator!=(const Point3D& a, const Point3D& b);
	

    
	// +---------
	// | Acces Operators
	// +---------
	inline const Float& operator[](Int i) const;
	inline Float& operator[](Int i);


    // +---------
    // | Coordinates
    // +---------

	union
	{
		struct
		{
			Float   x;
			Float   y;
			Float   z;
		};


		Float data[3];
	};
};

// define struct Segment3D


#endif //_Point3D_h_


