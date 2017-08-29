/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <Vector2.h>
#include <Exception.h>

namespace Math
{

template<class T>
inline T Lerp(const T &A, const T& B, float Factor)
{
    return A + (B - A) * Factor;
}

template<class T>
inline T CosInterpolation(const T &A, const T &B, float Factor)
{
    float angle = Factor * Pi;
    float cosFactor = 0.5f - cosf(angle) * 0.5f;

    return A + (B - A) * cosFactor;
}

template <class T>
inline T Sign(T Val)
{
    return Val < 0 ? -1 : 1;
}

inline float Sign(float Val)
{
    return Val < 0.0f ? -1.0f : 1.0f;
}

inline double Sign(double Val)
{
    return Val < 0.0 ? -1.0 : 1.0;
}

template <class T>
inline const T &Min(const T &A, const T &B)
{
    return A < B ? A : B;
}

template <class T>
inline const T &Max(const T &A, const T &B)
{
    return A > B ? A : B;
}

template<class T>
inline T Abs(T Val)
{
    return (T)abs(Val);
}

template <class T>
inline BasePoint2<T> Abs(const BasePoint2<T> &Point)
{
    return {(T)abs(Point.x), (T)abs(Point.y)};
}

template <class T>
inline BaseVector2<T> Abs(const BaseVector2<T> &Vector)
{
    return {(T)abs(Vector.x), (T)abs(Vector.y)};
}

template <class T>
inline Size<T> Abs(const Size<T> &Size)
{
    return {(T)abs(Size.width), (T)abs(Size.height)};
}

inline float Rand(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

inline float Rand(const Range<float> &Range)
{
    return Rand(Range.minVal, Range.maxVal);
}

inline INT Rand(INT Min, INT Max)
{
    return Min + rand() % (Max - Min);
}

inline float RandSNorm()
{
    return 2.0f * Rand(0.0f, 1.0f) - 1.0f;
}

inline float Round(float Val)
{
    float btm = floor(Val);

    return abs(Val - btm) >= 0.5f ? ceil(Val) : btm;
}

inline float FindPower(float Base, float Result)
{
    return logf(Result) / logf(Base);
}

inline bool Equals(const Point3F &A, const Point3F &B)
{
    return abs(A.x - B.x) < 0.00001f && 
           abs(A.y - B.y) < 0.00001f &&
           abs(A.z - B.z) < 0.00001f;
}

inline bool Equals(const Vector3 &A, const Vector3 &B)
{
    return abs(A.x - B.x) < 0.00001f && 
           abs(A.y - B.y) < 0.00001f &&
           abs(A.z - B.z) < 0.00001f;
}

inline D3DXMATRIX Inverse(const D3DXMATRIX &Matrix)
{
    D3DXMATRIX inv;    
    D3DXMatrixInverse(&inv, NULL, &Matrix);
    return inv;
}

inline D3DXMATRIX Transpose(const D3DXMATRIX &Matrix)
{
    D3DXMATRIX trans;
    D3DXMatrixTranspose(&trans, &Matrix);
    return trans;
}

inline D3DXVECTOR3 Normalize(const D3DXVECTOR3 &Vector)
{
    D3DXVECTOR3 vOut;
    D3DXVec3Normalize(&vOut, &Vector);
    return vOut;
}

inline D3DXVECTOR3 Cross(const D3DXVECTOR3 &A, const D3DXVECTOR3 &B)
{
    D3DXVECTOR3 vOut;
    D3DXVec3Cross(&vOut, &A, &B);
    return vOut;
}

inline FLOAT Dot(const D3DXVECTOR3 &A, const D3DXVECTOR3 &B)
{
    return D3DXVec3Dot(&A, &B);
}

inline FLOAT Dot(const D3DXVECTOR4 &A, const D3DXVECTOR4 &B)
{
    return D3DXVec4Dot(&A, &B);
}

inline D3DXVECTOR3 RotationAxis(const D3DXVECTOR3 &Point,
                                const D3DXVECTOR3 &Axis,
                                FLOAT Angle,
                                FLOAT UpVectorFactor = 1.0f,
                                FLOAT RightVectorFactor = 1.0f)
{
    D3DXVECTOR3 axisProj = Axis * Dot(Axis, Point);

    D3DXVECTOR3 right = Point - axisProj;

    D3DXVECTOR3 up = Cross(Axis, right);

    return axisProj + right * cosf(Angle) * RightVectorFactor + up * sinf(Angle) * UpVectorFactor;
}

inline Point3F RotationAxis(const Point3F &Point,
                            const Vector3 &Axis,
                            float Angle,
                            float UpVectorFactor = 1.0f,
                            float RightVectorFactor = 1.0f)
{
    Point3F axisProj = Axis * Vector3::Dot(Axis, Cast<Vector3>(Point));

    Vector3 right = Point - axisProj;

    Vector3 up = Vector3::Cross(Axis, right);

    return axisProj + right * cosf(Angle) * RightVectorFactor + up * sinf(Angle) * UpVectorFactor;
}

inline D3DXVECTOR3 TransformCoord(const D3DXVECTOR3 &A, const D3DXMATRIX &Matrix)
{
    D3DXVECTOR3 vOut;
    D3DXVec3TransformCoord(&vOut, &A, &Matrix);
    return vOut;
}

inline D3DXVECTOR3 TransformNormal(const D3DXVECTOR3 &A, const D3DXMATRIX &Matrix)
{
    D3DXVECTOR3 vOut;
    D3DXVec3TransformNormal(&vOut, &A, &Matrix);
    return vOut;
}

inline FLOAT Length(const D3DXVECTOR3 &V)
{
    return D3DXVec3Length(&V);
}

inline FLOAT Length(const D3DXVECTOR2 &V)
{
    return D3DXVec2Length(&V);
}

inline D3DXVECTOR4 Transform(const D3DXVECTOR4 &A, const D3DXMATRIX &Matrix)
{
    D3DXVECTOR4 vOut;
    D3DXVec4Transform(&vOut, &A, &Matrix);
    return vOut;
}

inline D3DXMATRIX RotationYawPitchRoll(const D3DXVECTOR3 &Rotation)
{
    D3DXMATRIX mRot;
    D3DXMatrixRotationYawPitchRoll(&mRot, Rotation.y, Rotation.x, Rotation.z);
    return mRot;
}

inline D3DXMATRIX PerspectiveFovLH(FLOAT FOV, FLOAT NearZ, FLOAT FarZ, FLOAT AspectRation)
{
    D3DXMATRIX projMatrix;
    D3DXMatrixPerspectiveFovLH(&projMatrix, FOV, AspectRation, NearZ, FarZ);
    return projMatrix;
}

inline D3DXMATRIX LookAtLH(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &Target, const D3DXVECTOR3 &Up)
{
    D3DXMATRIX mView;
    D3DXMatrixLookAtLH(&mView, &Pos, &Target,&Up);
    return mView;
}

inline D3DXMATRIX Identity()
{
    D3DXMATRIX mI;
    D3DXMatrixIdentity(&mI);
    return mI;
}

inline D3DXMATRIX RotationX(float Angle)
{
    D3DXMATRIX mRot;
    D3DXMatrixRotationX(&mRot, Angle);
    return mRot;
}

inline D3DXMATRIX RotationY(float Angle)
{
    D3DXMATRIX mRot;
    D3DXMatrixRotationY(&mRot, Angle);
    return mRot;
}

inline D3DXMATRIX RotationZ(float Angle)
{
    D3DXMATRIX mRot;
    D3DXMatrixRotationZ(&mRot, Angle);
    return mRot;
}

inline D3DXMATRIX Translation(const D3DXVECTOR3 &Translation)
{
    D3DXMATRIX mTrans;
    D3DXMatrixTranslation(&mTrans, Translation.x, Translation.y, Translation.z);
    return mTrans;
}

inline D3DXMATRIX Scaling(const D3DXVECTOR3 &Scalling)
{
    D3DXMATRIX mScl;
    D3DXMatrixScaling(&mScl, Scalling.x, Scalling.y, Scalling.z);
    return mScl;
}

template <class T>
inline T Saturate(const T &Val, const Range<T> &Constraints)
{
    if(Val > Constraints.maxVal)
        return Constraints.maxVal;
    else if(Val < Constraints.minVal)
        return Constraints.minVal;
    else 
        return Val;
}

inline float Saturate(float Val)
{
    if(Val > 1.0f)
        return 1.0f;
    else if(Val < 0.0f)
        return 0.0f;
    else 
        return Val;
}

DECLARE_EXCEPTION(DirectionToAngleException);

inline float DirectionToAngle(const Point2F &Point)
{
    bool xIsZero = abs(Point.x) < 0.0001f;
    bool yIsZero = abs(Point.y) < 0.0001f;

    if(xIsZero || yIsZero){

        if(xIsZero && Point.y > 0.0f)
            return Pi * 0.5f;
        else if(Point.x < 0.0f && yIsZero)
            return Pi;
        else if(xIsZero && Point.y < 0.0f)
            return Pi * 1.5f;
        else if(Point.x > 0.0f && yIsZero)
            return Pi * 2.0f;
        else 
            throw DirectionToAngleException("cant get angle from direction");

    }else{
        float angle = atan(Point.y / Point.x);

        if(Point.x < 0)
            angle = Pi + angle;
        else if(Point.x > 0 && Point.y < 0.0f)
            angle = Pi * 2.0f + angle;

        return angle;
    }
}

inline float Cos(float Angle)
{
    return (abs(Angle) == Pi * 0.5f || abs(Angle) == Pi * 1.5f) ? 0.0f : cosf(Angle);
}

inline float Sin(float Angle)
{
    return (abs(Angle) == Pi * 1.0f || abs(Angle) == Pi * 2.0f) ? 0.0f : sinf(Angle);
}

inline Point3F SphericalToDec(float AngleX, float AngleY, float Radius = 1.0f)
{
    float cAx = Cos(AngleX), cAy = Cos(AngleY);
    float sAx = Sin(AngleX), sAy = Sin(AngleY);

    Point3F out;
    out.x = cAx * sAy * Radius;
    out.y = cAy * Radius;
    out.z = sAx * sAy * Radius;

    return out;
}

inline Point3F SphericalToDec(const Point2F &Point, float Radius = 1.0f)
{
    return SphericalToDec(Point.x, Point.y, Radius);
}

inline void ComputeGramSchmidtBasis(Vector3 &V1, Vector3 &V2, Vector3 &V3)
{
    Vector3 u1 = Vector3::Normalize(V1);
    Vector3 u2 = Vector3::Normalize(V2 - u1 * Vector3::Dot(u1, V2));
    Vector3 u3 = Vector3::Normalize(V3 - u1 * Vector3::Dot(u1, V3) - u2 * Vector3::Dot(u2, V3));

    V1 = u1;
    V2 = u2;
    V3 = u3;
}

}