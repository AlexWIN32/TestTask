/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <math.h>
#include <vector>
#include <Vector2Fwd.h>

template<class T>
struct BaseVector2
{	
	T x, y;
	BaseVector2(float X, float Y):x(X), y(Y){}
	BaseVector2() : x(T()), y(T()){}
	static float Dot(const BaseVector2<T> &A, const BaseVector2<T> &B) 
	{
		return A.x * B.x + A.y * B.y;
	}	
	float Lenght() const { return sqrt((x * x) + (y * y));}	
	void Normalize()
	{
		float len = Lenght();
		x /= len;
		y /= len;
	}
    static BaseVector2<T> Normalize(const BaseVector2<T> &V)
    {
        BaseVector2<T> out = V;
        out.Normalize();
        return out;
    }
    static float Lenght(const BaseVector2<T> &V) 
    {
        return V.Lenght();
    }
	BaseVector2<T> operator + ( const BaseVector2<T> &Val) const
	{
		return BaseVector2<T>(x + Val.x, y + Val.y);
	}
    BaseVector2<T> operator + () const
    {
        return *this;
    }
    BaseVector2<T> operator - ( const BaseVector2<T> &Val) const
	{
		return BaseVector2<T>(x - Val.x, y - Val.y);
	}
	const BaseVector2<T> operator - () const
	{
		return BaseVector2<T>(-x, -y);
	}
	BaseVector2<T> operator * ( const T &Val) const
	{
		return BaseVector2<T>(x * Val, y * Val);
	}
	BaseVector2<T> operator / ( const T &Val) const
	{
		return BaseVector2<T>(x / Val, y / Val);
	}
	BaseVector2<T>& operator += ( const BaseVector2<T> &Val)
	{
		x += Val.x;
		y += Val.y;
		return *this;
	}
	BaseVector2<T>& operator -= ( const BaseVector2<T> &Val)
	{
		x -= Val.x;
		y -= Val.y;
		return *this;
	}    
	BaseVector2<T>& operator *= ( const T &Val)
	{
		x *= Val;
		y *= Val;
		return *this;
	}        
	BaseVector2<T>& operator /= ( const T &Val)
	{
		x /= Val;
		y /= Val;
		return *this;
	}
	bool operator == (const BaseVector2<T> & Val) const
	{
		return x == Val.x && y == Val.y; 
	}
	bool operator != (const BaseVector2<T> & Val) const
	{
		return !operator == (Val);
	}
};

template<class T>
struct BaseVector3
{
    T x = T(), y = T(), z = T();
    BaseVector3(){}
    BaseVector3(T X, T Y, T Z) : x(X), y(Y), z(Z){}
    BaseVector3(const BaseVector2<T> &V, T Z) : x(V.x), y(V.y), z(Z) {}
    static float Dot(const BaseVector3<T> &A, const BaseVector3<T> &B) 
    {
        return A.x * B.x + A.y * B.y + A.z * B.z;
    }
    static BaseVector3<T> Cross(const BaseVector3<T> &A, const BaseVector3<T> &B)
    {
        BaseVector3<T> out;
        out.x = A.y * B.z - A.z * B.y;
        out.y = A.z * B.x - A.x * B.z;
        out.z = A.x * B.y - A.y * B.x;
        return out;
    }
    static BaseVector3<T> Normalize(const BaseVector3<T> &In)
    {
        BaseVector3<T> out = In;
        out.Normalize();
        return out;
    }
    static float Lenght(const BaseVector3<T> &V) 
    {
        return V.Lenght();
    }
    float Lenght() const { return sqrt((x * x) + (y * y) + (z * z));}
    void Normalize()
    {
        float len = Lenght();

        if(len == 0.0f)
            return;

        x /= len;
        y /= len;
        z /= len;
    }
    BaseVector3<T> operator + ( const BaseVector3<T> &Val) const
    {
        return {x + Val.x, y + Val.y, z + Val.z};
    }
    BaseVector3<T> operator + () const
    {
        return *this;
    }
    BaseVector3<T> operator - ( const BaseVector3<T> &Val) const
    {
        return {x - Val.x, y - Val.y, z - Val.z};
    }
    BaseVector3<T> operator - () const
    {
        return {-x, -y, -z};
    }
    BaseVector3<T> operator * ( const T &Val) const
    {
        return {x * Val, y * Val, z * Val};
    }
    BaseVector3<T> operator / ( const T &Val) const
    {
        return {x / Val, y / Val, z / Val};
    }
    BaseVector3<T>& operator += ( const BaseVector3<T> &Val)
    {
        x += Val.x;
        y += Val.y;
        z += Val.z;
        return *this;
    }
    BaseVector3<T>& operator -= ( const BaseVector3<T> &Val)
    {
        x -= Val.x;
        y -= Val.y;
        z -= Val.z;
        return *this;
    }
    BaseVector3<T>& operator *= ( const T &Val)
    {
        x *= Val;
        y *= Val;
        z *= Val;
        return *this;
    }
    BaseVector3<T>& operator /= ( const T &Val)
    {
        x /= Val;
        y /= Val;
        z /= Val;
        return *this;
    }
    bool operator == (const BaseVector3<T> & Val) const
    {
        return x == Val.x && y == Val.y && z == Val.z; 
    }
    bool operator != (const BaseVector3<T> & Val) const
    {
        return !operator == (Val);
    }
};

template<class T>
struct BasePoint2
{
	T x, y;
	BasePoint2(T X, T Y) : x(X), y(Y) {}
	BasePoint2() : x(T()), y(T()){}
    BasePoint2(const BaseVector2<T> &V) : x(V.x), y(V.y) {}
	BasePoint2<T> operator + ( const BasePoint2<T> &Val) const
	{
		return BasePoint2<T>(x + Val.x, y + Val.y);
	}
    BaseVector2<T> operator - ( const BasePoint2<T> &Val) const
	{
		return BaseVector2<T>(x - Val.x, y - Val.y);
	}

    BasePoint2<T> operator + ( const BaseVector2<T> &Val) const
	{
        return BaseVector2<T>(x + Val.x, y + Val.y);
	}
    BasePoint2<T> operator - ( const BaseVector2<T> &Val) const
	{
        return BaseVector2<T>(x - Val.x, y - Val.y);
	}

	BasePoint2<T> operator * ( const T &Val) const
	{
		return BasePoint2<T>(x * Val, y * Val);
	}
	BasePoint2<T> operator / ( const T &Val) const
	{
		return BasePoint2<T>(x / Val, y / Val);
	}
	BasePoint2<T>& operator += ( const BasePoint2<T> &Val)
	{
		x += Val.x;
		y += Val.y;
		return *this;
	}
	BasePoint2<T>& operator -= ( const BasePoint2<T> &Val)
	{
		x -= Val.x;
		y -= Val.y;
		return *this;
	}    
	BasePoint2<T>& operator += ( const BaseVector2<T> &Val)
	{
		x += Val.x;
		y += Val.y;
		return *this;
	}
	BasePoint2<T>& operator -= ( const BaseVector2<T> &Val)
	{
		x -= Val.x;
		y -= Val.y;
		return *this;
	}  
	BasePoint2<T>& operator *= ( const T &Val)
	{
		x *= Val;
		y *= Val;
		return *this;
	}        
	BasePoint2<T>& operator /= ( const T &Val)
	{
		x /= Val;
		y /= Val;
		return *this;
	}
	bool operator == (const BasePoint2<T> & Val) const
	{
		return x == Val.x && y == Val.y; 
	}
	bool operator != (const BasePoint2<T> & Val) const
	{
		return !operator == (Val);
	}
    BasePoint2<T> operator - () const
    {
        return BasePoint2<T>(-x, -y);
    }
    BasePoint2<T> operator + () const
    {
        return *this;
    }
};

template<class T>
struct BasePoint3
{
    T x = T(), y = T(), z = T();
    BasePoint3(){}
    BasePoint3(T X, T Y, T Z) : x(X), y(Y), z(Z){}
    BasePoint3(const BasePoint2<T> &P, T Z) : x(P.x), y(P.y), z(Z){}
    BasePoint3(const BaseVector3<T> &V): x(V.x), y(V.y), z(V.z){}
    BasePoint3<T> operator + ( const BasePoint3<T> &Val) const
    {
        return {x + Val.x, y + Val.y, z + Val.z};
    }
    BaseVector3<T> operator - ( const BasePoint3<T> &Val) const
    {
        return {x - Val.x, y - Val.y, z - Val.z};
    }
    BasePoint3<T> operator + ( const BaseVector3<T> &Val) const
    {
        return {x + Val.x, y + Val.y, z + Val.z};
    }
    BasePoint3<T> operator - ( const BaseVector3<T> &Val) const
    {
        return {x - Val.x, y - Val.y, z - Val.z};
    }
    BasePoint3<T> operator * ( const T &Val) const
    {
        return {x * Val, y * Val, z * Val};
    }
    BasePoint3<T> operator / ( const T &Val) const
    {
        return {x / Val, y / Val, z / Val};
    }
    BasePoint3<T>& operator += ( const BasePoint3<T> &Val)
    {
        x += Val.x;
        y += Val.y;
        z += Val.z;
        return *this;
    }
    BasePoint3<T>& operator -= ( const BasePoint3<T> &Val)
    {
        x -= Val.x;
        y -= Val.y;
        z -= Val.z;
        return *this;
    }
    BasePoint3<T>& operator += ( const BaseVector3<T> &Val)
    {
        x += Val.x;
        y += Val.y;
        z += Val.z;
        return *this;
    }
    BasePoint3<T>& operator -= ( const BaseVector3<T> &Val)
    {
        x -= Val.x;
        y -= Val.y;
        z -= Val.z;
        return *this;
    }
    BasePoint3<T>& operator *= ( const T &Val)
    {
        x *= Val;
        y *= Val;
        z *= Val;
        return *this;
    }
    BasePoint3<T>& operator /= ( const T &Val)
    {
        x /= Val;
        y /= Val;
        z /= Val;
        return *this;
    }
    bool operator == (const BasePoint3<T> & Val) const
    {
        return x == Val.x && y == Val.y && z == Val.z; 
    }
    bool operator != (const BasePoint3<T> & Val) const
    {
        return !operator == (Val);
    }
    BasePoint3<T> operator - () const
    {
        return {-x, -y, -z};
    }
    BasePoint3<T> operator + () const
    {
        return *this;
    }
};

template<class T>
struct BasePoint4
{
    T x, y, z, w;
    BasePoint4() : x(T()), y(T()), z(T()), w(T()){}
    BasePoint4(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W){}
    BasePoint4(const BasePoint3<T> &Point, T W) : x(Point.x), y(Point.y), z(Point.z), w(W){}
    BasePoint4<T> operator + ( const BasePoint4<T> &Val) const
    {
        return {x + Val.x, y + Val.y, z + Val.z, w + Val.w};
    }
    BasePoint4<T> operator + () const
    {
        return *this;
    }
    BasePoint4<T> operator - ( const BasePoint4<T> &Val) const
    {
        return {x - Val.x, y - Val.y, z - Val.z, w + Val.w};
    }
    BasePoint4<T> operator - () const
    {
        return {-x, -y, -z, -w};
    }
    BasePoint4<T> operator * ( const T &Val) const
    {
        return {x * Val, y * Val, z * Val, w * Val};
    }
    BasePoint4<T> operator / ( const T &Val) const
    {
        return {x / Val, y / Val, z / Val, w / Val};
    }
    BasePoint4<T>& operator += ( const BasePoint4<T> &Val)
    {
        x += Val.x;
        y += Val.y;
        z += Val.z;
        w += Val.w;
        return *this;
    }
    BasePoint4<T>& operator -= ( const BasePoint4<T> &Val)
    {
        x -= Val.x;
        y -= Val.y;
        z -= Val.z;
        w -= Val.w;
        return *this;
    }
    BasePoint4<T>& operator *= ( const T &Val)
    {
        x *= Val;
        y *= Val;
        z *= Val;
        w *= Val;
        return *this;
    }
    BasePoint4<T>& operator /= ( const T &Val)
    {
        x /= Val;
        y /= Val;
        z /= Val;
        w /= Val;
        return *this;
    }
    bool operator == (const BasePoint4<T> & Val) const
    {
        return x == Val.x && y == Val.y && z == Val.z && w == Val.w; 
    }
    bool operator != (const BasePoint4<T> & Val) const
    {
        return !operator == (Val);
    }
};

template <class T>
struct BaseColor
{
    T r, g, b, a;
    BaseColor() : r(T()), g(T()), b(T()), a(T()){}
    BaseColor(T R, T G, T B, T A) : r(R), g(G), b(B), a(A){}
    BaseColor(const BaseVector4<T> &V): r(V.x), g(V.y), b(V.z), a(V.w) {}
    BaseColor<T> operator + ( const BaseColor<T> &Val) const
    {
        return {r + Val.r, g + Val.g, b + Val.b, a + Val.a};
    }
    BaseColor<T> operator + () const
    {
        return *this;
    }
    BaseColor<T> operator - ( const BaseColor<T> &Val) const
    {
        return {r - Val.r, g - Val.g, b - Val.b, a - Val.a};
    }
    BaseColor<T> operator - () const
    {
        return {-r, -g, -b, -a};
    }
    BaseColor<T> operator * ( const T &Val) const
    {
        return {r * Val, g * Val, b * Val, a * Val};
    }
    BaseColor<T> operator / ( const T &Val) const
    {
        return {r / Val, g / Val, b / Val, a / Val};
    }
    BaseColor<T>& operator += ( const BaseColor<T> &Val)
    {
        r += Val.r;
        g += Val.g;
        b += Val.b;
        a += Val.a;
        return *this;
    }
    BaseColor<T>& operator -= ( const BaseColor<T> &Val)
    {
        r -= Val.r;
        g -= Val.g;
        b -= Val.b;
        a -= Val.a;
        return *this;
    }
    BaseColor<T>& operator *= ( const T &Val)
    {
        r *= Val.r;
        g *= Val.g;
        b *= Val.b;
        a *= Val.a;
        return *this;
    }
    BaseColor<T>& operator /= ( const T &Val)
    {
        r /= Val.r;
        g /= Val.g;
        b /= Val.b;
        a /= Val.a;
        return *this;
    }
    bool operator == (const BaseColor<T> & Val) const
    {
        return r == Val.r && g == Val.g && b == Val.b && a == Val.a; 
    }
    bool operator != (const BaseColor<T> & Val) const
    {
        return !operator == (Val);
    }
    bool operator< (const BaseColor<T> & Val) const
    {
        if(r < Val.r)
            return true;
        else if(r > Val.r)
            return false;

        if(g < Val.g)
            return true;
        else if(g > Val.g)
            return false;

        if(b < Val.b)
            return true;
        else if(b > Val.b)
            return false;

        return a < Val.a;
    }
    bool operator> (const BaseColor<T> & Val) const
    {
        if(r > Val.r)
            return true;
        else if(r < Val.r)
            return false;

        if(g > Val.g)
            return true;
        else if(g < Val.g)
            return false;

        if(b > Val.b)
            return true;
        else if(b < Val.b)
            return false;

        return a > Val.a;
    }
};

template<class T>
struct Size
{
	T width, height;
	Size(T Width, T Height):width(Width), height(Height){}
	Size() :width(T()), height(T()){}		
	Size<T> operator + ( const Size<T> &Val) const
	{
		return Size<T>(width + Val.width, height + Val.height);
	}
    Size<T> operator - ( const Size<T> &Val) const
	{
		return Size<T>(width - Val.width, height - Val.height);
	}
	Size<T> operator * ( const T &Val) const
	{
		return Size<T>(width * Val, height * Val);
	}
	Size<T> operator / ( const T &Val) const
	{
		return Size<T>(width / Val, height / Val);
	}
	Size<T>& operator += ( const Size<T> &Val)
	{
		width += Val.width;
		height += Val.height;
		return *this;
	}
	Size<T>& operator -= ( const Size<T> &Val)
	{
		width -= Val.width;
		height -= Val.height;
		return *this;
	}    
	Size<T>& operator *= ( const T &Val)
	{
		width *= Val;
		height *= Val;
		return *this;
	}        
	Size<T>& operator /= ( const T &Val)
	{
		width /= Val;
		height /= Val;
		return *this;
	}
	bool operator == (const Size<T> & Val) const
	{
		return width == Val.width && height == Val.height; 
	}
	bool operator != (const Size<T> & Val) const
	{
		return !operator == (Val);
	}
};

template <class T>
struct Size3
{
    T width, height, depth;
    Size3(T Width, T Height, T Depth) : width(Width), height(Height), depth(Depth){}
    Size3() : width(T()), height(T()), depth(T()){}
    Size3<T> operator + ( const Size3<T> &Val) const
    {
        return {width + Val.width, height + Val.height, depth + Val.depth};
    }
    Size3<T> operator - ( const Size3<T> &Val) const
    {
        return {width - Val.width, height - Val.height, depth - Val.depth};
    }
    Size3<T> operator * ( const T &Val) const
    {
        return {width * Val, height * Val, depth * Val, depth * Val};
    }
    Size3<T> operator / ( const T &Val) const
    {
        return {width / Val, height / Val, depth / Val};
    }
    Size3<T>& operator += ( const Size3<T> &Val)
    {
        width += Val.width;
        height += Val.height;
        depth += Val.depth;
        return *this;
    }
    Size3<T>& operator -= ( const Size3<T> &Val)
    {
        width -= Val.width;
        height -= Val.height;
        depth -= Val.depth;
        return *this;
    }
    Size3<T>& operator *= ( const T &Val)
    {
        width *= Val;
        height *= Val;
        depth *= Val;
        return *this;
    }
    Size3<T>& operator /= ( const T &Val)
    {
        width /= Val;
        height /= Val;
        depth /= Val;
        return *this;
    }
    bool operator == (const Size3<T> & Val) const
    {
        return width == Val.width && height == Val.height && depth == Val.depth;
    }
    bool operator != (const Size3<T> & Val) const
    {
        return !operator == (Val);
    }
};

template<class T>
struct Range
{
	T minVal, maxVal;
	Range(T MinVal, T MaxVal) : minVal(MinVal), maxVal(MaxVal){}
	Range():minVal(T()),maxVal(T()){}
	Range<T> operator + ( const Range<T> &Val) const
	{
		return Range<T>(minVal + Val.minVal, maxVal + Val.maxVal);
	}
    Range<T> operator - ( const Range<T> &Val) const
	{
		return Range<T>(minVal - Val.minVal, maxVal - Val.maxVal);
	}
	Range<T> operator * ( const T &Val) const
	{
		return Range<T>(minVal * Val, maxVal * Val);
	}
	Range<T> operator / ( const T &Val) const
	{
		return Range<T>(minVal / Val, maxVal / Val);
	}
	Range<T>& operator += ( const Range<T> &Val)
	{
		minVal += Val.minVal;
		maxVal += Val.maxVal;
		return *this;
	}
	Range<T>& operator -= ( const Range<T> &Val)
	{
		minVal -= Val.minVal;
		maxVal -= Val.maxVal;
		return *this;
	}    
	Range<T>& operator *= ( const T &Val)
	{
		minVal *= Val;
		maxVal *= Val;
		return *this;
	}        
	Range<T>& operator /= ( const T &Val)
	{
		minVal /= Val;
		maxVal /= Val;
		return *this;
	}
	bool operator == (const Range<T> & Val) const
	{
		return minVal == Val.minVal && maxVal == Val.maxVal; 
	}
	bool operator != (const Range<T> & Val) const
	{
		return !operator == (Val);
	}
    bool In(const T &Val, bool CheckEquality = true) const
    {
        if(CheckEquality)
            return Val >= minVal && Val <= maxVal;
        else
            return Val > minVal && Val < maxVal;
    }
    bool Contains(const Range<T> &Val) const
    {
        return minVal < Val.minVal && maxVal > Val.maxVal;
    }
    void Update(const T &Val)
    {
        if(minVal > Val)
            minVal = Val;

        if(maxVal < Val)
            maxVal = Val;
    }
};

template<class T>
struct Rect
{
    Size<T> size;
    BasePoint2<T> pos;
    Rect(){}
    Rect(const BasePoint2<T> &Pos, const Size<T> &Size)
        : size(Size), pos(Pos) 
    {}
    bool operator == (const Rect<T> &Val) const
    {
        return size == Val.size && pos == Val.pos;
    }
    bool operator != (const Rect<T> & Val) const
    {
        return !operator == (Val);
    }
};

class Directions
{
private:
	std::vector<Point2> directions;
public:
	Directions()
	{
		directions.push_back(Point2(-1,  0));
		directions.push_back(Point2(-1, -1));
		directions.push_back(Point2( 0, -1));
		directions.push_back(Point2( 1, -1));						
		directions.push_back(Point2( 1,  0));
		directions.push_back(Point2( 1,  1));
		directions.push_back(Point2( 0,  1));
		directions.push_back(Point2(-1,  1));	
	}	
	const Point2 &operator [] (int Ind){return directions[Ind];}
};

#define Pi 3.141592654f //TODO make static const float Pi 

template<class T, class TVar>
T Cast(const BaseVector2<TVar> &Val){ return T(Val.x, Val.y);}

template<class T, class TVar>
T Cast(const BasePoint2<TVar> &Val){ return T(Val.x, Val.y);}

template<class T, class TVar>
T Cast(const Size<TVar> &Val){ return T(Val.width, Val.height);}

template<class T, class TVar>
T Cast(const Size3<TVar> &Val){ return T(Val.width, Val.height, Val.depth);}

template<class T, class TVal>
T Cast(const BasePoint3<TVal> &Val) {return T(Val.x, Val.y, Val.z);}

template <class T, class TVal>
T Cast(const BaseVector3<TVal> &Val) {return T(Val.x, Val.y, Val.z);}

template <class T, class TVal>
T Cast(const BasePoint4<TVal> &Val) {return T(Val.x, Val.y, Val.z, Val.w);}

template <class T, class TVal>
T Cast(const BaseColor<TVal> &Val) {return T(Val.r, Val.g, Val.b, Val.a);}

template<class T>
inline T Cast(const Point4F &Val){ return T(Val.x, Val.y, Val.z, Val.w);}

template<>
inline Vector3 Cast<Vector3>(const Point4F &Val) {return {Val.x, Val.y, Val.z};}

template<>
inline Point3F Cast<Point3F>(const Point4F &Val) {return {Val.x, Val.y, Val.z};}

template<class T>
inline T Cast(const Point3F &Val) { return T(Val.x, Val.y, Val.z);}

template<>
inline Point2F Cast<Point2F>(const Point3F &Val) { return Point2F(Val.x, Val.y);}

template<>
inline Vector2 Cast<Vector2>(const Point3F &Val) {return Vector2(Val.x, Val.y);}

template<class T>
inline T Cast(const Vector3 &Val) { return T(Val.x, Val.y, Val.z);}

template<>
inline Point2F Cast<Point2F>(const Vector3 &Val) { return {Val.x, Val.y};}

template<>
inline Vector2 Cast<Vector2>(const Vector3 &Val) {return {Val.x, Val.y};}

template<class T>
float Disstance(const T &Point1, const T &Point2)
{
	T del = Point2 - Point1;
	return (del.x * del.x) + (del.y * del.y);
}