/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Vector2.h>
#include <stdint.h>

class Matrix4x4
{
private:
    union {
        struct {
            float a11, a12, a13, a14;
            float a21, a22, a23, a24;
            float a31, a32, a33, a34;
            float a41, a42, a43, a44;
        };
        float m[4][4];
    };
public:
    Matrix4x4();
    Matrix4x4(const Point4F &Part1, const Point4F &Part2, const Point4F &Part3, const Point4F &Part4, bool AsRows = true);
    float Determinant() const;
    Vector4 Transform(const Vector4 &V) const;
    Vector3 Transform(const Vector3 &V) const;
    Point3F Transform(const Point3F &V, bool DevideByW = false) const;
    void Inverse(){*this = Inverse(*this);}
    void Transpose(){*this = Transpose(*this);}
    float &operator() (int32_t R, int32_t C) {return m[R][C];}
    float operator() (int32_t R, int32_t C) const {return m[R][C];}
    Matrix4x4 operator *(float Val) const {return Mul(*this, Val);}
    Matrix4x4 operator *(const Matrix4x4 &Matrix) const{return Mul(*this, Matrix);}
    Matrix4x4 operator +(const Matrix4x4 &Matrix) const{return Add(*this, Matrix);}
    Matrix4x4 operator -(const Matrix4x4 &Matrix) const{return Sub(*this, Matrix);}
    static Vector3 Transform(const Matrix4x4 &Matrix, const Vector3 &V);
    static Point3F Transform(const Matrix4x4 &Matrix, const Point3F &V, bool DevideByW = false);
    static Vector4 Transform(const Matrix4x4 &Matrix, const Vector4 &V);
    static Matrix4x4 Inverse(const Matrix4x4 &Matrix);
    static Matrix4x4 Transpose(const Matrix4x4 &Matrix);    
    static Matrix4x4 Mul(const Matrix4x4 &Matrix, float Val);
    static Matrix4x4 Mul(const Matrix4x4 &A, const Matrix4x4 &B);
    static Matrix4x4 Add(const Matrix4x4 &A, const Matrix4x4 &B);
    static Matrix4x4 Sub(const Matrix4x4 &A, const Matrix4x4 &B);
    static Matrix4x4 Translation(const Point3F &Pos);
    static Matrix4x4 Scalling(const Size3F &Size);
    static Matrix4x4 Scalling(float Factor);
    static Matrix4x4 RotationX(float Angle);
    static Matrix4x4 RotationY(float Angle);
    static Matrix4x4 RotationZ(float Angle);
    static Matrix4x4 RotationYawPitchRoll(float Yaw, float Pitch, float Roll);
    static Matrix4x4 RotationYawPitchRoll(const Vector3 &Rotation);
    static Matrix4x4 PerspectiveFovLH(float FOV, float NearZ, float FarZ, float AspectRatio);
    static Matrix4x4 OrthoLH(float Width, float Height, float NearZ, float FarZ);
    static Matrix4x4 OrthoOffCenterLH(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ);
    static Matrix4x4 LookAtLH(const Point3F &Pos, const Point3F &Target, const Vector3 &Up);
    static Matrix4x4 VectorProjection(const Vector3 &V);
    static Matrix4x4 VectorCrossProduct(const Vector3 &V);
    static Matrix4x4 RotationAxis(const Vector3 &Axis, float Angle);
};